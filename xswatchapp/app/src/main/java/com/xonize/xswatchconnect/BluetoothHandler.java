package com.xonize.xswatchconnect;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Handler;
import android.os.SystemClock;

import com.welie.blessed.BluetoothBytesParser;
import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothCentralManagerCallback;
import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.BluetoothPeripheralCallback;
import com.welie.blessed.BondState;
import com.welie.blessed.ConnectionPriority;
import com.welie.blessed.GattStatus;
import com.welie.blessed.HciStatus;

import com.welie.blessed.ScanFailure;
import com.welie.blessed.WriteType;

import org.jetbrains.annotations.NotNull;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.time.ZoneId;
import java.time.ZoneOffset;
import java.time.ZonedDateTime;
import java.util.UUID;

import timber.log.Timber;

import static com.welie.blessed.BluetoothBytesParser.FORMAT_UINT8;
import static com.welie.blessed.BluetoothBytesParser.string2bytes;


class BluetoothHandler {

    public static int MTU = 0;

    // Intent constants
    public static final String MEASUREMENT_STEPCOUNT = "xonize.xswatchconnect.measurement.stepcount";
    public static final String MEASUREMENT_STEPCOUNT_EXTRA = "xonize.xswatchconnect.measurement.stepcount.extra";

    public static final String COMMAND_CHANNEL = "xonize.xswatchconnect.channel.command";
    public static final String COMMAND_CHANNEL_EXTRA = "xonize.xswatchconnect.channel.command.extra";
    public static final String MEASUREMENT_EXTRA_PERIPHERAL = "xonize.xswatchconnect.measurement.peripheral";

    // UUIDs for the Device Information service (DIS)
    private static final UUID DEVICE_INFORMATION_SERVICE_UUID = UUID.fromString("0000180A-0000-1000-8000-00805F9B34FB");
    private static final UUID MANUFACTURER_NAME_CHARACTERISTIC_UUID = UUID.fromString("00002A29-0000-1000-8000-00805F9B34FB");
    private static final UUID MODEL_NUMBER_CHARACTERISTIC_UUID = UUID.fromString("00002A24-0000-1000-8000-00805F9B34FB");
    private static final UUID FIRMWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A26-0000-1000-8000-00805F9B34FB");
    private static final UUID HARDWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A27-0000-1000-8000-00805F9B34FB");
    private static final UUID SOFTWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A28-0000-1000-8000-00805F9B34FB");
    private static final UUID SYSTEM_ID_CHARACTERISTIC_UUID = UUID.fromString("00002A23-0000-1000-8000-00805F9B34FB");

    // UUIDs for the Battery Service (BAS)
    private static final UUID BATTERY_LEVEL_SERVICE_UUID = UUID.fromString("0000180F-0000-1000-8000-00805F9B34FB");
    private static final UUID BATTERY_LEVEL_CHARACTERISTIC_UUID = UUID.fromString("00002A19-0000-1000-8000-00805F9B34FB");

    // UUIDs for the Step Counter Service
    private static final UUID STEPCOUNT_SERVICE_UUID = UUID.fromString("00001809-0000-1000-8000-00805F9B34FB");
    private static final UUID STEPCOUNT_CHARACTERISTIC_UUID = UUID.fromString("00002B40-0000-1000-8000-00805F9B34FB");

    public static final UUID COMMAND_SERVICE_UUID = UUID.fromString("0dd0c28b-d173-43bf-9dce-f2446591366d");
    public static final UUID COMMAND_CHAR_UUID = UUID.fromString("fed4ded3-97f1-44ca-b7a3-116cf78d9e77");

    private static final UUID TIME_SERVICE_UUID = UUID.fromString("00001805-0000-1000-8000-00805F9B34FB");
    private static final UUID TIME_CHAR_UUID = UUID.fromString("00002A2B-0000-1000-8000-00805F9B34FB");

    // Local variables
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private final Context context;
    private final Handler handler = new Handler();

    public static boolean connected = false;

