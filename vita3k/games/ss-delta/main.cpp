// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// ss-delta.exe: Soul Sacrifice Delta (PCSA00152 US, PCSF00532 EU) on the shared single-game frontend
// (game-frontend/profile.h). This file holds what is particular to the game: its settings, save
// import and export, the add-on unlock, Ad Hoc Play, its engine settings and its frame rate.

#include "../../game-frontend/button_labels.h"
#include "../../game-frontend/profile.h"
#include "dlc.h"
#include "saves.h"

#include <config/state.h>
#include <ctrl/state.h>
#include <emuenv/state.h>
#include <gxm/state.h>
#include <gxm/types.h>
#include <io/functions.h>
#include <io/state.h>
#include <mem/functions.h>
#include <mem/ptr.h>
#include <modules/module_parent.h>
#include <renderer/state.h>
#include <renderer/texture_cache.h>
#include <util/fs.h>
#include <util/log.h>
#include <util/net_utils.h>

#include <SDL3/SDL.h>
#include <stb_image_write.h>
// provides WinMain for the windowed executable, which calls main below
#include <SDL3/SDL_main.h>

#include <cmath>
#include <cstring>
#include <iterator>
#include <map>
#include <set>
#include <string>

namespace {

using game_frontend::IniSection;
using game_frontend::SettingSpec;
using Kind = SettingSpec::Kind;
namespace layout = game_frontend::layout;

// The regions the port runs, at update 1.30 (the frontend runs the one installed). Their eboots are
// one build, the EU one linked 31 s before the US one: the code is byte for byte the same, and the
// data differs only in build stamps (ss-delta docs/01-research.md, "EU version"). So every guest
// address below holds for both, and the recompiled modules are the same.
struct Region {
    const char *title_id;
    const char *name;
};
constexpr Region REGIONS[] = {
    { "PCSA00152", "US" },
    { "PCSF00532", "EU" },
};
constexpr const char *TITLE_ID = REGIONS[0].title_id;

const char *region_name(const std::string &title_id) {
    for (const auto &r : REGIONS)
        if (title_id == r.title_id)
            return r.name;
    return "unknown";
}

// [multiplayer] adapter: Vita3K's adhoc_addr, an index into this PC's network adapters with an
// address (net_utils::get_all_assigned_addrs, the list Vita3K's own settings show). The settings
// menu lists them as "address - name" (Vita3K's order; a long name is cut, not the address);
// without any, it stays a number.
SettingSpec adapter_setting() {
#ifdef __ANDROID__
    const char *const help = "The network adapter Ad Hoc Play uses: the one in the same network as the other players\n"
                             "(usually the Wi-Fi). 0 is the first of this device's adapters with an address.";
#else
    const char *const help = "The network adapter LAN play uses (network = lan): the one in the same network as the other\n"
                             "PCs. 0 is the first of this PC's adapters with an address.";
#endif
    SettingSpec s{ "adapter", Kind::Int, "0", 0, 63, {}, help, true };
    s.title = "Network adapter";
    const auto addrs = net_utils::get_all_assigned_addrs();
    if (!addrs.empty()) {
        s.kind = Kind::Choice;
        for (size_t i = 0; i < addrs.size(); i++) {
            s.choices.push_back(std::to_string(i));
            s.choice_labels.push_back(addrs[i].name.empty() ? addrs[i].addr : addrs[i].addr + " - " + addrs[i].name);
        }
    }
    return s;
}

// [game] language: the Vita's system language, which the game reads once at boot. It has no
// language option of its own: the US region shows French, Spanish or Portuguese (Brazil) for those
// system languages and English for any other; the EU region French, Spanish, German or Italian,
// and English (UK) otherwise (ss-delta docs/01-research.md, "Text language").
const std::map<std::string, SceSystemParamLang> LANGUAGES{
    { "english", SCE_SYSTEM_PARAM_LANG_ENGLISH_US },
    { "french", SCE_SYSTEM_PARAM_LANG_FRENCH },
    { "spanish", SCE_SYSTEM_PARAM_LANG_SPANISH },
    { "german", SCE_SYSTEM_PARAM_LANG_GERMAN },
    { "italian", SCE_SYSTEM_PARAM_LANG_ITALIAN },
    { "portuguese", SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR },
};

SettingSpec language_setting() {
    SettingSpec s{ "language", Kind::Choice, "english", 0, 0, { "english", "french", "spanish", "german", "italian", "portuguese" },
        "The game's language, also in the system dialogs: english, french, spanish, german, italian,\n"
        "portuguese. The US game has English, French, Spanish and Portuguese (Brazil), the EU game\n"
        "English, French, Spanish, German and Italian; one the installed game lacks gives English." };
    s.title = "Language";
    s.choice_labels = { "English", "French", "Spanish", "German (EU game)", "Italian (EU game)", "Portuguese (US game)" };
    return s;
}

game_frontend::IniLayout ini_layout() {
    const std::string id = "ss-delta", exe = id + game_frontend::executable_suffix;
    IniSection display{ "display", "",
        {
            { "full_3d", Kind::Bool, "true", 0, 0, {},
                "Draw the 3D at 960x544 like the menus and HUD (the game's own ResoDown 0). Off: 720x408,\n"
                "upscaled, as on the Vita." },
            layout::scale(),
            layout::window_scale(),
            layout::fullscreen(id, exe),
            layout::vsync(),
            { "frame_rate", Kind::Choice, "60", 0, 0, { "30", "60" },
                "The game's frame rate: 60, or 30 as on the Vita. The game keeps its speed at 60; loading\n"
                "screens still run at 30 (they don't finish at 60)." },
            layout::filter(),
            // off: no visible change on the shimmering ST17 ground (the owner, 2026-10-14), so its cause
            // there is something else (**hypothesis**: explicit-LOD sampling or geometry)
            layout::mipmaps(false),
        } };
    // the settings menu changes the frame rate while the game runs: on_tick reads it every tick
    for (auto &spec : display.settings) {
        if (spec.key == "frame_rate")
            spec.live = true;
        if (spec.key == "full_3d")
            spec.title = "Full-resolution 3D";
    }
    IniSection game{ "game", "",
        { { "dlc", Kind::Bool, "true", 0, 0, {},
            "Unlock the add-ons (raiments, hoods, additional and otherworldly pacts). Their content is in the\n"
            "game's own data since update 1.30; PSN, which recorded the purchases, is closed. The save's\n"
            "system.bin is changed once (a copy goes to ss-delta/save-backups/)." } } };
    game.settings[0].title = "Add-ons (DLC)";
    game.settings.push_back({ "skip_intro", Kind::Bool, "false", 0, 0, {},
        "Skip the health warning and the company logos at start: the game opens on its title screen." });
    game.settings.back().title = "Skip intro logos";
    game.settings.push_back(language_setting());
    game.settings.push_back(game_frontend::button_labels_setting());
    // the Undub patch (Japanese voices, ss-delta docs/01-research.md, "Undub patch") is a rePatch mod
    game.settings.push_back(game_frontend::layout::repatch());
    // [cheats] (GitHub issue #7): switched in the settings menu while the game runs (apply_cheats)
    IniSection cheats{ "cheats",
        "Cheats, all off by default. They change the game's values while it runs; on the Cheats page of\n"
        "the settings menu they switch at once. What they change is kept when the game saves (Lacrima,\n"
        "levels), so export a save first if you want to go back (--export-save). In Ad Hoc Play they\n"
        "change only this player.",
        {
            { "infinite_health", Kind::Bool, "false", 0, 0, {},
                "In quests, your health is refilled to its maximum as soon as it drops. A hit that takes it\n"
                "all at once can still bring you down." },
            { "infinite_offerings", Kind::Bool, "false", 0, 0, {},
                "In quests, offerings don't run out: each is refilled to the uses it started the quest with." },
            { "max_lacrima", Kind::Bool, "false", 0, 0, {}, "Keep Lacrima at 9,999,999." },
            { "max_levels", Kind::Bool, "false", 0, 0, {},
                "Raise the Life and Magic levels to 99, the highest. Not yet early in the story (the\n"
                "journal's first chapters, while Portraiture hides the Right Arm's levels)." },
            { "one_hit_kills", Kind::Bool, "false", 0, 0, {},
                "In quests, your attacks defeat ordinary enemies in one hit. Bosses take five: the game\n"
                "limits a hit on a boss to a fifth of its health." },
        } };
    for (auto &spec : cheats.settings)
        spec.live = true;
    cheats.settings[1].title = "Unlimited offerings";
    cheats.settings[2].title = "Max Lacrima";
    cheats.settings[3].title = "Max levels";
    cheats.settings[4].title = "One-hit kills";
    // Windows starts with local, so a first start doesn't raise a firewall prompt; a Steam Deck or a Mac is
    // rarely used for several copies on one machine, and with local its rooms never reached the network
#ifdef _WIN32
    const char *const network_default = "local";
#else
    const char *const network_default = "lan";
#endif
    IniSection multiplayer{ "multiplayer", "",
        {
            { "network", Kind::Choice, network_default, 0, 0, { "local", "lan" },
                "Ad Hoc Play (Librom, Network). local (the default on Windows): copies of the game on this\n"
                "machine play together. lan (the default on Linux and macOS): machines in one network play\n"
                "together (Windows, Linux and macOS mixed), as in Vita3K: the copies find each other by UDP\n"
                "broadcasts on the network adapter set by `adapter` (no address to enter). Windows: allow\n"
                "ss-delta.exe through the firewall. macOS: allow Local Network access when it asks.\n"
                "Start one copy per player: ss-delta.exe is player 1, and \"ss-delta.exe --player 2\"\n"
                "(run-player2.bat) player 2, up to 4. Each player has a Vita user and saves of its own (player\n"
                "1's are copied on its first start), and its log and config in ss-delta/player<N>/.\n"
                "ss-delta-player<N>.ini next to this file overrides settings for player N." },
            adapter_setting(),
            { "gamepad", Kind::Int, "0", 0, 4, {},
                "0: every gamepad drives the window in focus. 1-4: only that gamepad, also while the window is in\n"
                "the background (two players at one PC, a gamepad each; set it in ss-delta-player<N>.ini)." },
        } };
    // Android runs one copy of the game, so its Ad Hoc Play is always lan (network's default there)
    for (auto &spec : multiplayer.settings)
        if (spec.key == "network" || spec.key == "gamepad")
            spec.desktop_only = true;
    IniSection boot_config{ "boot_config",
        "Engine settings passed to the game's boot_config.ini (Key = value, case as in that file), for\n"
        "experiments; they win over the settings above. The installed file is not changed." };
    boot_config.free_keys = boot_config.keep_case = true;
    boot_config.body = ";FPS = 30\n";
    return {
        "ss-delta settings. Lines starting with ; are comments; delete the ; to use a line.",
        {
            layout::paths("PCSA00152 for the US game, PCSF00532 for the EU one", id, exe),
            display,
            layout::audio(),
            game,
            cheats,
            multiplayer,
            layout::recompiled(id),
            layout::debug(id),
            layout::keyboard(),
            layout::gamepad(),
            layout::touch("; Black Rite: touch and hold the Black Rite icon, lower left during a quest.\n"
                          "black_rite = 56, 494\nblack_rite_key = F\nblack_rite_gamepad = lefttrigger\n"),
            boot_config,
        },
    };
}

// The game reads its engine settings from app0:resource/boot_config.ini, "Key<tab>Value" lines in
// two blocks (development, then retail). Serves a copy, written to `dir`, in which each key of
// `values` has the given value everywhere it is set; the installed file stays as it is.
void override_boot_config(EmuEnvState &emuenv, const std::string &title_id, const fs::path &dir, const std::map<std::string, std::string> &values) {
    const fs::path installed = emuenv.vita_fs_path / "ux0" / "app" / title_id / "resource" / "boot_config.ini";
    // a rePatch mod's copy, when it has one (game_frontend::apply_repatch ran before)
    const auto modded = emuenv.io.read_overrides.find(read_override_key(installed));
    fs::ifstream in(modded == emuenv.io.read_overrides.end() ? installed : modded->second, std::ios::binary);
    if (!in) {
        LOG_WARN("boot_config.ini not found at {}; the game's own settings stay", installed);
        return;
    }
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::string out;
    std::set<std::string> found;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find('\n', pos);
        end = end == std::string::npos ? text.size() : end + 1;
        std::string line = text.substr(pos, end - pos);
        const auto tab = line.find('\t');
        if (tab != std::string::npos) {
            const auto it = values.find(line.substr(0, tab));
            if (it != values.end()) {
                const bool crlf = line.ends_with("\r\n");
                line = it->first + "\t" + it->second + (crlf ? "\r\n" : "\n");
                found.insert(it->first);
            }
        }
        out += line;
        pos = end;
    }
    for (const auto &[key, value] : values) {
        if (!found.contains(key))
            out += key + "\t" + value + "\r\n";
        LOG_INFO("boot_config.ini: {} {}", key, value);
    }
    fs::create_directories(dir);
    const fs::path copy = dir / "boot_config.ini";
    fs::ofstream(copy, std::ios::binary) << out;
    emuenv.io.read_overrides[read_override_key(installed)] = copy;
}

