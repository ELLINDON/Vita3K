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

#include "importer.h"

#include <util/log.h>
#include <util/string_utils.h>

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char **environ;
#endif

#ifdef __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>
#include <sys/xattr.h>
#endif

namespace game_frontend {

// importer_apple.cpp (CoreFoundation's headers clash with Vita3K's)
std::string translocated_app_original_path();

namespace {

fs::path importer_script(const fs::path &game_dir) {
    return game_dir / "setup" / "kit" / "tools" / "setup.py";
}

fs::path importer_python(const fs::path &game_dir, bool console) {
#ifdef _WIN32
    return game_dir / "setup" / "python" / (console ? "python.exe" : "pythonw.exe");
#else
    (void)console;
    return game_dir / "setup" / "python" / "bin" / "python3";
#endif
}

#ifdef _WIN32
// one argument of a command line as CommandLineToArgvW (and Python) reads it back
std::wstring quote(const std::wstring &arg) {
    if (!arg.empty() && arg.find_first_of(L" \t\n\v\"") == std::wstring::npos)
        return arg;
    std::wstring out = L"\"";
    for (size_t i = 0;; i++) {
        size_t backslashes = 0;
        while (i < arg.size() && arg[i] == L'\\') {
            i++;
            backslashes++;
        }
        if (i == arg.size()) {
            out.append(backslashes * 2, L'\\');
            break;
        }
        if (arg[i] == L'"')
            out.append(backslashes * 2 + 1, L'\\');
        else
            out.append(backslashes, L'\\');
        out.push_back(arg[i]);
    }
    out.push_back(L'"');
    return out;
}
#endif

// starts argv[0] with argv; wait: its exit code, otherwise 0 once started; -1: not started. Windows,
// console: a console program that writes where this one does (its console, or the pipe or file a
// launcher gave it), never in a console window of its own
int spawn(const std::vector<std::string> &argv, bool wait, bool console = false) {
#ifdef _WIN32
    std::wstring line;
    for (const auto &arg : argv)
        line += (line.empty() ? L"" : L" ") + quote(string_utils::utf_to_wide(arg));
    const std::wstring program = string_utils::utf_to_wide(argv[0]);
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    BOOL inherit = FALSE;
    DWORD flags = 0;
    if (console && !GetConsoleWindow()) {
        // no console: its output goes to this process's handles if they lead somewhere, else nowhere
        flags = CREATE_NO_WINDOW;
        const HANDLE handles[] = { GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE) };
        if (handles[1] && handles[1] != INVALID_HANDLE_VALUE && GetFileType(handles[1]) != FILE_TYPE_UNKNOWN) {
            for (const HANDLE h : handles)
                if (h && h != INVALID_HANDLE_VALUE)
                    SetHandleInformation(h, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdInput = handles[0];
            si.hStdOutput = handles[1];
            si.hStdError = handles[2] && handles[2] != INVALID_HANDLE_VALUE ? handles[2] : handles[1];
            inherit = TRUE;
        }
    }
    if (!CreateProcessW(program.c_str(), line.data(), nullptr, nullptr, inherit, flags, nullptr, nullptr, &si, &pi)) {
        LOG_ERROR("Could not start {} (error {})", argv[0], GetLastError());
        return -1;
    }
    DWORD code = 0;
    if (wait) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &code);
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return static_cast<int>(code);
#else
    (void)console;
    std::vector<char *> args;
    for (const auto &arg : argv)
        args.push_back(const_cast<char *>(arg.c_str()));
    args.push_back(nullptr);
    pid_t pid = 0;
    if (const int err = posix_spawn(&pid, argv[0].c_str(), nullptr, nullptr, args.data(), environ)) {
        LOG_ERROR("Could not start {} (error {})", argv[0], err);
        return -1;
    }
    if (!wait)
        return 0;
    int status = 0;
    while (waitpid(pid, &status, 0) < 0) {
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
#endif
}

} // namespace

fs::path own_executable() {
#ifdef _WIN32
    std::wstring buffer(MAX_PATH, L'\0');
    for (;;) {
        const DWORD n = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (n < buffer.size()) {
            buffer.resize(n);
            return fs::path(buffer);
        }
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buffer(size, '\0');
    if (_NSGetExecutablePath(buffer.data(), &size) != 0)
        return {};
    char resolved[PATH_MAX];
    return fs::path(realpath(buffer.c_str(), resolved) ? resolved : buffer.c_str());
#else
    boost::system::error_code ec;
    return fs::read_symlink("/proc/self/exe", ec);
#endif
}

void attach_parent_console() {
#ifdef _WIN32
    static bool attached = false;
    if (attached)
        return;
    attached = true;
    // output already going somewhere (a pipe or file a launcher or script reads): keep it
    const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out && out != INVALID_HANDLE_VALUE && GetFileType(out) != FILE_TYPE_UNKNOWN)
        return;
    if (!AttachConsole(ATTACH_PARENT_PROCESS))
        return;
    FILE *file = nullptr;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONOUT$", "w", stderr);
#endif
}

void clear_quarantine(const fs::path &dir) {
#ifdef __APPLE__
    removexattr(dir.c_str(), "com.apple.quarantine", XATTR_NOFOLLOW);
    boost::system::error_code ec;
    for (fs::recursive_directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
        removexattr(it->path().c_str(), "com.apple.quarantine", XATTR_NOFOLLOW);
#else
    (void)dir;
#endif
}

bool relaunch_from_original_location(int argc, char *argv[]) {
#ifdef __APPLE__
    const std::string original = translocated_app_original_path();
    if (original.empty())
        return false;
    const fs::path app(original);
    LOG_INFO("Started from a translocated copy: clearing the quarantine of {} and starting {} again", app.parent_path().string(), app.string());
    clear_quarantine(app.parent_path());
    std::vector<std::string> open{ "/usr/bin/open", "-n", app.string() };
    if (argc > 1) {
        open.push_back("--args");
        for (int i = 1; i < argc; i++)
            open.push_back(argv[i]);
    }
    return spawn(open, false) == 0;
#else
    (void)argc;
    (void)argv;
    return false;
#endif
}

bool has_importer(const fs::path &game_dir) {
    return fs::exists(importer_script(game_dir)) && fs::exists(importer_python(game_dir, true));
}

int run_importer(const fs::path &game_dir, const fs::path &exe, const std::vector<std::string> &args, bool console) {
    if (!has_importer(game_dir))
        return -1;
    clear_quarantine(game_dir);
    // the importer's front-end runs find this folder (and any older front end the folder named)
#ifdef _WIN32
    _wputenv_s(L"VITA_RECOMPS_GAME_DIR", game_dir.wstring().c_str());
#else
    setenv("VITA_RECOMPS_GAME_DIR", game_dir.string().c_str(), 1);
#endif
    std::vector<std::string> argv{ fs_utils::path_to_utf8(importer_python(game_dir, console)), "-I", "-u",
        fs_utils::path_to_utf8(importer_script(game_dir)) };
    argv.insert(argv.end(), args.begin(), args.end());
    argv.push_back("--exe");
    argv.push_back(fs_utils::path_to_utf8(exe));
    std::string line;
    for (const auto &arg : argv)
        line += (line.empty() ? "" : " ") + arg;
    LOG_INFO("Importer: {}", line);
    return spawn(argv, true, console);
}

} // namespace game_frontend
