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
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

/**
 * "Import a Vita save" on the launcher's long-press menu: the desktop's --import-save on a phone, where
 * Android keeps other apps (file managers too) out of the game's folder. The player picks a decrypted
 * save (VitaShell, Apollo) as a folder or a .zip; the game's own --import-save runs on it as one of the
 * front end's steps (FrontendStepActivity), which keeps the saves it replaces in <id>/save-backups.
 * A folder is copied into the cache first (a save is a few hundred kB), since a step takes files.
 */
public class SaveImportActivity extends Activity {
    private static final String TAG = "SaveImport";
    private static final int REQUEST_FOLDER = 1, REQUEST_ZIP = 2, REQUEST_STEP = 3;

    private File copied; // a picked folder's copy in the cache

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (savedInstanceState != null)
            return;
        new AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
            .setTitle("Import a Vita save")
            .setMessage("Pick a decrypted save of this game (from a Vita with VitaShell or Apollo, or from a PC "
                + "copy): its folder, or a .zip of it.\n\nA save in the same slot is replaced; the saves as they "
                + "were are kept in the game's save-backups folder.")
            .setCancelable(false)
            .setPositiveButton("Folder…", (d, w) -> pickFolder())
            .setNeutralButton(".zip file…", (d, w) -> pickZip())
            .setNegativeButton("Cancel", (d, w) -> finish())
            .show();
    }

    private void pickFolder() {
        startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), REQUEST_FOLDER);
    }

    private void pickZip() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT).addCategory(Intent.CATEGORY_OPENABLE)
            .setType("*/*")
            .putExtra(Intent.EXTRA_MIME_TYPES, new String[] { "application/zip", "application/x-zip-compressed", "application/octet-stream" });
        startActivityForResult(intent, REQUEST_ZIP);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_STEP) {
            int code = data != null ? data.getIntExtra(FrontendStepActivity.EXTRA_EXIT_CODE, -1) : -1;
            deleteTree(copied);
            done(code == 0, saveMessage());
            return;
        }
        Uri uri = resultCode == RESULT_OK && data != null ? data.getData() : null;
        if (uri == null) {
            finish();
            return;
        }
        String source;
        if (requestCode == REQUEST_FOLDER) {
            copied = new File(getCacheDir(), "save-import");
            deleteTree(copied);
            try {
                copyTree(uri, DocumentsContract.getTreeDocumentId(uri), copied, 0);
            } catch (Exception e) {
                Log.w(TAG, "copy", e);
                done(false, "the folder could not be read: " + e.getMessage());
                return;
            }
            source = copied.getAbsolutePath();
        } else {
            source = uri.toString(); // FrontendStepActivity opens it, as a link named <name>.zip
        }
        String[] args = { "--import-save", source, "--quiet" };
        startActivityForResult(new Intent(this, FrontendStepActivity.class).putExtra("args", args), REQUEST_STEP);
    }

    // a picked folder's files and subfolders (a save folder, perhaps with sce_sys/), into dir
    private void copyTree(Uri tree, String documentId, File dir, int depth) throws Exception {
        if (depth > 4)
            return;
        if (!dir.isDirectory() && !dir.mkdirs())
            throw new java.io.IOException("could not create " + dir);
        Uri children = DocumentsContract.buildChildDocumentsUriUsingTree(tree, documentId);
        String[] columns = { DocumentsContract.Document.COLUMN_DOCUMENT_ID, DocumentsContract.Document.COLUMN_DISPLAY_NAME,
            DocumentsContract.Document.COLUMN_MIME_TYPE, DocumentsContract.Document.COLUMN_SIZE };
        try (Cursor c = getContentResolver().query(children, columns, null, null, null)) {
            while (c != null && c.moveToNext()) {
                String id = c.getString(0), name = c.getString(1), mime = c.getString(2);
                if (name == null || name.contains("/") || name.equals("..") || name.equals("."))
                    continue;
                File target = new File(dir, name);
                if (DocumentsContract.Document.MIME_TYPE_DIR.equals(mime)) {
                    copyTree(tree, id, target, depth + 1);
                } else if (c.getLong(3) <= 64L * 1024 * 1024) {
                    Uri file = DocumentsContract.buildDocumentUriUsingTree(tree, id);
                    try (InputStream in = getContentResolver().openInputStream(file); OutputStream out = new FileOutputStream(target)) {
                        byte[] buffer = new byte[65536];
                        for (int n; in != null && (n = in.read(buffer)) > 0;)
                            out.write(buffer, 0, n);
                    }
                }
            }
        }
    }

    private static void deleteTree(File f) {
        if (f == null || !f.exists())
            return;
        File[] children = f.listFiles();
        if (children != null)
            for (File c : children)
                deleteTree(c);
        f.delete();
    }

    // the step's "Save: ..." line, the game's own message about what it imported (its log, newest first)
    private String saveMessage() {
        File base = getExternalFilesDir(null);
        File[] dirs = base != null ? base.listFiles(File::isDirectory) : null;
        if (dirs == null)
            return null;
        for (File dir : dirs) {
            File log = new File(dir, "vita3k.log");
            if (!log.isFile())
                continue;
            String found = null;
            try (BufferedReader r = new BufferedReader(new InputStreamReader(new FileInputStream(log), StandardCharsets.UTF_8))) {
                for (String line; (line = r.readLine()) != null;) {
                    int at = line.indexOf("Save: ");
                    if (at >= 0)
                        found = line.substring(at + 6);
                }
            } catch (Exception e) {
                Log.w(TAG, "log", e);
            }
            if (found != null)
                return found;
        }
        return null;
    }

    private void done(boolean ok, String message) {
        String text = message != null ? message : ok ? "The save is imported." : "The save could not be imported.";
        // the game's message names its folders in full: inside the app's folder is enough
        File base = getExternalFilesDir(null);
        if (base != null)
            text = text.replace(base.getAbsolutePath() + "/", "");
        new AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
            .setTitle(ok ? "Save imported" : "No save imported")
            .setMessage(text)
            .setCancelable(false)
            .setPositiveButton("Play", (d, w) -> {
                startActivity(new Intent(this, SetupActivity.class));
                finish();
            })
            .setNegativeButton("Close", (d, w) -> finish())
            .show();
    }
}
