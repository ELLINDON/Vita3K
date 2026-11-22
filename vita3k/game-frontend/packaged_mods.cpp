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

#include "packaged_mods.h"

#include <util/log.h>
#include <util/string_utils.h>

#include <fmt/ranges.h>
#include <miniz.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <map>
#include <regex>
#include <set>

namespace game_frontend {

namespace {

constexpr const char *MANIFEST = "ss-delta-mod.json";

// --- LSPK archives: <base>.pkh, a big-endian count and 16-byte entries (hash, offset, size,
// compressed size or 0) sorted by hash; <base>.pk, the data (zlib when compressed) at 16-byte
// offsets. The hash is CRC-32/BZIP2 of the lower-cased path (tools/lspk.py).

struct Entry {
    uint32_t hash, off, size, csize;
};
using Index = std::map<uint32_t, Entry>;
using Bytes = std::vector<uint8_t>;

uint32_t path_hash(const std::string &path) {
    static const auto table = [] {
        std::array<uint32_t, 256> t{};
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i << 24;
            for (int k = 0; k < 8; k++)
                c = (c & 0x80000000) ? (c << 1) ^ 0x04C11DB7 : c << 1;
            t[i] = c;
        }
        return t;
    }();
    uint32_t c = 0xffffffff;
    for (const char ch : path) {
        const uint8_t b = static_cast<uint8_t>(std::tolower(static_cast<unsigned char>(ch)));
        c = (c << 8) ^ table[((c >> 24) ^ b) & 0xff];
    }
    return c ^ 0xffffffff;
}

fs::path with_ext(const fs::path &base, const char *ext) {
    fs::path p = base;
    p += ext;
    return p;
}

uint32_t be32(const uint8_t *p) {
    return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3];
}

void put_be32(Bytes &out, uint32_t v) {
    out.insert(out.end(), { uint8_t(v >> 24), uint8_t(v >> 16), uint8_t(v >> 8), uint8_t(v) });
}

std::optional<Bytes> read_file(const fs::path &path) {
    fs::ifstream in(path, std::ios::binary);
    if (!in)
        return std::nullopt;
    return Bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

bool has_archive(const fs::path &base) {
    return fs::is_regular_file(with_ext(base, ".pkh")) && fs::is_regular_file(with_ext(base, ".pk"));
}

std::optional<Index> read_index(const fs::path &base) {
    const auto pkh = read_file(with_ext(base, ".pkh"));
    if (!pkh || pkh->size() < 4)
        return std::nullopt;
    const uint32_t count = be32(pkh->data());
    if (pkh->size() != 4 + 16ull * count)
        return std::nullopt;
    Index index;
    for (uint32_t i = 0; i < count; i++) {
        const uint8_t *e = pkh->data() + 4 + 16 * i;
        index[be32(e)] = { be32(e), be32(e + 4), be32(e + 8), be32(e + 12) };
    }
    return index;
}

std::optional<Bytes> read_raw(fs::ifstream &pk, const Entry &e) {
    Bytes raw(e.csize ? e.csize : e.size);
    pk.clear();
    pk.seekg(e.off);
    if (!raw.empty() && !pk.read(reinterpret_cast<char *>(raw.data()), raw.size()))
        return std::nullopt;
    return raw;
}

std::optional<Bytes> unpack(const Bytes &raw, const Entry &e) {
    if (!e.csize)
        return raw;
    Bytes data(e.size);
    mz_ulong n = e.size;
    if (mz_uncompress(data.data(), &n, raw.data(), static_cast<mz_ulong>(raw.size())) != MZ_OK || n != e.size)
        return std::nullopt;
    return data;
}

std::optional<Bytes> read_member(const fs::path &base, const Index &index, const std::string &path) {
    const auto it = index.find(path_hash(path));
    if (it == index.end())
        return std::nullopt;
    fs::ifstream pk(with_ext(base, ".pk"), std::ios::binary);
    if (!pk)
        return std::nullopt;
    const auto raw = read_raw(pk, it->second);
    return raw ? unpack(*raw, it->second) : std::nullopt;
}

// base's members, raw, except `drop` and those in `put`, then `put` compressed; written to out.
bool write_archive(const fs::path &base, const Index &index, const std::set<uint32_t> &drop,
    const std::map<uint32_t, Bytes> &put, const fs::path &out) {
    fs::ifstream src(with_ext(base, ".pk"), std::ios::binary);
    fs::ofstream dst(with_ext(out, ".pk"), std::ios::binary | std::ios::trunc);
    if (!src || !dst)
        return false;
    std::vector<Entry> entries;
    uint64_t pos = 0;
    const auto write = [&](uint32_t hash, const Bytes &raw, uint32_t size, uint32_t csize) {
        static const char zeros[16] = {};
        const uint64_t pad = (16 - pos % 16) % 16;
        dst.write(zeros, static_cast<std::streamsize>(pad));
        pos += pad;
        entries.push_back({ hash, static_cast<uint32_t>(pos), size, csize });
        dst.write(reinterpret_cast<const char *>(raw.data()), static_cast<std::streamsize>(raw.size()));
        pos += raw.size();
    };
    std::vector<Entry> by_offset;
    for (const auto &[hash, e] : index)
        if (!drop.contains(hash) && !put.contains(hash))
            by_offset.push_back(e);
    std::sort(by_offset.begin(), by_offset.end(), [](const Entry &a, const Entry &b) { return a.off < b.off; });
    for (const auto &e : by_offset) {
        const auto raw = read_raw(src, e);
        if (!raw)
            return false;
        write(e.hash, *raw, e.size, e.csize);
    }
    for (const auto &[hash, data] : put) {
        mz_ulong n = mz_compressBound(static_cast<mz_ulong>(data.size()));
        Bytes packed(n);
        if (mz_compress2(packed.data(), &n, data.data(), static_cast<mz_ulong>(data.size()), 9) != MZ_OK)
            return false;
        packed.resize(n);
        write(hash, packed, static_cast<uint32_t>(data.size()), static_cast<uint32_t>(n));
    }
    if (!dst || pos > 0xffffffffull)
        return false;
    dst.close();
    std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b) { return a.hash < b.hash; });
    Bytes pkh;
    put_be32(pkh, static_cast<uint32_t>(entries.size()));
    for (const auto &e : entries) {
        put_be32(pkh, e.hash);
        put_be32(pkh, e.off);
        put_be32(pkh, e.size);
        put_be32(pkh, e.csize);
    }
    fs::ofstream h(with_ext(out, ".pkh"), std::ios::binary | std::ios::trunc);
    h.write(reinterpret_cast<const char *>(pkh.data()), static_cast<std::streamsize>(pkh.size()));
    return static_cast<bool>(h);
}

