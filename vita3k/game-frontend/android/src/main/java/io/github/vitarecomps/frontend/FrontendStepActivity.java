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

import android.app.Activity;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import org.libsdl.app.SDLActivity;

/**
 * One of the front end's own setup steps, run as the desktop setup runs the executable (ss-delta D27):
 * --install-firmware PUP, --install-content dump [--license ...], --dump-elfs seconds. Started for a
 * result with the "args" string array, as GameActivity takes it; the result code is the front end's
 * exit code (RESULT_OK for 0, else RESULT_FIRST_USER + code), and the "exit_code" extra holds it.
 *
 * It runs in a process of its own (the manifest's :frontend_step), which ends with the step: the
 * front end keeps state in static objects and is never run twice in one process.
 */
public class FrontendStepActivity extends GameActivity {
    private static final String TAG = "FrontendStep";
    public static final String EXTRA_EXIT_CODE = "exit_code";

    @Override
    protected void main() {
        String library = getMainSharedObject();
        String function = getMainFunction();
        String[] arguments = getArguments();
        Log.i(TAG, "step: " + String.join(" ", arguments));
        int code = SDLActivity.nativeRunMain(library, function, arguments);
        Log.i(TAG, "step ended with " + code);
        // set before SDL's thread calls finish(), which hands the result to the caller
        setResult(code == 0 ? Activity.RESULT_OK : Activity.RESULT_FIRST_USER + code,
            new Intent().putExtra(EXTRA_EXIT_CODE, code));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (isFinishing()) {
            // the result is with the system once finish() returned; a fresh process for the next step
            new Handler(Looper.getMainLooper()).postDelayed(() -> android.os.Process.killProcess(android.os.Process.myPid()), 300);
        }
    }
}
