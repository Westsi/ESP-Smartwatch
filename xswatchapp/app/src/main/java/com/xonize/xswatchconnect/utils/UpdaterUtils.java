package com.xonize.xswatchconnect.utils;

import android.net.Uri;

import com.xonize.xswatchconnect.MainActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;

import timber.log.Timber;

public class UpdaterUtils {

    public static int PART = 16384;
    public static String UPDATE_FILE = "update.bin";

    public static int  generateParts() {
        byte[] bytes;
        try {
            bytes = Files.readAllBytes(new File(MainActivity.reference.getCacheDir(), "update.bin").toPath());
        } catch (IOException e) {
            e.printStackTrace();
            return 0;
        }
        int s = bytes.length / PART;

        for (int i=0;i<s;i++) {
            byte[] data = new byte[PART];
            for (int j=0;j<PART;j++) {
                data[j] = bytes[(i*PART)+j];
            }
            saveData(data, i);
        }

        if (bytes.length % PART == 0) {
            return (bytes.length / PART);
        } else {
            return (bytes.length / PART) + 1;
        }
    }

    public static void saveData(byte[] bytes, int pos) {
        File directory = MainActivity.reference.getCacheDir();
        if (!directory.exists()) {
            directory.mkdirs();
        }
        File upload = new File(directory, "data");
        if (!upload.exists()) {
            upload.mkdirs();
        }
        File data = new File(upload, "data" + pos + ".bin");
        try {
            FileOutputStream fos = new FileOutputStream(data, true);
            fos.write(bytes);
            fos.flush();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void clearData() {
        File directory = MainActivity.reference.getCacheDir();
        if (!directory.exists()) {
            directory.mkdirs();
        }
        File upload = new File(directory, "data");
        if (upload.exists()) {
            if (!deleteDir(upload)) {
                Timber.e("Could not clear data");
            }
        }
    }

    private static boolean deleteDir(File dir) {
        File[] contents = dir.listFiles();
        boolean success = true;
        if (contents != null) {
            for (File file : contents) {
                success = success & deleteDir(file);
            }
        }
        return success & dir.delete();
    }

    public static void saveFile(File src, Uri uri) {
        File dir = MainActivity.reference.getCacheDir();
        Timber.d("Directory Path:%s", dir.getAbsolutePath());
        if (!dir.exists()) {
            dir.mkdirs();
        }
        File dst = new File(dir, UPDATE_FILE);
        if (dst.exists()) {
            dst.delete();
        }
        if (src != null) {
            File info = new File(dir, "info.txt");
            try (FileOutputStream fos = new FileOutputStream(info)) {
                fos.write(src.getName().getBytes(StandardCharsets.UTF_8));
            } catch (IOException e) {
                e.printStackTrace();
            }

            try (FileInputStream in = new FileInputStream(src);
                 FileOutputStream out = new FileOutputStream(dst)) {

                byte[] buf = new byte[1024];
                int len;
                while ((len = in.read(buf)) > 0) {
                    out.write(buf, 0, len);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            // Append mode FileOutputStream to flush
            try (FileOutputStream fos = new FileOutputStream(dst, true)) {
                fos.flush(); // Just flushes, doesn't write anything
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (uri != null) {
            File info = new File(dir, "info.txt");
            try (FileOutputStream fos = new FileOutputStream(info)) {
                fos.write("firmware.bin".getBytes(StandardCharsets.UTF_8));
            } catch (IOException e) {
                e.printStackTrace();
            }

            try (InputStream in = MainActivity.reference.getContentResolver().openInputStream(uri);
                 FileOutputStream out = new FileOutputStream(dst)) {

                byte[] buf = new byte[1024];
                int len;
                while ((len = in.read(buf)) > 0) {
                    out.write(buf, 0, len);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            // Append-mode FileOutputStream, just to flush
            try (FileOutputStream fos = new FileOutputStream(dst, true)) {
                fos.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