// whether two archives hold the same members with the same contents
bool same_contents(const fs::path &a, const fs::path &b) {
    const auto ia = read_index(a), ib = read_index(b);
    if (!ia || !ib || ia->size() != ib->size())
        return false;
    fs::ifstream pa(with_ext(a, ".pk"), std::ios::binary), pb(with_ext(b, ".pk"), std::ios::binary);
    for (const auto &[hash, ea] : *ia) {
        const auto it = ib->find(hash);
        if (it == ib->end() || ea.size != it->second.size)
            return false;
        const auto ra = read_raw(pa, ea), rb = read_raw(pb, it->second);
        if (!ra || !rb)
            return false;
        if (ea.csize == it->second.csize ? *ra != *rb : unpack(*ra, ea) != unpack(*rb, it->second))
            return false;
    }
    return true;
}

// --- tables: lines of bytes (Shift JIS in this game), ended by \r\n

std::string line_of(const std::string &l) {
    return !l.empty() && l.back() == '\r' ? l.substr(0, l.size() - 1) : l;
}

// data without the lines in `lines`
std::string strip_lines(const std::string &data, const std::set<std::string> &lines) {
    std::string out;
    size_t pos = 0;
    while (pos < data.size()) {
        size_t end = data.find('\n', pos);
        end = end == std::string::npos ? data.size() : end + 1;
        const std::string whole = data.substr(pos, end - pos);
        std::string l = whole;
        if (!l.empty() && l.back() == '\n')
            l.pop_back();
        if (!lines.contains(line_of(l)))
            out += whole;
        pos = end;
    }
    return out;
}

std::string append_lines(std::string data, const std::vector<std::string> &lines) {
    if (!data.empty() && data.back() != '\n')
        data += "\r\n";
    for (const auto &l : lines)
        data += l + "\r\n";
    return data;
}