    // Callback for peripherals
    private final BluetoothPeripheralCallback peripheralCallback = new BluetoothPeripheralCallback() {
        @Override
        public void onServicesDiscovered(@NotNull BluetoothPeripheral peripheral) {
            // Request a higher MTU, iOS always asks for 185
            peripheral.requestMtu(185);
            // Request a new connection priority
            peripheral.requestConnectionPriority(ConnectionPriority.HIGH);

            // Send Time Data to sync device time
            long seconds;
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
                seconds = SystemClock.currentNetworkTimeClock().millis() / 1000;
            } else {
                seconds = System.currentTimeMillis() / 1000;
            }
            int offset  = ZonedDateTime.now().getOffset().getTotalSeconds();
            long overallSeconds = seconds + offset;
            byte[] secs = String.valueOf(overallSeconds).getBytes();
            peripheral.writeCharacteristic(TIME_SERVICE_UUID, TIME_CHAR_UUID, secs, WriteType.WITH_RESPONSE);
            peripheral.writeCharacteristic(COMMAND_SERVICE_UUID, COMMAND_CHAR_UUID, "UPDATE_TIME".getBytes(StandardCharsets.UTF_8), WriteType.WITH_RESPONSE);

            // Read info from the Device Information Service
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, MANUFACTURER_NAME_CHARACTERISTIC_UUID);
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, MODEL_NUMBER_CHARACTERISTIC_UUID);
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, FIRMWARE_REV_CHARACTERISTIC_UUID);
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, HARDWARE_REV_CHARACTERISTIC_UUID);
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, SOFTWARE_REV_CHARACTERISTIC_UUID);
            peripheral.readCharacteristic(DEVICE_INFORMATION_SERVICE_UUID, SYSTEM_ID_CHARACTERISTIC_UUID);
            peripheral.readPhy();

            // Try to turn on notifications for other characteristics
            peripheral.readCharacteristic(BATTERY_LEVEL_SERVICE_UUID, BATTERY_LEVEL_CHARACTERISTIC_UUID);
            peripheral.setNotify(STEPCOUNT_SERVICE_UUID, STEPCOUNT_CHARACTERISTIC_UUID, true);
            peripheral.setNotify(COMMAND_SERVICE_UUID, COMMAND_CHAR_UUID, true);
        }

        @Override
        public void onNotificationStateUpdate(@NotNull BluetoothPeripheral peripheral, @NotNull BluetoothGattCharacteristic characteristic, @NotNull GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                final boolean isNotifying = peripheral.isNotifying(characteristic);
                Timber.i("SUCCESS: Notify set to '%s' for %s", isNotifying, characteristic.getUuid());
            } else {
                Timber.e("ERROR: Changing notification state failed for %s (%s)", characteristic.getUuid(), status);
            }
        }

        @Override
        public void onCharacteristicWrite(@NotNull BluetoothPeripheral peripheral, @NotNull byte[] value, @NotNull BluetoothGattCharacteristic characteristic, @NotNull GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                // NOTE: if something breaks in writing, this is probably culprit! changed bytes2String(value) to new String(value, StandardCharsets.UTF_8)
                // as it seems to have been removed from the library
                Timber.i("SUCCESS: Writing <%s> to <%s>", new String(value, StandardCharsets.UTF_8), characteristic.getUuid());
            } else {
                Timber.i("ERROR: Failed writing <%s> to <%s> (%s)", new String(value, StandardCharsets.UTF_8), characteristic.getUuid(), status);
            }
        }

        @Override
        public void onCharacteristicUpdate(@NotNull BluetoothPeripheral peripheral, @NotNull byte[] value, @NotNull BluetoothGattCharacteristic characteristic, @NotNull GattStatus status) {
            if (status != GattStatus.SUCCESS) {
                Timber.e("UNSUCCESSFUL CHARACTERISTIC UPDATE");
                return;
            }

            UUID characteristicUUID = characteristic.getUuid();
            BluetoothBytesParser parser = new BluetoothBytesParser(value);

            if (characteristicUUID.equals(BATTERY_LEVEL_CHARACTERISTIC_UUID)) {
                int batteryLevel = parser.getIntValue(FORMAT_UINT8);
                Timber.i("Received battery level %d%%", batteryLevel);
            } else if (characteristicUUID.equals(MANUFACTURER_NAME_CHARACTERISTIC_UUID)) {
                String manufacturer = parser.getStringValue(0);
                Timber.i("Received manufacturer: %s", manufacturer);
            } else if (characteristicUUID.equals(MODEL_NUMBER_CHARACTERISTIC_UUID)) {
                String modelNumber = parser.getStringValue(0);
                Timber.i("Received modelnumber: %s", modelNumber);
            } else if (characteristicUUID.equals(FIRMWARE_REV_CHARACTERISTIC_UUID)) {
                String firmwareRev = parser.getStringValue(0);
                Timber.i("Received firmware revision: %s", firmwareRev);
            } else if (characteristicUUID.equals(HARDWARE_REV_CHARACTERISTIC_UUID)) {
                String hardwareRev = parser.getStringValue(0);
                Timber.i("Received hardware revision: %s", hardwareRev);
            } else if (characteristicUUID.equals(SOFTWARE_REV_CHARACTERISTIC_UUID)) {
                String softwareRev = parser.getStringValue(0);
                Timber.i("Received software revision: %s", softwareRev);
            } else if (characteristicUUID.equals(SYSTEM_ID_CHARACTERISTIC_UUID)) {
                String systemID = parser.getStringValue(0);
                Timber.i("Received system ID: %s", systemID);
            } else if (characteristicUUID.equals(STEPCOUNT_CHARACTERISTIC_UUID)) {
                Integer stepcount = parser.getIntValue(BluetoothBytesParser.FORMAT_UINT32);
//                Timber.d("received stepcount %d", stepcount);
                Intent intent = new Intent(MEASUREMENT_STEPCOUNT);
                intent.putExtra(MEASUREMENT_STEPCOUNT_EXTRA, stepcount);
                sendMeasurement(intent, peripheral);
            } else if (characteristicUUID.equals(COMMAND_CHAR_UUID)) {
                Intent intent = new Intent(COMMAND_CHANNEL);
                String command = parser.getStringValue();
                intent.putExtra(COMMAND_CHANNEL_EXTRA, command);
                sendMeasurement(intent, peripheral);
            }
        }

        @Override
        public void onMtuChanged(@NotNull BluetoothPeripheral peripheral, int mtu, @NotNull GattStatus status) {
            Timber.i("new MTU set: %d", mtu);
            MTU = mtu;
        }

        private void sendMeasurement(@NotNull Intent intent, @NotNull BluetoothPeripheral peripheral ) {
            intent.putExtra(MEASUREMENT_EXTRA_PERIPHERAL, peripheral.getAddress());
            context.sendBroadcast(intent);
        }

    };

    // Callback for central
    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {

        @Override
        public void onConnectedPeripheral(@NotNull BluetoothPeripheral peripheral) {
            Timber.i("connected to '%s'", peripheral.getName());
            connected = true;
        }

        @Override
        public void onConnectionFailed(@NotNull BluetoothPeripheral peripheral, final @NotNull HciStatus status) {
            Timber.e("connection '%s' failed with status %s", peripheral.getName(), status);
        }

        @Override
        public void onDisconnectedPeripheral(@NotNull final BluetoothPeripheral peripheral, final @NotNull HciStatus status) {
            Timber.i("disconnected '%s' with status %s", peripheral.getName(), status);
            connected = false;

            // Reconnect to this device when it becomes available again
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    central.autoConnectPeripheral(peripheral, peripheralCallback);
                }
            }, 5000);
        }

        @Override
        public void onDiscoveredPeripheral(@NotNull BluetoothPeripheral peripheral, @NotNull ScanResult scanResult) {
            Timber.i("Found peripheral '%s'", peripheral.getName());
            central.stopScan();

            if (peripheral.getName().contains("Contour") && peripheral.getBondState() == BondState.NONE) {
                // Create a bond immediately to avoid double pairing popups
                central.createBond(peripheral, peripheralCallback);
            } else {
                central.connectPeripheral(peripheral, peripheralCallback);
            }
        }

        @Override
        public void onBluetoothAdapterStateChanged(int state) {
            Timber.i("bluetooth adapter changed state to %d", state);
            if (state == BluetoothAdapter.STATE_ON) {
                // Bluetooth is on now, start scanning again
                // Scan for peripherals with a certain service UUIDs
                central.startPairingPopupHack();
                startScan();
            }
        }

        @Override
        public void onScanFailed(@NotNull ScanFailure scanFailure) {
            Timber.i("scanning failed with error %s", scanFailure);
        }
    };

    public static synchronized BluetoothHandler getInstance(Context context) {
        if (instance == null) {
            instance = new BluetoothHandler(context.getApplicationContext());
        }
        return instance;
    }

    private BluetoothHandler(Context context) {
        this.context = context;

        // Create BluetoothCentral
        central = new BluetoothCentralManager(context, bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        try {
            central.startPairingPopupHack();
            startScan();
        } catch (SecurityException e) {
            e.printStackTrace();
        }

    }

    private void startScan() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                central.scanForPeripheralsWithServices(new UUID[]{STEPCOUNT_SERVICE_UUID});
            }
        },1000);
    }
}