// With the 3D at 960x544 (full_3d), the game samples its depth buffer through vertex shaders that
// scale the coordinates by 720/736 and 408/416: right for the 720x408 mode, whose depth texture is
// padded to whole 32-pixel tiles (1472x832 samples), 2% short at 960x544, which needs no padding.
// Depth of field and haze then use the depth beside each pixel (bones in the hub blurred). Both
// values are in the shaders' literal table, a list of {u32 register, f32 value}, as consecutive
// entries; they are set to 1.0 there.
void unscale_depth_uvs(SceGxmProgram &program) {
    constexpr uint32_t x_scale = 0x3f7a6f4e; // 720 / 736
    constexpr uint32_t y_scale = 0x3f7b13b2; // 408 / 416
    constexpr uint32_t one = 0x3f800000;
    uint8_t *const bytes = reinterpret_cast<uint8_t *>(&program);
    int replaced = 0;
    for (uint32_t i = 0; i + 12 <= program.size; i += 4) {
        uint32_t x, y;
        std::memcpy(&x, bytes + i, 4);
        std::memcpy(&y, bytes + i + 8, 4);
        if (x == x_scale && y == y_scale) {
            std::memcpy(bytes + i, &one, 4);
            std::memcpy(bytes + i + 8, &one, 4);
            replaced++;
        }
    }
    LOG_INFO_IF(replaced, "full_3d: depth UV scale set to 1.0 in a {} shader", program.is_vertex() ? "vertex" : "fragment");
}