bool has_line_starting(const std::string &data, const std::string &prefix) {
    size_t pos = 0;
    while (pos < data.size()) {
        if (data.compare(pos, prefix.size(), prefix) == 0)
            return true;
        const size_t end = data.find('\n', pos);
        if (end == std::string::npos)
            break;
        pos = end + 1;
    }
    return false;
}

// --- packages

struct Append {
    std::string member;
    std::vector<std::string> lines;
    std::vector<std::string> conflicts;
};

struct Mod {
    fs::path dir;
    std::string id, name, version, archive, remove_note;
    std::vector<std::string> titles, tables_from;
    std::vector<Append> appends;
    std::map<std::string, fs::path> files; // path in the archive -> the file
};

bool safe_relative(const std::string &p) {
    if (p.empty() || p.front() == '/' || p.find(':') != std::string::npos)
        return false;
    size_t pos = 0;
    while (pos <= p.size()) {
        const size_t end = std::min(p.find('/', pos), p.size());
        const std::string seg = p.substr(pos, end - pos);
        if (seg == ".." || seg == "." || seg.empty())
            return false;
        pos = end + 1;
    }
    return true;
}

std::vector<std::string> strings(const YAML::Node &node) {
    std::vector<std::string> out;
    if (node && node.IsSequence())
        for (const auto &n : node)
            out.push_back(n.as<std::string>());
    return out;
}

std::optional<Mod> parse_manifest(const std::string &text, const std::string &where) {
    try {
        const YAML::Node m = YAML::Load(text);
        if (!m.IsMap() || !m["ss-delta-mod"] || m["ss-delta-mod"].as<int>() != 1) {
            LOG_ERROR("{}: not a packaged mod of format 1 (\"ss-delta-mod\": 1)", where);
            return std::nullopt;
        }
        Mod mod;
        mod.id = m["id"] ? m["id"].as<std::string>() : "";
        mod.name = m["name"] ? m["name"].as<std::string>() : mod.id;
        mod.version = m["version"] ? m["version"].as<std::string>() : "";
        mod.archive = m["archive"] ? m["archive"].as<std::string>() : "";
        mod.remove_note = m["remove_note"] ? m["remove_note"].as<std::string>() : "";
        mod.titles = strings(m["titles"]);
        mod.tables_from = strings(m["tables_from"]);
        static const std::regex id("[A-Za-z0-9_-]{1,64}");
        if (!std::regex_match(mod.id, id) || !safe_relative(mod.archive)) {
            LOG_ERROR("{}: needs an \"id\" (letters, digits, - and _) and the \"archive\" it changes", where);
            return std::nullopt;
        }
        for (const auto &t : mod.tables_from)
            if (!safe_relative(t))
                return std::nullopt;
        if (m["append"])
            for (const auto &a : m["append"]) {
                Append append;
                append.member = a["member"].as<std::string>();
                append.lines = { a["lines"].as<std::string>() }; // the file's name, read by load_mod
                append.conflicts = strings(a["conflicts"]);
                if (!safe_relative(append.lines[0])) {
                    LOG_ERROR("{}: bad \"lines\" file {}", where, append.lines[0]);
                    return std::nullopt;
                }
                mod.appends.push_back(append);
            }
        return mod;
    } catch (const YAML::Exception &e) {
        LOG_ERROR("{}: {}", where, e.what());
        return std::nullopt;
    }
}

std::string generic(const fs::path &p) {
    std::string s = fs_utils::path_to_utf8(p);
    string_utils::replace(s, "\\", "/");
    return s;
}

// a package unpacked in dir: the manifest, the lines files read, files/ listed
std::optional<Mod> load_mod(const fs::path &dir) {
    const auto text = read_file(dir / MANIFEST);
    if (!text)
        return std::nullopt;
    auto mod = parse_manifest(std::string(text->begin(), text->end()), generic(dir / MANIFEST));
    if (!mod)
        return std::nullopt;
    mod->dir = dir;
    for (auto &a : mod->appends) {
        const auto data = read_file(dir / fs_utils::utf8_to_path(a.lines[0]));
        if (!data) {
            LOG_ERROR("{}: {} is missing", mod->id, a.lines[0]);
            return std::nullopt;
        }
        a.lines.clear();
        std::string all(data->begin(), data->end()), l;
        size_t pos = 0;
        while (pos < all.size()) {
            size_t end = all.find('\n', pos);
            if (end == std::string::npos)
                end = all.size();
            l = line_of(all.substr(pos, end - pos));
            if (!l.empty())
                a.lines.push_back(l);
            pos = end + 1;
        }
    }
    const fs::path files = dir / "files";
    boost::system::error_code ec;
    for (fs::recursive_directory_iterator it(files, ec), end; !ec && it != end; it.increment(ec))
        if (fs::is_regular_file(it->path()))
            mod->files[generic(it->path().lexically_relative(files))] = it->path();
    return mod;
}

