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

// macOS's CoreFoundation, apart from Vita3K's headers (MacTypes.h declares a Ptr of its own)

#include <string>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <limits.h>
#endif

namespace game_frontend {

// the app bundle's original path when Gatekeeper runs it from a translocated copy, otherwise ""
std::string translocated_app_original_path() {
#ifdef __APPLE__
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = bundle ? CFBundleCopyBundleURL(bundle) : nullptr;
    if (!url)
        return {};
    // Security.framework's translocation calls, by name (they aren't in its public headers)
    using IsTranslocated = Boolean (*)(CFURLRef, bool *, CFErrorRef *);
    using OriginalPath = CFURLRef (*)(CFURLRef, CFErrorRef *);
    void *security = dlopen("/System/Library/Frameworks/Security.framework/Security", RTLD_LAZY);
    const auto is_translocated = security ? reinterpret_cast<IsTranslocated>(dlsym(security, "SecTranslocateIsTranslocatedURL")) : nullptr;
    const auto original_path = security ? reinterpret_cast<OriginalPath>(dlsym(security, "SecTranslocateCreateOriginalPathForURL")) : nullptr;
    bool translocated = false;
    if (!is_translocated || !original_path || !is_translocated(url, &translocated, nullptr) || !translocated) {
        CFRelease(url);
        return {};
    }
    CFURLRef original = original_path(url, nullptr);
    CFRelease(url);
    if (!original)
        return {};
    char path[PATH_MAX];
    const bool ok = CFURLGetFileSystemRepresentation(original, true, reinterpret_cast<UInt8 *>(path), sizeof(path));
    CFRelease(original);
    return ok ? std::string(path) : std::string();
#else
    return {};
#endif
}

} // namespace game_frontend