// The frame rate (docs/03-plan.md, 60 fps): the game paces its main loop with a frame timer
// created for 30 fps (0x812ab888: movs r0, #30), whose period in microseconds is the first u64 of
// the timer object (*(*0x81c957cc)), and flips every 2nd vblank (threshold at 0x81d3a630). Both
// are data, so they are changed in guest memory once the game has set them up. Game logic steps
// by the measured frame time, so it keeps its speed at 60 fps, but the loading screen does not
// finish with frames shorter than about 1/45 s: its "roading_loop" animation never gets told to
// stop. So loading screens run at 30 fps: the NowLoadingTask singleton (*0x81c9603c) shows one
// while its state (+0xa4) is 2, 3 or 5 (opening, "roading_loop", "roading_end"; 0x812b58ec). After
// a quest it plays the loop backwards (6) and rests in 4 until reset, so those stay at 60.
// The frame timer's period for a rate. At 60 it is 16000 us, not 16666: the game waits out the
// timer and then flips on the next vblank, and a host wait overshoots by up to about a millisecond,
// so with 16666 a few frames a second just missed their vblank and took two (57-58 fps on every GPU).
// The flip still waits for each vblank, so the game can't run faster than 60.
static uint64_t frame_period_us(int fps) {
    return fps == 60 ? 16000 : 1000000ull / fps;
}

