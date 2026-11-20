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

// The single-game libraries' glue to their APK's Java side on Android (ss-delta 09-android), in place
// of Vita3K's own Android app (android/jni), which the games don't build:
//   * the surface: the renderer draws only while the Java SurfaceView has one. The Java class
//     io.github.vitarecomps.frontend.GameSurface (game-frontend/android/java) reports it; every
//     game's APK compiles that class, so the JNI name below is the same for all of them;
//   * the soft keyboard the game's text dialogs ask for (ime/keyboard.h): SDL's text input.

#ifdef __ANDROID__

#include <ime/keyboard.h>
#include <renderer/vulkan/screen_renderer.h>
#include <util/android_driver.h>
#include <util/fs.h>
#include <util/log.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_system.h>
#include <miniz.h>
#include <spdlog/spdlog.h>

#include <jni.h>

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {
SDL_Window *s_window = nullptr;

// Android may kill a paused app without another word (swiped away from the recent apps): the log
// is written on a thread of its own, so its last lines are flushed when the app leaves the screen.
// An event watch, because SDL blocks the game's event loop while the app is paused.
bool SDLCALL flush_logs_on_pause(void *, SDL_Event *event) {
    switch (event->type) {
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
    case SDL_EVENT_TERMINATING:
    case SDL_EVENT_LOW_MEMORY:
        spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });
        break;
    default:
        break;
    }
    return true;
}
} // namespace

namespace game_frontend {

// Vulkan driver packages (libadrenotools zips, such as Turnip's <name>.adpkg.zip) that a PC put in
// <game_dir>/drivers/: unpacked into the app's internal files/driver/<name>/, where the driver loader
// takes them from ([debug] gpu_driver = <name>), when the zip is new or changed. A release APK can't
// be reached there from a PC (no run-as), so this is how a driver gets in (ss-delta 09-android).
void android_install_driver_packages(const fs::path &game_dir) {
    const fs::path source_dir = game_dir / "drivers";
    if (!fs::is_directory(source_dir))
        return;
    const fs::path driver_root = fs::path(SDL_GetAndroidInternalStoragePath()) / "driver";
    boost::system::error_code ec;
    for (const auto &entry : fs::directory_iterator(source_dir)) {
        const fs::path zip_path = entry.path();
        if (zip_path.extension() != ".zip")
            continue;
        std::string name = zip_path.stem().string(); // <name>.adpkg.zip -> <name>.adpkg -> <name>
        if (name.ends_with(".adpkg"))
            name.resize(name.size() - 6);
        const fs::path target = driver_root / name;
        const std::string stamp = fmt::format("{} {}", fs::file_size(zip_path), fs::last_write_time(zip_path));
        const fs::path marker = target / ".package";
        if (std::ifstream in{ fs_utils::path_to_utf8(marker) }; in) {
            std::stringstream recorded;
            recorded << in.rdbuf();
            if (recorded.str() == stamp)
                continue;
        }
        fs::remove_all(target, ec);
        fs::create_directories(target, ec);

        mz_zip_archive zip{};
        if (!mz_zip_reader_init_file(&zip, fs_utils::path_to_utf8(zip_path).c_str(), 0)) {
            LOG_ERROR("Driver package {}: not a zip ({})", zip_path, mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
            continue;
        }
        bool ok = true;
        for (mz_uint i = 0; ok && i < mz_zip_reader_get_num_files(&zip); i++) {
            mz_zip_archive_file_stat stat{};
            if (!mz_zip_reader_file_stat(&zip, i, &stat) || mz_zip_reader_is_file_a_directory(&zip, i))
                continue;
            const fs::path relative = fs::path(stat.m_filename).lexically_normal();
            if (!android_driver::is_safe_driver_relative_path(relative)) {
                LOG_ERROR("Driver package {}: unsafe entry {}", zip_path, stat.m_filename);
                ok = false;
                break;
            }
            const fs::path out = target / relative;
            fs::create_directories(out.parent_path(), ec);
            ok = mz_zip_reader_extract_to_file(&zip, i, fs_utils::path_to_utf8(out).c_str(), 0);
        }
        mz_zip_reader_end(&zip);
        if (!ok || !android_driver::resolve_custom_driver_library_name(target)) {
            LOG_ERROR("Driver package {} could not be unpacked into {}", zip_path, target);
            fs::remove_all(target, ec);
            continue;
        }
        std::ofstream(fs_utils::path_to_utf8(marker)) << stamp;
        LOG_INFO("Driver package {} unpacked: [debug] gpu_driver = {} uses it", zip_path.filename(), name);
    }
}

// The driver packages on the device, by the name [debug] gpu_driver takes: those unpacked into
// files/driver/ and those still waiting as zips in the game's drivers/ (unpacked at the next start)
std::vector<std::string> android_driver_names() {
    std::set<std::string> names;
    boost::system::error_code ec;
    const fs::path driver_root = fs::path(SDL_GetAndroidInternalStoragePath()) / "driver";
    if (fs::is_directory(driver_root, ec))
        for (const auto &entry : fs::directory_iterator(driver_root, ec))
            if (fs::exists(entry.path() / ".package", ec))
                names.insert(entry.path().filename().string());
    if (const char *external = SDL_GetAndroidExternalStoragePath()) {
        const fs::path drivers = fs::path(external) / "drivers";
        if (fs::is_directory(drivers, ec))
            for (const auto &entry : fs::directory_iterator(drivers, ec)) {
                if (entry.path().extension() != ".zip")
                    continue;
                std::string name = entry.path().stem().string();
                if (name.ends_with(".adpkg"))
                    name.resize(name.size() - 6);
                names.insert(name);
            }
    }
    return { names.begin(), names.end() };
}

// [debug] gpu_driver as Vita3K's custom driver name: auto is the package installed last from drivers/
// (the newest marker), or the system's driver when there is none; system (or empty) is the system's
std::string android_gpu_driver(const std::string &setting) {
    if (setting.empty() || setting == "system")
        return {};
    if (setting != "auto")
        return setting;
    const fs::path driver_root = fs::path(SDL_GetAndroidInternalStoragePath()) / "driver";
    std::string newest;
    std::time_t newest_time = 0;
    boost::system::error_code ec;
    if (fs::is_directory(driver_root)) {
        for (const auto &entry : fs::directory_iterator(driver_root)) {
            const fs::path marker = entry.path() / ".package";
            if (!fs::exists(marker))
                continue;
            const std::time_t t = fs::last_write_time(marker, ec);
            if (!ec && (newest.empty() || t > newest_time)) {
                newest = entry.path().filename().string();
                newest_time = t;
            }
        }
    }
    return newest;
}

// the frontend's window exists: the soft keyboard's window, the log flush on pause
void android_window_created(SDL_Window *window) {
    ime::set_sdl_window(window);
    static bool watching = false;
    if (!watching) {
        SDL_AddEventWatch(flush_logs_on_pause, nullptr);
        watching = true;
    }
}

} // namespace game_frontend

extern "C" JNIEXPORT void JNICALL
Java_io_github_vitarecomps_frontend_GameSurface_setSurfaceStatus(JNIEnv *, jobject, jboolean surface_present) {
    renderer::vulkan::set_android_surface(surface_present);
}

namespace ime {

void set_sdl_window(SDL_Window *window) {
    s_window = window;
}

void set_keyboard_active(bool active) {
    if (!s_window)
        return;
    if (active)
        SDL_StartTextInput(s_window);
    else
        SDL_StopTextInput(s_window);
}

void notify_ime_state_changed() {
}

} // namespace ime

#endif // __ANDROID__
