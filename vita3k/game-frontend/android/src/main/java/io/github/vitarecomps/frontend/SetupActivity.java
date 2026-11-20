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
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLES20;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.security.MessageDigest;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Starts the game, after offering a Turnip Vulkan driver on the Adreno GPUs it was tested for
 * (ss-delta 09-android, D25). Qualcomm's own driver lacks coherent framebuffer reads, and games that
 * blend in their shaders (Soul Sacrifice Delta's fog) come out with black rectangles; Mesa's Turnip has
 * them. The offer: a pinned release, checked against its SHA-256, saved as files/drivers/<name>.zip,
 * which the game unpacks into its own storage and uses ([debug] gpu_driver = auto). Asked once per
 * install unless the player chooses "Not now".
 */
public class SetupActivity extends Activity {
    private static final String TAG = "SetupActivity";
    private static final String PREFS = "setup";
    private static final String KEY_DRIVER_ANSWERED = "driver_offer_answered";

    // Eden's recommendation for Adreno 711-799, tested on the Adreno 740 (AYN Thor)
    private static final String DRIVER_NAME = "Turnip T23 (Mr. Purple, Mesa 26.0.0)";
    private static final String DRIVER_FILE = "turnip_mrpurple_T23-toasted.adpkg.zip";
    private static final String DRIVER_URL =
        "https://github.com/MrPurple666/purple-turnip/releases/download/vturnip_mrpurple_T23-toasted.adpkg/turnip_mrpurple_T23-toasted.adpkg.zip";
    private static final String DRIVER_SHA256 = "5d290bc1421af9fbe30ca35afa9e28bb27f03daf3648203afe83a34e56620b82";
    private static final int DRIVER_SIZE = 3558070;

    private final Handler main = new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!shouldOffer()) {
            startGame();
            return;
        }
        new AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
            .setTitle("GPU driver")
            .setMessage("This device's own Vulkan driver (Qualcomm's) draws some of this game's effects wrong: "
                + "black rectangles in fog and smoke.\n\n"
                + "Turnip, the open-source Mesa driver for Adreno GPUs, draws them correctly. Download it now? "
                + "(" + DRIVER_NAME + ", " + (DRIVER_SIZE / 100000) / 10.0 + " MB from github.com/MrPurple666/purple-turnip, "
                + "MIT licence.) It is used by this game only.")
            .setCancelable(false)
            .setPositiveButton("Download", (d, w) -> download())
            .setNeutralButton("Not now", (d, w) -> startGame())
            .setNegativeButton("Don't ask again", (d, w) -> {
                answered();
                startGame();
            })
            .show();
    }

    private boolean shouldOffer() {
        if (getSharedPreferences(PREFS, MODE_PRIVATE).getBoolean(KEY_DRIVER_ANSWERED, false))
            return false;
        File drivers = driversDir();
        String[] present = drivers.list();
        if (present != null) {
            for (String name : present)
                if (name.endsWith(".zip"))
                    return false; // a package is there already (the PC setup, or an earlier download)
        }
        int model = adrenoModel();
        Log.i(TAG, "Adreno model " + model);
        return model >= 711 && model <= 799;
    }

    private File driversDir() {
        File base = getExternalFilesDir(null);
        return new File(base != null ? base : getFilesDir(), "drivers");
    }

    private void answered() {
        getSharedPreferences(PREFS, MODE_PRIVATE).edit().putBoolean(KEY_DRIVER_ANSWERED, true).apply();
    }

    // The GPU's name from an off-screen OpenGL ES context ("Adreno (TM) 740"): its model number, or 0
    private static int adrenoModel() {
        EGLDisplay display = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (display == EGL14.EGL_NO_DISPLAY)
            return 0;
        int[] version = new int[2];
        if (!EGL14.eglInitialize(display, version, 0, version, 1))
            return 0;
        String renderer = null;
        try {
            int[] attributes = { EGL14.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT, EGL14.EGL_SURFACE_TYPE, EGL14.EGL_PBUFFER_BIT, EGL14.EGL_NONE };
            EGLConfig[] configs = new EGLConfig[1];
            int[] count = new int[1];
            if (!EGL14.eglChooseConfig(display, attributes, 0, configs, 0, 1, count, 0) || count[0] == 0)
                return 0;
            EGLContext context = EGL14.eglCreateContext(display, configs[0], EGL14.EGL_NO_CONTEXT,
                new int[] { EGL14.EGL_CONTEXT_CLIENT_VERSION, 2, EGL14.EGL_NONE }, 0);
            EGLSurface surface = EGL14.eglCreatePbufferSurface(display, configs[0],
                new int[] { EGL14.EGL_WIDTH, 1, EGL14.EGL_HEIGHT, 1, EGL14.EGL_NONE }, 0);
            if (EGL14.eglMakeCurrent(display, surface, surface, context))
                renderer = GLES20.glGetString(GLES20.GL_RENDERER);
            EGL14.eglMakeCurrent(display, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
            EGL14.eglDestroySurface(display, surface);
            EGL14.eglDestroyContext(display, context);
        } finally {
            EGL14.eglTerminate(display);
        }
        if (renderer == null)
            return 0;
        Matcher m = Pattern.compile("Adreno[^0-9]*([0-9]{3})").matcher(renderer);
        return m.find() ? Integer.parseInt(m.group(1)) : 0;
    }

    private void download() {
        AlertDialog progress = new AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
            .setTitle("GPU driver")
            .setMessage("Downloading " + DRIVER_NAME + "…")
            .setCancelable(false)
            .show();
        new Thread(() -> {
            String error = fetch();
            main.post(() -> {
                progress.dismiss();
                if (error == null) {
                    answered();
                    startGame();
                    return;
                }
                new AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
                    .setTitle("GPU driver")
                    .setMessage("The download failed: " + error + "\n\nThe game starts with the device's own driver; "
                        + "the offer comes back at the next start.")
                    .setCancelable(false)
                    .setPositiveButton("OK", (d, w) -> startGame())
                    .show();
            });
        }).start();
    }

    // The pinned package into drivers/, checked; null, or what went wrong
    private String fetch() {
        File dir = driversDir();
        File part = new File(dir, DRIVER_FILE + ".part");
        try {
            if (!dir.isDirectory() && !dir.mkdirs())
                return "no folder " + dir;
            HttpURLConnection connection = (HttpURLConnection) new URL(DRIVER_URL).openConnection();
            connection.setConnectTimeout(20000);
            connection.setReadTimeout(30000);
            connection.setInstanceFollowRedirects(true);
            MessageDigest sha = MessageDigest.getInstance("SHA-256");
            long total = 0;
            try (InputStream in = connection.getInputStream(); FileOutputStream out = new FileOutputStream(part)) {
                byte[] buffer = new byte[65536];
                for (int n; (n = in.read(buffer)) > 0;) {
                    out.write(buffer, 0, n);
                    sha.update(buffer, 0, n);
                    total += n;
                    if (total > DRIVER_SIZE)
                        break;
                }
            } finally {
                connection.disconnect();
            }
            StringBuilder hex = new StringBuilder();
            for (byte b : sha.digest())
                hex.append(String.format("%02x", b));
            if (total != DRIVER_SIZE || !hex.toString().equals(DRIVER_SHA256)) {
                part.delete();
                return "the file is not the expected one (" + total + " bytes)";
            }
            File done = new File(dir, DRIVER_FILE);
            if (!part.renameTo(done))
                return "could not save " + done;
            return null;
        } catch (Exception e) {
            Log.e(TAG, "driver download", e);
            part.delete();
            return e.getMessage() != null ? e.getMessage() : e.toString();
        }
    }

    private void startGame() {
        Intent intent = new Intent(this, GameActivity.class);
        Intent own = getIntent();
        if (own != null && own.getExtras() != null)
            intent.putExtras(own.getExtras());
        startActivity(intent);
        finish();
    }
}
