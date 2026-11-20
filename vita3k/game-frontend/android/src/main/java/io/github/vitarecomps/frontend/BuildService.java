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

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Runs one command of the game's setup with the build tools' Python (vita-recomp-kit's
 * android-setup.py: the firmware download, the eboot check, the build) as a foreground service, so the
 * build goes on while the player leaves the app, with the CPU kept awake (ss-delta D27). BuildActivity
 * starts it and watches {@link #state}; its output also goes to setup.log in the game's folder.
 */
public class BuildService extends Service {
    private static final String TAG = "BuildService";
    static final String EXTRA_ARGS = "args";
    static final String EXTRA_TITLE = "title";
    private static final String CHANNEL = "setup";
    private static final int NOTIFICATION_RUNNING = 1, NOTIFICATION_DONE = 2;

    /** What the screen shows: shared with BuildActivity (one process), guarded by itself. */
    static final class State {
        final List<String> lines = new ArrayList<>();
        String step = "";
        String error = null;
        boolean running = false;
        int exitCode = 0;
        long generation = 0; // counts commands, so the screen knows a result is for the one it started
    }
    static final State state = new State();

    private Thread worker;
    private String stepBase = "";
    private PowerManager.WakeLock wakeLock;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    static File logFile(Context context) {
        File base = context.getExternalFilesDir(null);
        return new File(base != null ? base : context.getFilesDir(), "setup.log");
    }

    static void append(Context context, String line) {
        synchronized (state) {
            state.lines.add(line);
            if (state.lines.size() > 400)
                state.lines.subList(0, state.lines.size() - 400).clear();
        }
        try (FileWriter out = new FileWriter(logFile(context), true)) {
            out.write(line + "\n");
        } catch (IOException e) {
            Log.w(TAG, "setup.log", e);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String[] args = intent != null ? intent.getStringArrayExtra(EXTRA_ARGS) : null;
        String title = intent != null ? intent.getStringExtra(EXTRA_TITLE) : null;
        if (args == null || worker != null) {
            if (worker == null)
                stopSelf();
            return START_NOT_STICKY;
        }
        NotificationManager notifications = getSystemService(NotificationManager.class);
        notifications.createNotificationChannel(new NotificationChannel(CHANNEL, "Game setup", NotificationManager.IMPORTANCE_LOW));
        notifications.cancel(NOTIFICATION_DONE);
        Notification running = notification(title != null ? title : "Setting up", "", true);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
            startForeground(NOTIFICATION_RUNNING, running, ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC);
        else
            startForeground(NOTIFICATION_RUNNING, running);
        wakeLock = getSystemService(PowerManager.class).newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, getPackageName() + ":setup");
        wakeLock.acquire(3 * 60 * 60 * 1000L);

        synchronized (state) {
            state.running = true;
            state.error = null;
            state.step = title != null ? title : "";
            state.generation++;
        }
        worker = new Thread(() -> {
            int code;
            try {
                BuildTools.unpack(this, line -> append(this, line));
                List<String> command = new ArrayList<>();
                command.add(new File(BuildTools.kit(this), "tools/android-setup.py").getPath());
                command.addAll(Arrays.asList(args));
                append(this, "$ android-setup.py " + String.join(" ", args));
                code = BuildTools.run(BuildTools.python(this, command), line -> {
                    append(this, line);
                    if (line.startsWith("@ step ")) {
                        synchronized (state) {
                            state.step = line.substring(7);
                            stepBase = state.step;
                        }
                        notifications.notify(NOTIFICATION_RUNNING, notification(line.substring(7), "", true));
                    } else if (line.trim().matches("\\d+/\\d+")) {
                        // the compiler's progress (toolchain.py: files done / files)
                        String[] n = line.trim().split("/");
                        int done = Integer.parseInt(n[0]), total = Integer.parseInt(n[1]);
                        synchronized (state) {
                            state.step = stepBase + ": " + done + " of " + total + " files";
                        }
                        Notification progress = new Notification.Builder(this, CHANNEL)
                            .setSmallIcon(android.R.drawable.stat_sys_download).setContentTitle(stepBase)
                            .setProgress(total, done, false).setOngoing(true).build();
                        notifications.notify(NOTIFICATION_RUNNING, progress);
                    } else if (line.startsWith("@ error ")) {
                        synchronized (state) {
                            state.error = line.substring(8);
                        }
                    }
                });
            } catch (Exception e) {
                Log.e(TAG, "setup command", e);
                append(this, "failed: " + e);
                synchronized (state) {
                    if (state.error == null)
                        state.error = e.getMessage() != null ? e.getMessage() : e.toString();
                }
                code = -1;
            }
            final int exit = code;
            synchronized (state) {
                state.exitCode = exit;
                if (exit != 0 && state.error == null)
                    state.error = "the setup stopped (exit " + exit + "): see setup.log";
                state.running = false;
            }
            // a note for a player who left the app; the screen takes over when it is open
            if (!BuildActivity.visible) {
                String text = exit == 0 ? "Done: open the game to go on" : "Stopped: open the game to see why";
                notifications.notify(NOTIFICATION_DONE, notification(getString(getApplicationInfo().labelRes), text, false));
            }
            if (wakeLock.isHeld())
                wakeLock.release();
            stopForeground(STOP_FOREGROUND_REMOVE);
            stopSelf();
        }, "setup");
        worker.start();
        return START_NOT_STICKY;
    }

    private Notification notification(String title, String text, boolean ongoing) {
        Intent open = new Intent(this, BuildActivity.class).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        PendingIntent pending = PendingIntent.getActivity(this, 0, open, PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT);
        return new Notification.Builder(this, CHANNEL)
            .setSmallIcon(android.R.drawable.stat_sys_download)
            .setContentTitle(title)
            .setContentText(text)
            .setOngoing(ongoing)
            .setAutoCancel(!ongoing)
            .setContentIntent(pending)
            .build();
    }

    @Override
    public void onDestroy() {
        if (wakeLock != null && wakeLock.isHeld())
            wakeLock.release();
        super.onDestroy();
    }
}