// Called every frame; returns the rate it set (0: not yet).
int update_frame_rate(EmuEnvState &emuenv, int fps, int current) {
    auto &mem = emuenv.mem;
    const auto u32 = [&](Address a) -> uint32_t * { return is_valid_addr(mem, a) ? Ptr<uint32_t>(a).get(mem) : nullptr; };
    uint32_t *const holder = u32(0x81c957cc);
    if (!holder || !*holder)
        return 0;
    uint32_t *const timer = u32(*holder);
    uint32_t *const flip = u32(0x81d3a630);
    if (!timer || !*timer || !is_valid_addr(mem, *timer + 7) || !flip)
        return 0;
    uint64_t *const period = Ptr<uint64_t>(*timer).get(mem);
    const uint32_t *const loading_task = u32(0x81c9603c);
    const uint32_t *const loading_state = loading_task && *loading_task ? u32(*loading_task + 0xa4) : nullptr;
    const uint32_t state = loading_state ? *loading_state : 0;
    const int want = state == 2 || state == 3 || state == 5 ? 30 : fps;
    if (want == current && *period == frame_period_us(want))
        return current;
    if ((*period != frame_period_us(30) && *period != frame_period_us(fps)) || (*flip != 2 && *flip != 60u / fps)) {
        LOG_WARN("frame_rate: unexpected frame timer {} us / flip threshold {}; left alone", *period, *flip);
        return -1;
    }
    *period = frame_period_us(want);
    *flip = 60 / want;
    LOG_INFO("frame_rate: {} fps{} (loading state {}, frame timer {} us, flip every {} vblank)", want, want != fps ? " for a loading screen" : "", state, *period, *flip);
    return want;
}

// [game] skip_intro (ss-delta docs/01-research.md, "Boot logos"): the boot's health warning and
// logos are GameLogoTasks that GameLogoControlTask (the singleton *0x81c957d0, built by 0x812ad8ce)
// keeps in a list (+0x50, count +0x48) and shows one after another. Each logo plays a timeline of
// floats in seconds: start delay +0x50, fade in +0x54, hold +0x58, fade out +0x5c, black +0x60;
// it is done (state +0x44 = 3) at its end, and the control task moves on, then starts the title.
// The timelines are data: set to 0, each logo ends on its first frame, before its fade-in. The
// vtables are checked, so nothing else is written. The constructor sets the singleton before it
// fills the list, so the list is read once the control task's init has run (its state +0x44 is 1,
// then 2 when all are shown); the first logo then waits out its start delay (1/6 s) at most.
// Called every frame; returns the control task handled.
Address skip_intro_logos(EmuEnvState &emuenv, Address done) {
    constexpr Address CONTROL_HOLDER = 0x81c957d0;
    constexpr uint32_t CONTROL_VTABLE = 0x81c5ede0, LOGO_VTABLE = 0x81c5ee14;
    constexpr uint32_t CONTROL_RUNNING = 1, LOGO_DONE = 3;
    auto &mem = emuenv.mem;
    const auto u32 = [&](Address a) -> uint32_t * { return is_valid_addr(mem, a) ? Ptr<uint32_t>(a).get(mem) : nullptr; };
    const uint32_t *const holder = u32(CONTROL_HOLDER);
    if (!holder || !*holder || *holder == done)
        return done;
    const Address control = *holder;
    const uint32_t *const vtable = u32(control);
    const uint32_t *const state = u32(control + 0x44);
    const uint32_t *const count = u32(control + 0x48);
    const uint32_t *const list = u32(control + 0x50);
    if (!vtable || *vtable != CONTROL_VTABLE || !state || !count || !list)
        return done;
    if (*state != CONTROL_RUNNING)
        return *state > CONTROL_RUNNING ? control : done;
    if (!*list || !*count || *count > 8)
        return control;
    int skipped = 0;
    for (uint32_t i = 0; i < *count; i++) {
        const uint32_t *const entry = u32(*list + 4 * i);
        if (!entry || !*entry || !is_valid_addr(mem, *entry + 0x63))
            continue;
        uint32_t *const logo = Ptr<uint32_t>(*entry).get(mem);
        if (logo[0] != LOGO_VTABLE || logo[0x44 / 4] == LOGO_DONE)
            continue;
        for (uint32_t field = 0x50; field <= 0x60; field += 4)
            logo[field / 4] = 0; // 0.0f
        skipped++;
    }
    LOG_INFO("skip_intro: {} of {} boot logos skipped", skipped, *count);
    return control;
}

