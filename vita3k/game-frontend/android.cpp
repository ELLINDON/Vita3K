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

#include <SDL3/SDL_keyboard.h>

#include <jni.h>

namespace {
SDL_Window *s_window = nullptr;
} // namespace

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
