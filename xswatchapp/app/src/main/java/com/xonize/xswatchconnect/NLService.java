package com.xonize.xswatchconnect;

/*
* Credits:
* Matthew Bellafaire (bellafaire on Github) who's code this references heavily
* https://github.com/Bellafaire/ESP32-Smart-Watch
* */

import android.app.Notification;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

import timber.log.Timber;

public class NLService extends NotificationListenerService {
    final static int maxTextLen = 240;

    public final static String NOTIFICATION_ACTION = "xonize.xswatchconnect.notification_listener";
    public final static String GET_NOTIFICATION_INTENT = "xonize.xswatchconnect.notification_listener_service";
    private NLReceiver nlReceiver;

    @Override
    public void onCreate() {
//        Timber.d("CREATED NLSERVICE");
        super.onCreate();
        nlReceiver = new NLReceiver();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(GET_NOTIFICATION_INTENT);
        registerReceiver(nlReceiver, intentFilter, RECEIVER_EXPORTED);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(nlReceiver);
    }

    public void onNotificationPosted(StatusBarNotification sbn) {
        Intent i = new Intent(NOTIFICATION_ACTION);
        i.putExtra("notification_status_event", "onNotificationPosted :" + sbn.getPackageName() + "n");
        i.putExtra("event_type", "posted");
        sendBroadcast(i);

        MainActivity.updateNotifications();
    }
    public void onNotificationRemoved(StatusBarNotification sbn) {
        Intent i = new Intent(NOTIFICATION_ACTION);
        i.putExtra("notification_status_event", "onNotificationPosted :" + sbn.getPackageName() + "n");
        i.putExtra("event_type", "removed");
        sendBroadcast(i);

        MainActivity.updateNotifications();
    }

    class NLReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
//            Timber.d("received notification in nlservice");
            if (intent.getStringExtra("command").equals("clearall")) {
                NLService.this.cancelAllNotifications();
            } else if (intent.getStringExtra("command").equals("list")) {
                Intent i1 = new Intent(NOTIFICATION_ACTION);
                i1.putExtra("notification_event", "");
                sendBroadcast(i1);
                for (StatusBarNotification sbn : NLService.this.getActiveNotifications()) {
                    Intent i2 = new Intent(NOTIFICATION_ACTION);
                    //reference for pulling information out of notification objects http://gmariotti.blogspot.com/2013/11/notificationlistenerservice-and-kitkat.html
                    try {
                        //parse the data out of the statusbar notification object and format it into a string
                        //format appName,Title;ExtraText,ExtraInfoText,ExtraSubText,ExtraTitle;Description;
                        Bundle extras = sbn.getNotification().extras;
                        CharSequence title = extras.getCharSequence(Notification.EXTRA_TITLE, "");
                        CharSequence text = extras.getCharSequence(Notification.EXTRA_TEXT, "");
                        CharSequence infoText = extras.getCharSequence(Notification.EXTRA_INFO_TEXT, "");
                        CharSequence subText = extras.getCharSequence(Notification.EXTRA_SUB_TEXT, "");
                        CharSequence bigTitle = extras.getCharSequence(Notification.EXTRA_TITLE_BIG, "");
                        String data = ifNotNull(getAppNameFromPkgName(context, sbn.getPackageName())) + ","
                        + (title.length() == 0 ? "" : title + ",")
                        + (text.length() == 0 ? "" : text + ",")
                        + (infoText.length() == 0 ? "" : infoText + ",")
                        + (subText.length() == 0 ? "" : subText + ",")
                        + (bigTitle.length() == 0 ? "" : bigTitle) + "\n\n\n";


                        //remove non-ascii characters, i guess if you want emoji on your other device then keep this
                        data = data.replaceAll("[^\\p{ASCII}]", "");
                        try {
                            if (sbn.getNotification().category.equals(Notification.CATEGORY_EMAIL)) {
                                data += shortenString(sbn.getNotification().extras.getCharSequence("android.bigText")).replace("\n", "").replace(";", ",");
                            } else if (sbn.getNotification().category.equals(Notification.CATEGORY_MESSAGE)) {
                                data += ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_MESSAGES));
                            }
                        } catch (Exception e) {

                        }
                        if (!data.contains("XSWatch Connect") && !data.contains("Spotify")) {
                            i2.putExtra("notification_event", data + "\n");
                            sendBroadcast(i2);
                        }
                    } catch (Exception e) {
                        Timber.e("Could not parse data for: " + getAppNameFromPkgName(context, sbn.getPackageName()) + " due to " + e.getMessage());
                    }

                }
                Intent i3 = new Intent(NOTIFICATION_ACTION);
                i3.putExtra("notification_event", "");
                sendBroadcast(i3);
            }
        }
    }
    public static String shortenString(CharSequence s) {
        if (s.length() > maxTextLen) {
            return s.toString().substring(0, maxTextLen) + "...";
        } else {
            return s.toString();
        }
    }

    //this just cleans code above a bit. i don't need to send a bunch of "null" over bluetooth
    public static String ifNotNull(String str) {
        if (str != null) {
            return str;
        } else {
            return "";
        }

    }


    //https://stackoverflow.com/questions/5841161/get-application-name-from-package-name
    public static String getAppNameFromPkgName(Context context, String Packagename) {
        try {
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo info = packageManager.getApplicationInfo(Packagename, PackageManager.GET_META_DATA);
            String appName = (String) packageManager.getApplicationLabel(info);
            return appName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return "";
        }
    }
}