// [cheats] (GitHub issue #7; ss-delta docs/01-research.md, "Cheats"): values the game keeps in
// guest memory, set every frame while a cheat is on. Native code stays on, as for skip_intro.
// Addresses from the VitaCheat codes for 1.30 (r0ah, tomberyx), checked in our build:
//   * the quest's player: *(*0x81c98f00 + 0x18), set during a quest only (0 otherwise); the player
//     object's vtable is 0x81c61f6c, the quest's 0x81c7a40c. HP is a float at +0x58, its maximum an
//     int at +0xf8. Offering slots 0-5: uses left at +0x200 + 4 * slot (-1: empty slot, 9999: no
//     limit), the uses each started the quest with at +0x218 + 4 * slot.
//   * the save data: *0x81c98618 (the game's own pointer, set from boot). Lacrima at +0x8c, the
//     Life and Magic levels (u16, 1-99, boot/lowchaos_growth_param.csv) at +0x17ced8 and +0x17ceda.
struct CheatState {
    std::set<std::string> logged; // each cheat logs once per object (player, save data) it changes
};

constexpr uint32_t LACRIMA_MAX = 9999999, LEVEL_MAX = 99;

void apply_cheats(EmuEnvState &emuenv, const game_frontend::Values &values, CheatState &state) {
    constexpr Address QUEST_HOLDER = 0x81c98f00, SAVE_HOLDER = 0x81c98618;
    constexpr uint32_t QUEST_VTABLE = 0x81c7a40c, PLAYER_VTABLE = 0x81c61f6c;
    auto &mem = emuenv.mem;
    const auto u32 = [&](Address a) -> uint32_t * { return is_valid_addr_range(mem, a, a + 4) ? Ptr<uint32_t>(a).get(mem) : nullptr; };
    const auto once = [&](const std::string &what, Address object) {
        if (state.logged.insert(fmt::format("{} {:#x}", what, object)).second)
            LOG_INFO("cheats: {} ({:#x})", what, object);
    };

    const bool health = values.get_bool("cheats.infinite_health");
    const bool offerings = values.get_bool("cheats.infinite_offerings");
    const bool one_hit = values.get_bool("cheats.one_hit_kills");
    if (health || offerings || one_hit) {
        const uint32_t *const quest_holder = u32(QUEST_HOLDER);
        const uint32_t *const quest = quest_holder && *quest_holder ? u32(*quest_holder) : nullptr;
        const uint32_t *const player_ref = quest && *quest == QUEST_VTABLE ? u32(*quest_holder + 0x18) : nullptr;
        const Address player = player_ref ? *player_ref : 0;
        const uint32_t *const vtable = player ? u32(player) : nullptr;
        if (vtable && *vtable == PLAYER_VTABLE && is_valid_addr_range(mem, player, player + 0x400)) {
            uint8_t *const p = Ptr<uint8_t>(player).get(mem);
            if (health) {
                float hp;
                int32_t max_hp;
                std::memcpy(&hp, p + 0x58, 4);
                std::memcpy(&max_hp, p + 0xf8, 4);
                // not once health is 0: the game has started the player's death (or a rescue) by then
                if (hp > 0 && max_hp > 0 && max_hp < 1000000 && hp < static_cast<float>(max_hp)) {
                    const float full = static_cast<float>(max_hp);
                    std::memcpy(p + 0x58, &full, 4);
                    once("infinite health", player);
                }
            }
            if (offerings) {
                for (int slot = 0; slot < 6; slot++) {
                    int32_t left, start;
                    std::memcpy(&left, p + 0x200 + 4 * slot, 4);
                    std::memcpy(&start, p + 0x218 + 4 * slot, 4);
                    if (left >= 0 && start > 0 && start < 9999 && left < start) {
                        std::memcpy(p + 0x200 + 4 * slot, &start, 4);
                        once("unlimited offering uses", player);
                    }
                }
            }
            if (one_hit) {
                // tomberyx's "One Hit Kill": 1.07e20 at +0x370 (a damage factor, **hypothesis**), which the
                // game sets back from time to time. Ordinary enemies then fall to one hit; the game still
                // limits a hit on a boss to a fifth of its health
                constexpr uint32_t ONE_HIT = 0x60babe00;
                uint32_t current;
                std::memcpy(&current, p + 0x370, 4);
                if (current != ONE_HIT) {
                    std::memcpy(p + 0x370, &ONE_HIT, 4);
                    once("one-hit kills", player);
                }
            }
        }
    }

    const bool lacrima = values.get_bool("cheats.max_lacrima");
    const bool levels = values.get_bool("cheats.max_levels");
    if (lacrima || levels) {
        const uint32_t *const save_holder = u32(SAVE_HOLDER);
        const Address save = save_holder ? *save_holder : 0;
        if (save && is_valid_addr_range(mem, save, save + 0x17cedc)) {
            uint8_t *const s = Ptr<uint8_t>(save).get(mem);
            uint32_t count;
            uint16_t life, magic;
            std::memcpy(&count, s + 0x8c, 4);
            std::memcpy(&life, s + 0x17ced8, 2);
            std::memcpy(&magic, s + 0x17ceda, 2);
            // values the game itself could hold, so a wrong pointer is not written through (at boot,
            // before a save is loaded, the block holds anything). Levels are 0 until the Right Arm's
            // levels open, early in the story.
            const bool plausible = count <= LACRIMA_MAX && life <= LEVEL_MAX && magic <= LEVEL_MAX;
            if (!plausible) {
                if (state.logged.insert(fmt::format("implausible {:#x}", save)).second)
                    LOG_WARN("cheats: unexpected save data at {:#x} (Lacrima {}, levels {} / {}); left alone", save, count, life, magic);
            } else {
                if (lacrima && count != LACRIMA_MAX) {
                    std::memcpy(s + 0x8c, &LACRIMA_MAX, 4);
                    once("max Lacrima", save);
                }
                // Early in the story (the journal's own chapters) these are 0 and the levels in play are in
                // another record, at a place that depends on the save (+0x139fa8 in one); left alone then
                if (levels && (!life || !magic) && state.logged.insert(fmt::format("no levels {:#x}", save)).second)
                    LOG_INFO("cheats: max levels: this save's levels aren't at the known place yet (early in the story); left alone");
                if (levels && life && magic && (life != LEVEL_MAX || magic != LEVEL_MAX)) {
                    const uint16_t max = LEVEL_MAX;
                    std::memcpy(s + 0x17ced8, &max, 2);
                    std::memcpy(s + 0x17ceda, &max, 2);
                    once("max levels", save);
                }
            }
        }
    }
}