fs::path mods_dir(const fs::path &vita_fs, const std::string &title) {
    return vita_fs / "ux0" / "rePatch" / title / PACKAGED_MODS_DIR;
}

std::vector<Mod> installed_mods(const fs::path &vita_fs, const std::string &title) {
    std::vector<Mod> mods;
    boost::system::error_code ec;
    for (fs::directory_iterator it(mods_dir(vita_fs, title), ec), end; !ec && it != end; it.increment(ec)) {
        const std::string name = fs_utils::path_to_utf8(it->path().filename());
        if (fs::is_directory(it->path()) && !name.starts_with("."))
            if (auto mod = load_mod(it->path()))
                mods.push_back(std::move(*mod));
    }
    return mods;
}

void remove_empty_dirs(fs::path dir, const fs::path &top) {
    boost::system::error_code ec;
    while (dir.string().size() > top.string().size() && fs::is_directory(dir) && fs::is_empty(dir, ec)) {
        fs::remove(dir, ec);
        dir = dir.parent_path();
    }
}

// The archive `archive` of title, made from the one the game reads now (rePatch's or its own):
// the contributions of `strip` and `keep` taken out, then those of `keep` put in. Deleted from
// rePatch when what is left is the game's own.
bool rebuild(const fs::path &vita_fs, const std::string &title, const std::string &archive,
    const std::vector<const Mod *> &keep, const std::vector<const Mod *> &strip) {
    const fs::path app = vita_fs / "ux0" / "app" / title;
    const fs::path own = app / fs_utils::utf8_to_path(archive);
    const fs::path target = vita_fs / "ux0" / "rePatch" / title / fs_utils::utf8_to_path(archive);
    const fs::path base = has_archive(target) ? target : own;
    const auto index = read_index(base);
    const auto own_index = read_index(own);
    if (!index || !own_index) {
        LOG_ERROR("Mods: {}.pkh is not an archive this game reads", base);
        return false;
    }
    std::set<uint32_t> drop;
    std::map<uint32_t, Bytes> put;
    std::vector<const Mod *> all = keep;
    all.insert(all.end(), strip.begin(), strip.end());

    // files: a mod's earlier copy taken out (the game's own put back), then the kept mods' added
    for (const Mod *mod : all)
        for (const auto &[path, _] : mod->files) {
            const uint32_t h = path_hash(path);
            if (!index->contains(h))
                continue;
            if (own_index->contains(h) && base != own) {
                if (const auto data = read_member(own, *own_index, path))
                    put[h] = *data;
            } else if (!own_index->contains(h))
                drop.insert(h);
        }

    // tables: every mod's lines taken out, the kept mods' appended
    std::set<std::string> members;
    for (const Mod *mod : all)
        for (const auto &a : mod->appends)
            members.insert(a.member);
    for (const auto &member : members) {
        const uint32_t h = path_hash(member);
        std::optional<Bytes> data = read_member(base, *index, member);
        const bool in_base = data.has_value();
        for (size_t i = 0; !data && i < keep.size(); i++)
            for (const auto &from : keep[i]->tables_from) {
                const fs::path other = app / fs_utils::utf8_to_path(from);
                if (const auto other_index = read_index(other); other_index && (data = read_member(other, *other_index, member)))
                    break;
            }
        if (!data) {
            if (!keep.empty())
                LOG_ERROR("Mods: {} is in none of the archives the mods name", member);
            if (keep.empty())
                continue;
            return false;
        }
        std::set<std::string> ours;
        for (const Mod *mod : all)
            for (const auto &a : mod->appends)
                if (a.member == member)
                    ours.insert(a.lines.begin(), a.lines.end());
        std::string text = strip_lines(std::string(data->begin(), data->end()), ours);
        bool appended = false;
        for (const Mod *mod : keep)
            for (const auto &a : mod->appends) {
                if (a.member != member)
                    continue;
                for (const auto &c : a.conflicts)
                    if (has_line_starting(text, c)) {
                        LOG_ERROR("Mods: {} can't add its rows to {}: rows starting \"{}\" are there already (another mod, or an older version of this one made otherwise: remove it first)",
                            mod->name, member, c);
                        return false;
                    }
                text = append_lines(text, a.lines);
                appended = true;
            }
        // a table the archive got from tables_from (the game's own lacks it), with no rows of a
        // kept mod left: dropped again when it is the source's (the game then reads that)
        bool from_source = false;
        if (!appended && in_base && !own_index->contains(h))
            for (const Mod *mod : all) {
                for (const auto &from : mod->tables_from) {
                    const fs::path other = app / fs_utils::utf8_to_path(from);
                    const auto other_index = read_index(other);
                    if (const auto source = other_index ? read_member(other, *other_index, member) : std::nullopt) {
                        from_source = std::string(source->begin(), source->end()) == text;
                        break;
                    }
                }
                if (from_source)
                    break;
            }
        if ((appended || in_base) && !from_source)
            put[h] = Bytes(text.begin(), text.end());
        else
            drop.insert(h);
    }
    for (const Mod *mod : keep)
        for (const auto &[path, file] : mod->files) {
            const auto data = read_file(file);
            if (!data) {
                LOG_ERROR("Mods: {} could not be read", file);
                return false;
            }
            put[path_hash(path)] = *data;
            drop.erase(path_hash(path));
        }

    boost::system::error_code ec;
    fs::create_directories(target.parent_path(), ec);
    const fs::path tmp = with_ext(target, ".new");
    if (!write_archive(base, *index, drop, put, tmp)) {
        LOG_ERROR("Mods: {} could not be written", with_ext(tmp, ".pk"));
        fs::remove(with_ext(tmp, ".pk"), ec);
        fs::remove(with_ext(tmp, ".pkh"), ec);
        return false;
    }
    if (keep.empty() && same_contents(tmp, own)) {
        // nothing but the game's own left: rePatch's copy goes
        for (const char *ext : { ".pk", ".pkh" }) {
            fs::remove(with_ext(tmp, ext), ec);
            fs::remove(with_ext(target, ext), ec);
        }
        LOG_INFO("Mods: {} is the game's own again: rePatch's copy deleted", archive);
        remove_empty_dirs(target.parent_path(), vita_fs / "ux0" / "rePatch");
        return true;
    }
    for (const char *ext : { ".pk", ".pkh" }) {
        fs::rename(with_ext(tmp, ext), with_ext(target, ext), ec);
        if (ec) {
            LOG_ERROR("Mods: {} could not replace {}: {}", with_ext(tmp, ext), with_ext(target, ext), ec.message());
            return false;
        }
    }
    LOG_INFO("Mods: {} written ({} file{} added or changed, over {})", generic(target.lexically_relative(vita_fs)),
        put.size(), put.size() == 1 ? "" : "s", base == own ? "the game's own" : "the rePatch mod's");
    return true;
}

