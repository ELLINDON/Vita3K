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

#include <util/fs.h>
#include <util/string_utils.h>

#include <SDL3/SDL_iostream.h>

#ifdef __ANDROID__
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {

// A file the player picked through Android's file picker reaches the front end as a link in the app's
// cache to /proc/self/fd/N, N a descriptor the app holds open. Opening that link opens the file again
// with the app's own permissions, which it lacks for a file in shared storage on some devices (a
// Galaxy S25, ss-delta #23); the descriptor itself is readable. Each FILE reads a copy of it with
// pread at its own offset, so two FILEs on one picked file don't move each other.
struct PickedFile {
    int fd;
    off64_t pos;
};

int picked_read(void *cookie, char *buf, int n) {
    auto *f = static_cast<PickedFile *>(cookie);
    const ssize_t got = pread64(f->fd, buf, n, f->pos);
    if (got > 0)
        f->pos += got;
    return static_cast<int>(got);
}

fpos64_t picked_seek(void *cookie, fpos64_t offset, int whence) {
    auto *f = static_cast<PickedFile *>(cookie);
    off64_t base = 0;
    if (whence == SEEK_CUR)
        base = f->pos;
    else if (whence == SEEK_END) {
        struct stat64 st;
        if (fstat64(f->fd, &st) != 0)
            return -1;
        base = st.st_size;
    } else if (whence != SEEK_SET) {
        errno = EINVAL;
        return -1;
    }
    if (base + offset < 0) {
        errno = EINVAL;
        return -1;
    }
    f->pos = base + offset;
    return f->pos;
}

int picked_close(void *cookie) {
    auto *f = static_cast<PickedFile *>(cookie);
    const int result = close(f->fd);
    delete f;
    return result;
}

} // namespace

FILE *android_fopen(const char *filename, const char *mode) {
    static constexpr char prefix[] = "/proc/self/fd/";
    char target[64];
    const ssize_t n = (mode[0] == 'r' && !std::strchr(mode, '+')) ? readlink(filename, target, sizeof(target) - 1) : -1;
    if (n > static_cast<ssize_t>(sizeof(prefix) - 1) && std::strncmp(target, prefix, sizeof(prefix) - 1) == 0) {
        target[n] = 0;
        char *end = nullptr;
        const long fd = std::strtol(target + sizeof(prefix) - 1, &end, 10);
        const int copy = (*end == 0 && fd >= 0) ? fcntl(static_cast<int>(fd), F_DUPFD_CLOEXEC, 0) : -1;
        if (copy >= 0) {
            auto *cookie = new PickedFile{ copy, 0 };
            if (FILE *f = funopen64(cookie, picked_read, nullptr, picked_seek, picked_close))
                return f;
            close(copy);
            delete cookie;
        }
    }
    return fopen(filename, mode);
}
#endif

namespace fs_utils {

fs::path construct_file_name(const fs::path &base_path, const fs::path &folder_path, const fs::path &file_name, const fs::path &extension) {
    fs::path full_file_path{ base_path / folder_path / file_name };
    if (!extension.empty())
        full_file_path.replace_extension(extension);

    return full_file_path.generic_path();
}

std::string path_to_utf8(const fs::path &path) {
    if constexpr (sizeof(fs::path::value_type) == sizeof(wchar_t)) {
        return string_utils::wide_to_utf(path.wstring());
    } else {
        return path.string();
    }
}

fs::path utf8_to_path(const std::string &str) {
    if constexpr (sizeof(fs::path::value_type) == sizeof(wchar_t)) {
        return fs::path{ string_utils::utf_to_wide(str) };
    } else {
        return fs::path{ str };
    }
}

fs::path path_concat(const fs::path &path1, const fs::path &path2) {
    return fs::path{ path1.native() + path2.native() };
}

void dump_data(const fs::path &path, const void *data, const std::streamsize size) {
    fs::ofstream of{ path, fs::ofstream::binary };
    if (!of.fail()) {
        of.write(static_cast<const char *>(data), size);
        of.close();
    }
}
template <typename T>
static bool read_data(const fs::path &path, std::vector<T> &data) {
    data.clear();
    SDL_IOStream *file = SDL_IOFromFile(fs_utils::path_to_utf8(path).c_str(), "rb");
    if (!file) {
        return false;
    }

    // Get the size of the file
    const Sint64 size = SDL_GetIOSize(file);
    if (size <= 0) {
        SDL_CloseIO(file);
        return false;
    }

    // Resize the vector to fit the file content
    data.resize(size);

    // Read the content of the file
    if (SDL_ReadIO(file, data.data(), size) != size) {
        SDL_CloseIO(file);
        data.clear();
        return false;
    }

    SDL_CloseIO(file);
    return true;
}

bool read_data(const fs::path &path, std::vector<uint8_t> &data) { return read_data<uint8_t>(path, data); }
bool read_data(const fs::path &path, std::vector<int8_t> &data) { return read_data<int8_t>(path, data); }
bool read_data(const fs::path &path, std::vector<char> &data) { return read_data<char>(path, data); }

bool copy_directory_contents(const fs::path &src_path, const fs::path &dst_path, const fs::copy_options options) {
    try {
        if (!fs::is_directory(src_path))
            return false;

        fs::create_directories(dst_path);

        for (const auto &src : fs::recursive_directory_iterator(src_path)) {
            const auto relative_path = fs::relative(src.path(), src_path);
            const auto output_path = dst_path / relative_path;

            if (fs::is_directory(src)) {
                fs::create_directories(output_path);
            } else if (fs::is_regular_file(src)) {
                fs::create_directories(output_path.parent_path());
                fs::copy_file(src.path(), output_path, options);
            }
        }

        return true;
    } catch (const std::exception &) {
        return false;
    }
}

} // namespace fs_utils
