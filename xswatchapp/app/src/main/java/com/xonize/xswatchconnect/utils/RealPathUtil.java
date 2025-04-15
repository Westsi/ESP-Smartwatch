package com.xonize.xswatchconnect.utils;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.text.TextUtils;

import androidx.loader.content.CursorLoader;

import java.util.Arrays;

public class RealPathUtil {
    public static String getRealPath(Context context, Uri fileUri) {
        if (Build.VERSION.SDK_INT < 19) {
            return getRealPathFromURIAPI11to18(context, fileUri);
        } else {
            return getRealPathFromURIAPI19(context, fileUri);
        }
    }

    private static String getRealPathFromURIAPI11to18(Context context, Uri fileUri) {
        String[] proj = new String[]{MediaStore.Images.Media.DATA};
        String result = null;

        CursorLoader cursorLoader = new CursorLoader(context, fileUri, proj, null, null, null);
        Cursor cursor = cursorLoader.loadInBackground();

        if (cursor != null) {
            int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            result = cursor.getString(columnIndex);
            cursor.close();
        }

        return result;
    }

    private static String getRealPathFromURIAPI19(Context context, Uri fileUri) {
        boolean isKitKat = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;

        // Document Provider
        if (isKitKat && DocumentsContract.isDocumentUri(context, fileUri)) {
            // External Storage Provider
            if (isExternalStorageDocument(fileUri)) {
                String docID = DocumentsContract.getDocumentId(fileUri);
                String[] temp = docID.split(":");
                int newLength = temp.length;
                while (newLength > 0 && temp[newLength - 1].isEmpty()) {newLength--;}
                String[] split = Arrays.copyOf(temp, newLength);

                String type = split[0];
                if ("primary".equalsIgnoreCase(type)) {
                    return Environment.getExternalStorageDirectory().toString() + "/" + split[1];
                }
            } else if (isDownloadsDocument(fileUri)) {
                Cursor cursor = null;
                try {
                    cursor = context.getContentResolver().query(fileUri, new String[]{MediaStore.MediaColumns.DISPLAY_NAME}, null, null, null);
                    cursor.moveToNext();
                    String fileName = cursor.getString(0);
                    String path = Environment.getExternalStorageDirectory().toString() + "/Download/" + fileName;
                    if (!TextUtils.isEmpty(path)) {
                        return path;
                    }
                } finally {
                    if (cursor != null) {
                        cursor.close();
                    }
                }
                String id = DocumentsContract.getDocumentId(fileUri);
                if (id.startsWith("raw:")) {
                    return id.replaceFirst("raw:", "");
                }
                Uri contentUri = ContentUris.withAppendedId(Uri.parse("content://downloads"), Long.parseLong(id));

                return getDataColumn(context, contentUri, null, null);
            } else if (isMediaDocument(fileUri)) {
                String docID = DocumentsContract.getDocumentId(fileUri);
                String[] temp = docID.split(":");
                int newLength = temp.length;
                while (newLength > 0 && temp[newLength - 1].isEmpty()) {newLength--;}
                String[] split = Arrays.copyOf(temp, newLength);

                String type = split[0];
                Uri contentUri = null;
                switch (type) {
                    case "image":
                        contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                        break;
                    case "video":
                        contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                        break;
                    case "audio":
                        contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                        break;
                }
                String selection = "_id=?";
                String[] selectionArgs = new String[]{split[1]};

                return getDataColumn(context, contentUri, selection, selectionArgs);
            }
        } else if ("content".equalsIgnoreCase(fileUri.getScheme())) {
            if (isGooglePhotosUri(fileUri)) {
                return fileUri.getLastPathSegment();
            } else {
                return getDataColumn(context, fileUri, null, null);
            }
        } else if ("file".equalsIgnoreCase(fileUri.getScheme())) {
            return fileUri.getPath();
        }
        return null;
    }
    private static String getDataColumn(Context context, Uri contentUri, String selection, String[] selectionArgs) {
        Cursor cursor = null;
        String column = "_data";
        String[] projection = new String[]{column};
        try {
            cursor = context.getContentResolver().query(contentUri, projection, selection, selectionArgs, null);
            if (cursor != null && cursor.moveToFirst()) {
                int index = cursor.getColumnIndexOrThrow(column);
                return cursor.getString(index);
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return null;
    }

    private static boolean isExternalStorageDocument(Uri uri) {
        return "com.android.externalstorage.documents".equals(uri.getAuthority());
    }

    private static boolean isDownloadsDocument(Uri uri) {
        return "com.android.providers.downloads.documents".equals(uri.getAuthority());
    }

    private static boolean isMediaDocument(Uri uri) {
        return "com.android.providers.media.documents".equals(uri.getAuthority());
    }

    private static boolean isGooglePhotosUri(Uri uri) {
        return "com.google.android.apps.photos.content".equals(uri.getAuthority());
    }
}
