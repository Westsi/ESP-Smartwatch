package com.xonize.xswatchconnect;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.WriteType;
import com.xonize.xswatchconnect.functionality.BluetoothHandler;
import com.xonize.xswatchconnect.utils.ByteConversions;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Arrays;

import timber.log.Timber;

public class UpdaterDataReceiver extends BroadcastReceiver {
    private final int SERVICE_ID = 6108;
    private final int SERVICE_ID2 = 6109;
    public boolean foregroundServiceRunning = false;

    public int prt = 0, parts = 0;
    public boolean fastMode = false;

    private int startID = 0;
    private Context context;
    private boolean isReconnect = false;

    @Override
    public void onReceive(Context context, Intent intent) {
        BluetoothPeripheral peripheral = MainActivity.reference.getPeripheral(intent.getStringExtra(BluetoothHandler.MEASUREMENT_EXTRA_PERIPHERAL));
        byte[] bytes = (byte[]) intent.getSerializableExtra(BluetoothHandler.UPDATER_CHANNEL_EXTRA);

        assert bytes != null;
        if (bytes[0] == (byte) 0xF1) {
            int nextID = (ByteConversions.toint8(bytes[1]) * 256) + ByteConversions.toint8(bytes[2]);
            sendData(context, nextID);
        }

        if (bytes[0] == (byte) 0xAA){
            fastMode = bytes[1] == (byte) 0x01;
            Toast.makeText(context, "Fastmode: " + fastMode, Toast.LENGTH_SHORT).show();
            if (fastMode) {
                uploadData(context);
            } else {
                sendData(context, 0);
            }
        }
        if (bytes[0] == (byte) 0xF2) {
            Timber.w("Transfer complete");
            Toast.makeText(context, "Transfer Complete", Toast.LENGTH_SHORT).show();
            Button ub = MainActivity.reference.findViewById(R.id.upload_update);
            ub.setVisibility(View.VISIBLE);
        }
        if (bytes[0] == (byte) 0x0F){
            byte[] textArray = new byte[bytes.length-1];

            for (int i=1;i<bytes.length;i++) {
                textArray[i-1] = bytes[i];
            }
            Timber.e(Arrays.toString(textArray));
        }

    }

    public void sendData(Context context, int pos) {
        File dir = new File(context.getCacheDir(), "data");
        byte[] data;
        try {
            data = Files.readAllBytes(new File(dir, "data" + pos + ".bin").toPath());
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        int s = BluetoothHandler.MTU-2;
        int total = data.length / s;

        for (int i=0;i<total;i++) {
            byte[] arr = new byte[s+2];
            arr[0] = (byte) 0xFB;
            arr[1] = (byte) i;
            for (int j=0;j<s;j++) {
                arr[j+2] = data[(i*s) + j];
            }
            sendData(arr);
        }

        if (data.length % s != 0) {
            byte[] arr = new byte[(data.length % s) + 2];
            arr[0] = (byte) 0xFB;
            arr[1] = (byte) total;
            for (int j=0;j<data.length%s;j++) {
                arr[j + 2] = data[(total * s) + j];
            }
            sendData(arr);
        }

        byte[] update = ByteConversions.byteArrayOfInts(0xFC, data.length / 256, data.length % 256, pos / 256, pos % 256);
        int cur = (int) ((((float) pos) / parts) * 100);
        transmitData(update, cur, context);

    }

    public boolean writeBytes(boolean fastMode, byte[] data) {
        if (!BluetoothHandler.connected) {
            return false;
        }
        if (fastMode) {
            Timber.w("HELP FASTMODE");
            return false;
        }
        return BluetoothHandler.connectedPeripheral.writeCharacteristic(
                BluetoothHandler.UPDATER_SERVICE_UUID, BluetoothHandler.UPDATER_TX_CHAR_UUID,
                data, WriteType.WITH_RESPONSE);
    }

    public void uploadData(Context context) {
        final int[] current = {0};
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (current[0] < parts) {
                    sendData(context, current[0]);
                    current[0]++;
                }
            }
        });
        thread.start();
    }

    private boolean transmitData(byte[] data, int progress, Context context) {
        if (BluetoothHandler.central != null) {
            return transmitData(fastMode, data, progress, context);
        }
        return false;
    }

    private boolean transmitData(boolean fastMode, byte[] data, int progress, Context context) {
        if (!BluetoothHandler.connected) {
            return false;
        }
        if (fastMode) {
            Timber.w("HELP FASTMODE");
            return false;
        }
        return BluetoothHandler.connectedPeripheral.writeCharacteristic(
                BluetoothHandler.UPDATER_SERVICE_UUID, BluetoothHandler.UPDATER_TX_CHAR_UUID,
                data, WriteType.WITH_RESPONSE);
    }

    public boolean sendData(byte[] data) {
        if (BluetoothHandler.central != null) {
            return writeBytes(fastMode, data);
        }
        return false;
    }
}
