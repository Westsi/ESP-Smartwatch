package com.xonize.xswatchconnect;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemClock;

import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.WriteType;

import java.nio.charset.StandardCharsets;
import java.util.Objects;
import java.util.TimeZone;

import timber.log.Timber;

public class CommandDataReceiver extends BroadcastReceiver {

    boolean isSending = false;
    @Override
    public void onReceive(Context context, Intent intent) {
        Timber.d("COMMAND RECEIVED");
        BluetoothPeripheral peripheral = MainActivity.reference.getPeripheral(intent.getStringExtra(BluetoothHandler.MEASUREMENT_EXTRA_PERIPHERAL));
        String data = (String) intent.getSerializableExtra(BluetoothHandler.COMMAND_CHANNEL_EXTRA);
        if (isSending) {
            return;
        }
        if (Objects.equals(data, "UPDATE_NOTIFS")) {
            Timber.d("Updating notifications");
            isSending = true;
            sendNotifications(context, peripheral);
        }

        else if (Objects.equals(data, "SYNC_SPOTIFY")) {
            Timber.d("Syncing spotify");
            isSending = true;
            syncSpotify(context, peripheral);
        }

        else if (Objects.equals(data, "TOGGLE_SPOTIFY_PLAYING")) {
            MainActivity.sReceiver.toggleSpotifyPlay();
        }

        else if (Objects.equals(data, "SKIP_PREV_SPOTIFY")) {
            MainActivity.sReceiver.prevSpotify();
        }

        else if (Objects.equals(data, "SKIP_NEXT_SPOTIFY")) {
            MainActivity.sReceiver.nextSpotify();
        }

        else if (Objects.equals(data, "SYNC_TIME")) {
            Timber.d("Syncing time");
            isSending = true;
            syncTime(context, peripheral);
        }
    }

    private void sendNotifications(Context ctx, BluetoothPeripheral peripheral) {
//        MainActivity.updateNotifications();
        Timber.d(String.valueOf(MainActivity.notifications.size()));
        for (String notif : MainActivity.notifications) {
            MessageClipper messageClipper = new MessageClipper(notif, 32);
            while (!messageClipper.messageComplete()) {
                peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                        BluetoothHandler.COMMAND_CHAR_UUID,
                        messageClipper.getNextMessage().getBytes(),
                        WriteType.WITH_RESPONSE);
            }
            peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                    BluetoothHandler.COMMAND_CHAR_UUID,
                    "END_NOTIF".getBytes(), // let device know that this is the end of the notification
                    WriteType.WITH_RESPONSE);
        }
        peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                BluetoothHandler.COMMAND_CHAR_UUID,
                "END_ALL".getBytes(), // let device know that this is the end of the transmission
                WriteType.WITH_RESPONSE);
        isSending = false;

    }

    private void syncSpotify(Context ctx, BluetoothPeripheral peripheral) {
        String songData = MainActivity.sReceiver.getWatchFormattedSongData();
        MessageClipper messageClipper = new MessageClipper(songData, 32);
        while (!messageClipper.messageComplete()) {
            peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                    BluetoothHandler.COMMAND_CHAR_UUID,
                    messageClipper.getNextMessage().getBytes(),
                    WriteType.WITH_RESPONSE);
        }
        peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                BluetoothHandler.COMMAND_CHAR_UUID,
                "END_SONG".getBytes(), // let device know that this is the end of the song
                WriteType.WITH_RESPONSE);
        peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID,
                BluetoothHandler.COMMAND_CHAR_UUID,
                "END_ALL".getBytes(), // let device know that this is the end of the transmission
                WriteType.WITH_RESPONSE);
        isSending = false;

    }

    private void syncTime(Context ctx, BluetoothPeripheral peripheral) {
        long seconds;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            seconds = SystemClock.currentNetworkTimeClock().millis() / 1000;
        } else {
            seconds = System.currentTimeMillis() / 1000;
        }
        String tz = ZoneIDToPosixString.ids.get(TimeZone.getDefault().toZoneId().toString());
        Timber.d(tz);
        byte[] secs = String.valueOf(seconds).getBytes();
        byte[] tzs = tz.getBytes();
        peripheral.writeCharacteristic(BluetoothHandler.TIME_SERVICE_UUID, BluetoothHandler.TIME_CHAR_UUID, secs, WriteType.WITH_RESPONSE);
        peripheral.writeCharacteristic(BluetoothHandler.TIME_SERVICE_UUID, BluetoothHandler.TIME_ZONE_CHAR_UUID, tzs, WriteType.WITH_RESPONSE);
        peripheral.writeCharacteristic(BluetoothHandler.COMMAND_SERVICE_UUID, BluetoothHandler.COMMAND_CHAR_UUID, "UPDATE_TIME".getBytes(StandardCharsets.UTF_8), WriteType.WITH_RESPONSE);
        isSending = false;
    }
}
