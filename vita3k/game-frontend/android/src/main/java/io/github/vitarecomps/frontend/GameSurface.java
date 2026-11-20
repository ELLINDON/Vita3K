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

package io.github.vitarecomps.frontend;

import android.content.Context;
import android.os.Build;
import android.view.Surface;
import android.view.SurfaceHolder;

import org.libsdl.app.SDLSurface;

/**
 * SDL's surface, which also tells the renderer when it has one to draw on (the native side is
 * game-frontend/android.cpp; the renderer skips frames while there is none), and asks the display
 * for 60 Hz: the games run at 30 or 60 fps, which a 120 Hz screen gains nothing from.
 */
public class GameSurface extends SDLSurface {
    public GameSurface(Context context) {
        super(context);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        setSurfaceStatus(true);
        super.surfaceCreated(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        super.surfaceChanged(holder, format, width, height);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Surface surface = holder.getSurface();
            if (surface != null && surface.isValid())
                surface.setFrameRate(60.0f, Surface.FRAME_RATE_COMPATIBILITY_FIXED_SOURCE);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        setSurfaceStatus(false);
        super.surfaceDestroyed(holder);
    }

    public native void setSurfaceStatus(boolean surfacePresent);
}