// the game's textures with PlayStation buttons (the com_001_* icon atlases, tutorial pictures, the
// font page with the text's symbols) and the symbols in them, all languages (ss-delta
// tools/button-atlases.py; docs/01-research.md, "Button prompts")
std::vector<game_frontend::ButtonAtlas> button_atlases() {
    using Symbol = game_frontend::ButtonSymbol;
    return {
#include "button_atlases.inc"
    };
}

// --button-labels-test <in> <out>: the labels drawn on the one-texture GXT files in <in> (extracted
// from the game's archives, tools/lspk.py) as PNGs in <out>, at [display] scale; [gamepad] as set
int button_labels_test(game_frontend::ButtonLabels &labels, const game_frontend::Settings &settings, const fs::path &in, const fs::path &out) {
    labels.configure("xbox", settings.gamepad, static_cast<int>(std::lround(settings.scale)));
    fs::create_directories(out);
    int written = 0;
    for (const auto &entry : fs::directory_iterator(in)) {
        if (entry.path().extension() != ".gxt")
            continue;
        std::vector<uint8_t> data;
        if (!fs_utils::read_data(entry.path(), data) || data.size() < 64)
            continue;
        // GXT: magic, version, texture count, ...; the first texture's offset, size, palette, flags,
        // type, format, width | height << 16
        uint32_t header[15];
        std::memcpy(header, data.data(), sizeof(header));
        const uint32_t offset = header[8], size = header[9], type = header[12];
        const auto format = static_cast<SceGxmTextureBaseFormat>(header[13] & 0x9f000000u);
        const uint32_t width = header[14] & 0xffff, height = header[14] >> 16;
        if (header[2] != 1 || type != SCE_GXM_TEXTURE_SWIZZLED || static_cast<size_t>(offset) + size > data.size() || !labels.wants(format, width, height))
            continue;
        std::vector<uint8_t> rgba;
        uint32_t w = 0, h = 0;
        if (!labels.make(format, width, height, data.data() + offset, size, rgba, w, h))
            continue;
        const fs::path png = out / (fs_utils::path_to_utf8(entry.path().stem()) + ".png");
        if (stbi_write_png(fs_utils::path_to_utf8(png).c_str(), w, h, 4, rgba.data(), w * 4))
            written++;
    }
    LOG_INFO("--button-labels-test: {} pictures in {}", written, fs_utils::path_to_utf8(out));
    return written > 0 ? 0 : 1;
}

