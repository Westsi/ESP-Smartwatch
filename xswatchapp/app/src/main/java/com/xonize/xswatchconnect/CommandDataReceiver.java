package com.xonize.xswatchconnect;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.WriteType;

import java.util.Objects;

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

        if (Objects.equals(data, "SYNC_SPOTIFY")) {
            Timber.d("Syncing spotify");
            isSending = true;
            syncSpotify(context, peripheral);
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
}