std::vector<const Mod *> on_archive(const std::vector<Mod> &mods, const std::string &archive, const std::string &except = {}) {
    std::vector<const Mod *> out;
    for (const auto &m : mods)
        if (m.archive == archive && m.id != except)
            out.push_back(&m);
    return out;
}

bool merged(const fs::path &vita_fs, const std::string &title, const Mod &mod) {
    const fs::path target = vita_fs / "ux0" / "rePatch" / title / fs_utils::utf8_to_path(mod.archive);
    const auto index = has_archive(target) ? read_index(target) : std::nullopt;
    if (!index)
        return false;
    for (const auto &[path, _] : mod.files)
        if (!index->contains(path_hash(path)))
            return false;
    for (const auto &a : mod.appends) {
        const auto data = read_member(target, *index, a.member);
        if (!data || a.lines.empty())
            return a.lines.empty();
        if (!has_line_starting(std::string(data->begin(), data->end()), a.lines.front()))
            return false;
    }
    return true;
}

bool installed_game(const fs::path &vita_fs, const std::string &title) {
    return fs::exists(vita_fs / "ux0" / "app" / title / "eboot.bin");
}

// the installed game a mod goes to: the first of title_ids installed that the mod names
std::string pick_title(const fs::path &vita_fs, const Mod &mod, const std::vector<std::string> &title_ids) {
    for (const auto &t : title_ids)
        if (installed_game(vita_fs, t) && (mod.titles.empty() || std::find(mod.titles.begin(), mod.titles.end(), t) != mod.titles.end()))
            return t;
    return {};
}

