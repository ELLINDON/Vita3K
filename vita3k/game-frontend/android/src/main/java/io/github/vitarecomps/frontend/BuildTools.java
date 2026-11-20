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
import android.system.ErrnoException;
import android.system.Os;
import android.system.OsConstants;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * The tools the APK carries to build the game's native modules on the device (ss-delta D27, made by
 * vita-recomp-kit's android-build-tools.py): python.org's Python with a launcher, capstone, zig, and the
 * kit's scripts. The programs are in the app's native library folder, the only place Android lets an app
 * run programs from; the rest (Python's standard library, zig's lib/, the kit) is unpacked from
 * assets/buildtools.zip into files/buildtools/ on first use, and again when assets/buildtools.json
 * changes. APKs built without the tools have neither asset.
 */
final class BuildTools {
    private static final String STAMP = "buildtools.json";
    private static final String ZIP = "buildtools.zip";

    private BuildTools() {}

    static boolean present(Context context) {
        try {
            return Arrays.asList(context.getAssets().list("")).contains(STAMP);
        } catch (IOException e) {
            return false;
        }
    }

    static File root(Context context) {
        return new File(context.getFilesDir(), "buildtools");
    }

    /** The kit's toolchain folder (toolchain.py's toolchain_dir): zig/zig, zig's cache. */
    static File toolchain(Context context) {
        return new File(root(context), "toolchain");
    }

    static File kit(Context context) {
        return new File(root(context), "kit");
    }

    private static String nativeLibraryDir(Context context) {
        return context.getApplicationInfo().nativeLibraryDir;
    }

    /** Unpacks the tools unless this APK's are there already; reports what it does to log. */
    static void unpack(Context context, Consumer<String> log) throws IOException {
        String stamp = new String(readAll(context.getAssets().open(STAMP)), StandardCharsets.UTF_8);
        File root = root(context);
        File done = new File(root, STAMP);
        if (done.isFile() && new String(readAll(new java.io.FileInputStream(done)), StandardCharsets.UTF_8).equals(stamp)) {
            linkZig(context);
            return;
        }
        log.accept("unpacking the build tools");
        deleteTree(root);
        if (!root.mkdirs())
            throw new IOException("could not make " + root);
        String base = root.getCanonicalPath() + File.separator;
        int files = 0;
        byte[] buffer = new byte[1 << 16];
        try (ZipInputStream zip = new ZipInputStream(new BufferedInputStream(context.getAssets().open(ZIP), 1 << 16))) {
            for (ZipEntry entry; (entry = zip.getNextEntry()) != null;) {
                File target = new File(root, entry.getName());
                if (!target.getCanonicalPath().startsWith(base))
                    throw new IOException("bad entry " + entry.getName());
                if (entry.isDirectory()) {
                    target.mkdirs();
                    continue;
                }
                File parent = target.getParentFile();
                if (parent != null && !parent.isDirectory() && !parent.mkdirs())
                    throw new IOException("could not make " + parent);
                try (FileOutputStream out = new FileOutputStream(target)) {
                    for (int n; (n = zip.read(buffer)) > 0;)
                        out.write(buffer, 0, n);
                }
                files++;
            }
        }
        linkZig(context);
        try (FileOutputStream out = new FileOutputStream(done)) {
            out.write(stamp.getBytes(StandardCharsets.UTF_8));
        }
        log.accept("unpacked " + files + " files");
    }

    // toolchain.py runs <toolchain>/zig/zig: a link to the program in the native library folder, made
    // again each time, because that folder moves whenever the APK is installed or updated
    // (/data/app/~~<random>/...). zig finds its lib/ through ZIG_LIB_DIR (see environment), as it
    // resolves its own path past the link
    private static void linkZig(Context context) throws IOException {
        File zigDir = new File(toolchain(context), "zig");
        if (!zigDir.isDirectory() && !zigDir.mkdirs())
            throw new IOException("could not make " + zigDir);
        File link = new File(zigDir, "zig");
        String target = nativeLibraryDir(context) + "/libzig.so";
        try {
            if (isLink(link) && target.equals(Os.readlink(link.getPath())))
                return;
            link.delete();
            Os.symlink(target, link.getPath());
        } catch (ErrnoException e) {
            throw new IOException("could not link zig: " + e.getMessage());
        }
    }

    /** A process running the tools' Python with these arguments (as the python command takes them). */
    static ProcessBuilder python(Context context, List<String> args) {
        String lib = nativeLibraryDir(context);
        List<String> command = new ArrayList<>();
        command.add(lib + "/libpython-launcher.so");
        command.addAll(args);
        ProcessBuilder builder = new ProcessBuilder(command);
        environment(context, builder.environment());
        builder.redirectErrorStream(true);
        return builder;
    }

    static void environment(Context context, Map<String, String> env) {
        File root = root(context);
        String lib = nativeLibraryDir(context);
        env.put("PYTHONHOME", new File(root, "python").getPath());
        env.put("PYTHONPATH", new File(root, "site").getPath());
        env.put("PYTHONUNBUFFERED", "1");
        // the launcher and the extension modules find libpython and its OpenSSL there; capstone's binding
        // loads its library from LIBCAPSTONE_PATH
        env.put("LD_LIBRARY_PATH", lib);
        env.put("LIBCAPSTONE_PATH", lib);
        env.put("ZIG_LIB_DIR", new File(root, "zig/lib").getPath());
        env.put("HOME", root.getPath());
        env.put("TMPDIR", context.getCacheDir().getPath());
    }

    /** Runs a process to its end, passing each line of its output to log; its exit code. */
    static int run(ProcessBuilder builder, Consumer<String> log) throws IOException, InterruptedException {
        Process process = builder.start();
        try (BufferedReader out = new BufferedReader(new InputStreamReader(process.getInputStream(), StandardCharsets.UTF_8))) {
            for (String line; (line = out.readLine()) != null;)
                log.accept(line);
        }
        return process.waitFor();
    }

    private static byte[] readAll(InputStream in) throws IOException {
        try (InputStream input = in) {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            for (int n; (n = input.read(buffer)) > 0;)
                out.write(buffer, 0, n);
            return out.toByteArray();
        }
    }

    private static void deleteTree(File file) {
        // a link (toolchain/zig/zig) is removed, never followed
        File[] children = file.isDirectory() && !isLink(file) ? file.listFiles() : null;
        if (children != null)
            for (File child : children)
                deleteTree(child);
        file.delete();
    }

    private static boolean isLink(File file) {
        try {
            return OsConstants.S_ISLNK(Os.lstat(file.getPath()).st_mode);
        } catch (ErrnoException e) {
            return false;
        }
    }
}
