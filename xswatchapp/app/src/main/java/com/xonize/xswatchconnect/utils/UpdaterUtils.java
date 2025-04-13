package com.xonize.xswatchconnect.utils;

import com.xonize.xswatchconnect.MainActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;

import timber.log.Timber;

public class UpdaterUtils {

    public static int PART = 16384;
    public static String UPDATE_FILE = "update.bin";

    public static int generateParts() {
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

}