size_t write_to_stream(void *opaque, mz_uint64, const void *buf, size_t n) {
    auto *out = static_cast<fs::ofstream *>(opaque);
    out->write(static_cast<const char *>(buf), static_cast<std::streamsize>(n));
    return *out ? n : 0;
}

// a zip holding ss-delta-mod.json (at the top or in one folder): its members below that, extracted
// into `to` once the manifest says where; nullopt: no package
struct Zip {
    mz_zip_archive zip{};
    FILE *fp = nullptr;
    bool open = false;
    ~Zip() {
        if (open)
            mz_zip_reader_end(&zip);
        if (fp)
            fclose(fp);
    }
};

} // namespace

std::optional<bool> install_packaged_mod(const fs::path &vita_fs, const fs::path &path, const std::vector<std::string> &title_ids) {
    // the manifest: in the folder (or one below), or the zip's top (or one folder)
    std::string manifest_text, prefix;
    fs::path folder;
    Zip z;
    if (fs::is_directory(path)) {
        if (fs::is_regular_file(path / MANIFEST))
            folder = path;
        else {
            boost::system::error_code ec;
            for (fs::directory_iterator it(path, ec), end; !ec && it != end; it.increment(ec))
                if (fs::is_regular_file(it->path() / MANIFEST)) {
                    folder = it->path();
                    break;
                }
        }
        if (folder.empty())
            return std::nullopt;
        const auto text = read_file(folder / MANIFEST);
        manifest_text.assign(text->begin(), text->end());
    } else {
        if (string_utils::tolower(fs_utils::path_to_utf8(path.extension())) != ".zip")
            return std::nullopt;
        z.fp = FOPEN(path.c_str(), "rb");
        if (!z.fp || !mz_zip_reader_init_cfile(&z.zip, z.fp, 0, 0))
            return std::nullopt;
        z.open = true;
        const mz_uint count = mz_zip_reader_get_num_files(&z.zip);
        for (mz_uint i = 0; i < count && manifest_text.empty(); i++) {
            mz_zip_archive_file_stat stat;
            if (!mz_zip_reader_file_stat(&z.zip, i, &stat))
                continue;
            std::string name = stat.m_filename;
            string_utils::replace(name, "\\", "/");
            const size_t slash = name.find('/');
            if (name == MANIFEST || (slash != std::string::npos && name.substr(slash + 1) == MANIFEST)) {
                prefix = name.substr(0, name.size() - std::strlen(MANIFEST));
                size_t n = 0;
                void *data = mz_zip_reader_extract_to_heap(&z.zip, i, &n, 0);
                if (data) {
                    manifest_text.assign(static_cast<char *>(data), n);
                    mz_free(data);
                }
            }
        }
        if (manifest_text.empty())
            return std::nullopt;
    }

    auto mod = parse_manifest(manifest_text, fs_utils::path_to_utf8(path));
    if (!mod)
        return false;
    const std::string title = pick_title(vita_fs, *mod, title_ids);
    if (title.empty()) {
        LOG_ERROR("Mods: {} is for {}, which isn't installed here", mod->name, fmt::join(mod->titles, " or "));
        return false;
    }

    // unpacked beside the installed packages, then put in place once merged
    const fs::path dir = mods_dir(vita_fs, title);
    const fs::path incoming = dir / ("." + mod->id + ".new");
    boost::system::error_code ec;
    fs::remove_all(incoming, ec);
    fs::create_directories(incoming, ec);
    if (folder.empty()) {
        const mz_uint count = mz_zip_reader_get_num_files(&z.zip);
        for (mz_uint i = 0; i < count; i++) {
            mz_zip_archive_file_stat stat;
            if (!mz_zip_reader_file_stat(&z.zip, i, &stat) || mz_zip_reader_is_file_a_directory(&z.zip, i))
                continue;
            std::string name = stat.m_filename;
            string_utils::replace(name, "\\", "/");
            if (!name.starts_with(prefix))
                continue;
            const std::string rel = name.substr(prefix.size());
            if (!safe_relative(rel)) {
                LOG_WARN("Mods: {} in the zip left out (not a plain path)", name);
                continue;
            }
            const fs::path dst = incoming / fs_utils::utf8_to_path(rel);
            fs::create_directories(dst.parent_path(), ec);
            fs::ofstream out(dst, std::ios::binary | std::ios::trunc);
            if (!out || !mz_zip_reader_extract_to_callback(&z.zip, i, write_to_stream, &out, 0) || !out) {
                LOG_ERROR("Mods: {} could not be extracted", rel);
                fs::remove_all(incoming, ec);
                return false;
            }
        }
    } else {
        for (fs::recursive_directory_iterator it(folder, ec), end; !ec && it != end; it.increment(ec)) {
            if (!fs::is_regular_file(it->path()))
                continue;
            const fs::path dst = incoming / it->path().lexically_relative(folder);
            fs::create_directories(dst.parent_path(), ec);
            fs::copy_file(it->path(), dst, fs::copy_options::overwrite_existing, ec);
            if (ec) {
                LOG_ERROR("Mods: {} could not be copied: {}", it->path(), ec.message());
                fs::remove_all(incoming, ec);
                return false;
            }
        }
    }
    auto fresh = load_mod(incoming);
    if (!fresh) {
        fs::remove_all(incoming, ec);
        return false;
    }

    // merged with the others on that archive, an earlier version of it taken out
    const auto mods = installed_mods(vita_fs, title);
    const Mod *old = nullptr;
    for (const auto &m : mods)
        if (m.id == fresh->id)
            old = &m;
    auto keep = on_archive(mods, fresh->archive, fresh->id);
    keep.push_back(&*fresh);
    std::vector<const Mod *> strip;
    if (old)
        strip.push_back(old);
    LOG_INFO("Mods: installing {} {}{} for {}", fresh->name, fresh->version, old ? fmt::format(" (in place of {})", old->version) : "", title);
    if (!rebuild(vita_fs, title, fresh->archive, keep, strip)) {
        fs::remove_all(incoming, ec);
        return false;
    }
    const fs::path final_dir = dir / fresh->id;
    fs::remove_all(final_dir, ec);
    fs::rename(incoming, final_dir, ec);
    if (ec) {
        LOG_ERROR("Mods: {} could not be moved to {}: {}", incoming, final_dir, ec.message());
        return false;
    }
    LOG_INFO("Mods: {} installed ({} file{}, {} table{}; --remove-mod {} takes it out)", fresh->name, fresh->files.size(),
        fresh->files.size() == 1 ? "" : "s", fresh->appends.size(), fresh->appends.size() == 1 ? "" : "s", fresh->id);
    return true;
}

