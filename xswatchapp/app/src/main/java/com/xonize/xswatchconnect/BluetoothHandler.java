package com.xonize.xswatchconnect.main;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;

import com.welie.blessed.BluetoothBytesParser;
import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothCentralManagerCallback;
import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.BluetoothPeripheralCallback;
import com.welie.blessed.BondState;
import com.welie.blessed.ConnectionPriority;
import com.welie.blessed.GattStatus;
import com.welie.blessed.HciStatus;

import com.welie.blessed.PhyOptions;
import com.welie.blessed.PhyType;
import com.welie.blessed.ScanFailure;
import com.welie.blessed.WriteType;

import org.jetbrains.annotations.NotNull;

import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.UUID;

import timber.log.Timber;

import static com.welie.blessed.BluetoothBytesParser.FORMAT_UINT8;


class BluetoothHandler {

    // Intent constants
    public static final String MEASUREMENT_STEPCOUNT = "xonize.measurement.stepcount";

    // UUIDs for the Device Information service (DIS)
    private static final UUID DEVICE_INFORMATION_SERVICE_UUID = UUID.fromString("00002A19-0000-1000-8000-00805F9B34FB");
    private static final UUID MANUFACTURER_NAME_CHARACTERISTIC_UUID = UUID.fromString("00002A29-0000-1000-8000-00805F9B34FB");
    private static final UUID MODEL_NUMBER_CHARACTERISTIC_UUID = UUID.fromString("00002A24-0000-1000-8000-00805F9B34FB");
    private static final UUID FIRMWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A26-0000-1000-8000-00805F9B34FB");
    private static final UUID HARDWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A27-0000-1000-8000-00805F9B34FB");
    private static final UUID SOFTWARE_REV_CHARACTERISTIC_UUID = UUID.fromString("00002A28-0000-1000-8000-00805F9B34FB");
    private static final UUID SYSTEM_ID_CHARACTERISTIC_UUID = UUID.fromString("00002A23-0000-1000-8000-00805F9B34FB");

    // UUIDs for the Battery Service (BAS)
    private static final UUID BATTERY_LEVEL_SERVICE_UUID = UUID.fromString("0000180F-0000-1000-8000-00805F9B34FB");
    private static final UUID BATTERY_LEVEL_CHARACTERISTIC_UUID = UUID.fromString("00002A19-0000-1000-8000-00805F9B34FB");

    // Local variables
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private final Context context;
    private final Handler handler = new Handler();

    // Callback for peripherals
    private final BluetoothPeripheralCallback peripheralCallback = new BluetoothPeripheralCallback() {
        @Override
        public void onServicesDiscovered(@NotNull BluetoothPeripheral peripheral) {
            // Request a higher MTU, iOS always asks for 185
            peripheral.requestMtu(185);
            // Request a new connection priority
            peripheral.requestConnectionPriority(ConnectionPriority.HIGH);

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
//            peripheral.setNotify(BLOOD_PRESSURE_SERVICE_UUID, BLOOD_PRESSURE_MEASUREMENT_CHARACTERISTIC_UUID, true);
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
            if (status != GattStatus.SUCCESS) return;

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
            }
        }

        @Override
        public void onMtuChanged(@NotNull BluetoothPeripheral peripheral, int mtu, @NotNull GattStatus status) {
            Timber.i("new MTU set: %d", mtu);
        }

    };

    // Callback for central
    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {

        @Override
        public void onConnectedPeripheral(@NotNull BluetoothPeripheral peripheral) {
            Timber.i("connected to '%s'", peripheral.getName());
        }

        @Override
        public void onConnectionFailed(@NotNull BluetoothPeripheral peripheral, final @NotNull HciStatus status) {
            Timber.e("connection '%s' failed with status %s", peripheral.getName(), status);
        }

        @Override
        public void onDisconnectedPeripheral(@NotNull final BluetoothPeripheral peripheral, final @NotNull HciStatus status) {
            Timber.i("disconnected '%s' with status %s", peripheral.getName(), status);

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

        // Plant a tree
        // TODO: figure out why the fuck this doesn't work
//        Timber.plant(new Timber.DebugTree());

        // Create BluetoothCentral
        central = new BluetoothCentralManager(context, bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        central.startPairingPopupHack();
        startScan();
    }

    private void startScan() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                // ### add HRS = HeartRate service to scan UUIDs
                // TODO: add step counter UUID ONLY into list
                central.scanForPeripheralsWithServices(new UUID[]{});
            }
        },1000);
    }
}