// --import-save / --export-save
struct SaveCommand {
    std::string import_from, export_to;
    int slot = 1;
    bool quiet = false;
};

} // namespace

int main(int argc, char *argv[]) {
    // the game's own command line, besides the frontend's (game-frontend/frontend.cpp):
    //   --import-save <folder|zip> copy a save (decrypted, e.g. from a Vita) in, keeping the old one
    //   --export-save <folder>     copy a save out
    //   --slot <1-3>               the game's save slot for the two above (default 1)
    //   --quiet                    no message box for the two above (the result is in the log)
    SaveCommand save;
    std::string labels_test_in, labels_test_out; // --button-labels-test
    // [game] button_labels: the textures with PlayStation buttons redrawn with Xbox labels as they are
    // uploaded (the renderer exists from the first ticks, before the game loads them); the settings menu
    // switches it
    game_frontend::ButtonLabels button_labels(button_atlases());
    int frame_rate = 30; // what update_frame_rate last set (-1: left alone)
    Address intro_skipped = 0; // the logo control task skip_intro_logos handled
    CheatState cheats;

    game_frontend::Profile profile;
    profile.title_id = TITLE_ID;
    for (const auto &r : REGIONS)
        if (r.title_id != profile.title_id)
            profile.other_title_ids.push_back(r.title_id);
    profile.name = "Soul Sacrifice Delta";
    profile.id = "ss-delta";
    profile.ini = ini_layout();
    profile.default_touches = { { "black_rite", 56, 494, "F", "lefttrigger" } };
    profile.multiplayer = true;
    profile.icon_resource = true;
    profile.usage = "Saves (then quit):\n"
                    "  --import-save <folder|zip>  copy a decrypted Vita save (e.g. from a Vita) in, keeping the\n"
                    "                              old one in ss-delta/save-backups/\n"
                    "  --export-save <folder>      copy a save out\n"
                    "  --slot <1-3>                the game's save slot for the two above (default 1)\n"
                    "  --quiet                     no message box for the two above (the result is in the log)\n"
                    "Development:\n"
                    "  --button-labels-test <in> <out>  the Xbox button labels drawn on the textures (.gxt) in\n"
                    "                              <in>, as PNGs in <out>; then quit\n";

    auto &hooks = profile.hooks;
    hooks.parse_arg = [&](game_frontend::Context &, const std::vector<std::string> &args, size_t &i) {
        const std::string &arg = args[i];
        const bool has_value = i + 1 < args.size();
        if (arg == "--import-save" && has_value)
            save.import_from = args[++i];
        else if (arg == "--export-save" && has_value)
            save.export_to = args[++i];
        else if (arg == "--slot" && has_value)
            save.slot = std::atoi(args[++i].c_str());
        else if (arg == "--button-labels-test" && i + 2 < args.size()) {
            labels_test_in = args[++i];
            labels_test_out = args[++i];
        } else if (arg == "--quiet")
            save.quiet = true;
        else
            return false;
        return true;
    };
    hooks.before_init = [&](game_frontend::Context &ctx) -> std::optional<int> {
        ss_delta::set_save_user(ctx.user_id);
        // the frontend has picked the installed region's title ID by now
        ss_delta::set_save_title(profile.title_id);
        LOG_INFO("Region: {} ({})", region_name(profile.title_id), profile.title_id);
        if (!labels_test_in.empty())
            return button_labels_test(button_labels, *ctx.settings, fs_utils::utf8_to_path(labels_test_in), fs_utils::utf8_to_path(labels_test_out));
        if (save.import_from.empty() && save.export_to.empty())
            return std::nullopt;
        const auto &vita_fs = ctx.settings->vita_fs;
        std::string message;
        const bool ok = !save.import_from.empty()
            ? ss_delta::import_save(vita_fs, fs_utils::utf8_to_path(save.import_from), save.slot, ctx.data_dir / "save-backups", message)
            : ss_delta::export_save(vita_fs, fs_utils::utf8_to_path(save.export_to), save.slot, message);
        if (ok)
            LOG_INFO("Save: {}", message);
        else
            LOG_ERROR("Save: {}", message);
        if (!save.quiet)
            SDL_ShowSimpleMessageBox(ok ? SDL_MESSAGEBOX_INFORMATION : SDL_MESSAGEBOX_ERROR, ctx.window_name.c_str(),
                ("Save: " + message).c_str(), nullptr);
        return ok ? 0 : 1;
    };
    hooks.configure = [&](game_frontend::Context &ctx, Config &cfg) {
        const auto &values = ctx.settings->values;
        // Ad Hoc Play: copies on this PC talk over 127.0.0.<player>; on a network, through an
        // adapter (Vita3K's adhoc-addr). Players 2 and up only read the shader cache player 1 keeps.
        net_utils::set_local_adhoc(values.get("multiplayer.network") == "lan" ? 0 : ctx.player);
        cfg.adhoc_addr = values.get_int("multiplayer.adapter");
        cfg.shader_cache_read_only = ctx.player > 1;
        // the fog colour leaves the vertex shaders as COLOR0/COLOR1 and goes above 1 (blue haze in ST02A
        // without the clamp; grey, as on the Vita, with it)
        cfg.clamp_color_varyings = true;
        // [game] language: the system language the game asks SceAppUtil for at boot
        cfg.sys_lang = LANGUAGES.at(values.get("game.language"));
        // [multiplayer] gamepad: one gamepad per copy of the game, read also while its window is in
        // the background
        if (values.get_int("multiplayer.gamepad") > 0)
            SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    };
    hooks.before_run = [&](game_frontend::Context &ctx) {
        EmuEnvState &emuenv = *ctx.emuenv;
        const auto &settings = *ctx.settings;
        const auto &values = settings.values;
        emuenv.ctrl.only_gamepad = values.get_int("multiplayer.gamepad");
        if (ctx.player > 1) {
            std::string message;
            if (ss_delta::copy_save_to_user(settings.vita_fs, "00", ctx.user_id, message))
                LOG_INFO("Save: {}", message);
            else
                LOG_ERROR("Save: {}", message);
        }
        // add-ons: the game reads what was bought on PSN from the save's system.bin; the content
        // itself is in the game's data (docs/decisions.md D14 in ss-delta)
        if (values.get_bool("game.dlc")) {
            std::string why;
            if (ss_delta::addon_data_present(emuenv.vita_fs_path / "ux0" / "app" / profile.title_id, why)) {
                std::string message;
                if (ss_delta::unlock_addons(settings.vita_fs, ctx.data_dir / "save-backups", message))
                    LOG_INFO("Add-ons: {}", message);
                else
                    LOG_ERROR("Add-ons: {}", message);
            } else
                LOG_WARN("Add-ons: not unlocked, the installed game lacks them ({})", why);
        }
        // the game's own engine settings that ss-delta.ini changes
        const bool full_3d = values.get_bool("display.full_3d");
        std::map<std::string, std::string> boot_config{ { "ResoDown", full_3d ? "0" : "1" } };
        for (const auto &[key, value] : values.section("boot_config"))
            boot_config[key] = value;
        override_boot_config(emuenv, profile.title_id, ctx.data_dir / "game", boot_config);
        if (full_3d)
            emuenv.gxm.on_program_registered = unscale_depth_uvs;
    };
    hooks.on_tick = [&](game_frontend::Context &ctx) {
        const int fps = ctx.settings->values.get_int("display.frame_rate");
        if (fps != 30 && frame_rate != -1)
            frame_rate = update_frame_rate(*ctx.emuenv, fps, frame_rate);
        if (ctx.settings->values.get_bool("game.skip_intro"))
            intro_skipped = skip_intro_logos(*ctx.emuenv, intro_skipped);
        apply_cheats(*ctx.emuenv, ctx.settings->values, cheats);
        if (renderer::TextureCache *cache = ctx.emuenv->renderer ? ctx.emuenv->renderer->get_texture_cache() : nullptr) {
            // auto: from the first connected gamepad's own labels (it may change while the game runs)
            std::string style = ctx.settings->values.get("game.button_labels");
            std::map<std::string, std::string> face;
            if (style == "auto")
                style = game_frontend::auto_button_style(ctx.emuenv->ctrl, face);
            const bool changed = button_labels.configure(style, ctx.settings->gamepad, static_cast<int>(std::lround(ctx.settings->scale)), face);
            if (cache->texture_override.load() != &button_labels) {
                cache->texture_override = &button_labels;
                cache->refresh_texture_override();
            } else if (changed)
                cache->refresh_texture_override();
        }
    };
    hooks.before_dump_quit = [&](game_frontend::Context &ctx) {
        // Ad Hoc Play's matching module loads only when that menu opens: load it now so it is
        // written too (it is recompiled for the address it takes in play, tools/install-game.ps1)
        if (!load_sys_module(*ctx.emuenv, SCE_SYSMODULE_NET_ADHOC_MATCHING))
            LOG_ERROR("--dump-elfs: the ad hoc matching module could not be loaded");
    };

    return game_frontend::run(profile, argc, argv);
}