bool remove_packaged_mod(const fs::path &vita_fs, const std::string &id, const std::vector<std::string> &title_ids) {
    std::vector<std::string> found;
    for (const auto &title : title_ids) {
        const auto mods = installed_mods(vita_fs, title);
        for (const auto &m : mods)
            found.push_back(m.id);
        const auto it = std::find_if(mods.begin(), mods.end(), [&](const Mod &m) { return m.id == id; });
        if (it == mods.end())
            continue;
        if (!rebuild(vita_fs, title, it->archive, on_archive(mods, it->archive, id), { &*it }))
            return false;
        boost::system::error_code ec;
        fs::remove_all(it->dir, ec);
        remove_empty_dirs(mods_dir(vita_fs, title), vita_fs / "ux0" / "rePatch");
        LOG_INFO("Mods: {} removed from {}", it->name, title);
        if (!it->remove_note.empty())
            LOG_WARN("{}", it->remove_note);
        return true;
    }
    LOG_ERROR("Mods: no installed mod {} (installed: {})", id, found.empty() ? "none" : fmt::format("{}", fmt::join(found, ", ")));
    return false;
}

void refresh_packaged_mods(const fs::path &vita_fs, const std::string &title_id) {
    const auto mods = installed_mods(vita_fs, title_id);
    std::set<std::string> archives;
    for (const auto &m : mods)
        archives.insert(m.archive);
    for (const auto &archive : archives) {
        const auto keep = on_archive(mods, archive);
        const bool all_there = std::all_of(keep.begin(), keep.end(), [&](const Mod *m) { return merged(vita_fs, title_id, *m); });
        if (all_there)
            continue;
        LOG_INFO("Mods: {} lacks the packaged mods (another mod replaced it, or it was deleted): merging them again", archive);
        rebuild(vita_fs, title_id, archive, keep, {});
    }
}

} // namespace game_frontend
