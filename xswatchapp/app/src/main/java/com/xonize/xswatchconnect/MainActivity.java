package com.xonize.xswatchconnect;

// https://github.com/Bellafaire/Android-Companion-App-For-BLE-Devices/blob/master/SmartwatchCompanionAppV2/app/src/main/java/com/example/smartwatchcompanionappv2/MainActivity.java

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Build;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationManagerCompat;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.welie.blessed.BluetoothBytesParser;
import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothPeripheral;

import org.jetbrains.annotations.NotNull;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Objects;

import timber.log.Timber;

public class MainActivity extends AppCompatActivity {

    public static MainActivity reference;

    private TextView measurementValue;

    private TextView txtView;
    private static final int REQUEST_ENABLE_BT = 1;
    private static final int ACCESS_LOCATION_REQUEST = 2;
    private final DateFormat dateFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.ENGLISH);
    public static String notificationData = "";

    private NotificationReceiver nReceiver;
    public static SpotifyReceiver sReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        reference = this;
        // Says it can't resolve method in some cases but compiles and works fine.
        Timber.plant(new Timber.DebugTree());
        Timber.d("CREATED MAINACTIVITY");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        measurementValue = findViewById(R.id.deviceconnectiondata);
        txtView = findViewById(R.id.infodata);
        txtView.setText("Hello!");
        registerReceiver(stepcountDataReceiver, new IntentFilter(BluetoothHandler.MEASUREMENT_STEPCOUNT));
        startService(new Intent(this, NLService.class));

        if (!(NotificationManagerCompat.getEnabledListenerPackages(getApplicationContext()).contains(getApplicationContext().getPackageName()))) {
            Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
            startActivity(intent);

        }

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(NLService.NOTIFICATION_ACTION);
        registerReceiver(nReceiver, filter);

        sReceiver = new SpotifyReceiver();
        IntentFilter sfilter = new IntentFilter();
        sfilter.addAction("com.spotify.music.playbackstatechanged");
        sfilter.addAction("com.spotify.music.metadatachanged");
        sfilter.addAction("com.spotify.music.queuechanged");
        registerReceiver(sReceiver, sfilter);

        //get the current notifications by broadcasting an intent
        Intent i = new Intent(NLService.GET_NOTIFICATION_INTENT);
        i.putExtra("command", "list");
        sendBroadcast(i);

        updateStatusText();
    }

    @SuppressLint("MissingPermission")
    @Override
    protected void onResume() {
        super.onResume();
        checkPermissions();

        if (getBluetoothManager().getAdapter() != null) {
            if (!isBluetoothEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                checkPermissions();
            }
        } else {
            Timber.e("This device has no Bluetooth hardware");
        }
    }

    private boolean isBluetoothEnabled() {
        BluetoothAdapter bluetoothAdapter = getBluetoothManager().getAdapter();
        if(bluetoothAdapter == null) return false;

        return bluetoothAdapter.isEnabled();
    }

    private void initBluetoothHandler()
    {
        BluetoothHandler.getInstance(getApplicationContext());
    }

    @NotNull
    private BluetoothManager getBluetoothManager() {
        return Objects.requireNonNull((BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE),"cannot get BluetoothManager");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(stepcountDataReceiver);
    }

    private final BroadcastReceiver stepcountDataReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            BluetoothPeripheral peripheral = getPeripheral(intent.getStringExtra(BluetoothHandler.MEASUREMENT_EXTRA_PERIPHERAL));
            Integer value = (Integer) intent.getSerializableExtra(BluetoothHandler.MEASUREMENT_STEPCOUNT_EXTRA);
            Timber.d("received value stepcount %d", value);
            if (value != null) {
                measurementValue.setText(String.format(Locale.ENGLISH, "%d from %s", value, peripheral.getName()));
            }
        }
    };

    private BluetoothPeripheral getPeripheral(String peripheralAddress) {
        BluetoothCentralManager central = BluetoothHandler.getInstance(getApplicationContext()).central;
        return central.getPeripheral(peripheralAddress);
    }

    public static void updateNotifications() {
        notificationData = "";
        Timber.d("Updating Notifications");
        Intent i = new Intent(NLService.GET_NOTIFICATION_INTENT);
        i.putExtra("command", "list");
        reference.sendBroadcast(i);
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            String[] missingPermissions = getMissingPermissions(getRequiredPermissions());
            if (missingPermissions.length > 0) {
                requestPermissions(missingPermissions, ACCESS_LOCATION_REQUEST);
            } else {
                permissionsGranted();
            }
        }
    }

    private String[] getMissingPermissions(String[] requiredPermissions) {
        List<String> missingPermissions = new ArrayList<>();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            for (String requiredPermission : requiredPermissions) {
                if (getApplicationContext().checkSelfPermission(requiredPermission) != PackageManager.PERMISSION_GRANTED) {
                    missingPermissions.add(requiredPermission);
                }
            }
        }
        return missingPermissions.toArray(new String[0]);
    }

    private String[] getRequiredPermissions() {
        int targetSdkVersion = getApplicationInfo().targetSdkVersion;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S && targetSdkVersion >= Build.VERSION_CODES.S) {
            return new String[]{Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.FOREGROUND_SERVICE};
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q && targetSdkVersion >= Build.VERSION_CODES.Q) {
            return new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.FOREGROUND_SERVICE};
        } else return new String[]{Manifest.permission.ACCESS_COARSE_LOCATION};
    }

    private void permissionsGranted() {
        // Check if Location services are on because they are required to make scanning work for SDK < 31
        int targetSdkVersion = getApplicationInfo().targetSdkVersion;
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S && targetSdkVersion < Build.VERSION_CODES.S) {
            if (checkLocationServices()) {
                initBluetoothHandler();
            }
        } else {
            initBluetoothHandler();
        }
    }

    private boolean areLocationServicesEnabled() {
        LocationManager locationManager = (LocationManager) getApplicationContext().getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null) {
            Timber.e("could not get location manager");
            return false;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            return locationManager.isLocationEnabled();
        } else {
            boolean isGpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
            boolean isNetworkEnabled = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);

            return isGpsEnabled || isNetworkEnabled;
        }
    }

    private boolean checkLocationServices() {
        if (!areLocationServicesEnabled()) {
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle("Location services are not enabled")
                    .setMessage("Scanning for Bluetooth peripherals requires locations services to be enabled.") // Want to enable?
                    .setPositiveButton("Enable", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.cancel();
                            startActivity(new Intent(android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS));
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // if this button is clicked, just close
                            // the dialog box and do nothing
                            dialog.cancel();
                        }
                    })
                    .create()
                    .show();
            return false;
        } else {
            return true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        Timber.d("%d permissions given, %d request code", permissions.length, requestCode);
        for (String perm : permissions) {
            Timber.d("\tpermission given: %s", perm);
        }
        if (permissions.length == 0 || grantResults.length == 0) return;


        // Check if all permission were granted
        boolean allGranted = true;
        for (int result : grantResults) {
            if (result != PackageManager.PERMISSION_GRANTED) {
                allGranted = false;
                break;
            }
        }

        if (allGranted) {
            permissionsGranted();
        } else {
//            new AlertDialog.Builder(MainActivity.this)
//                    .setTitle("Permission is required for scanning Bluetooth peripherals")
//                    .setMessage("Please grant permissions")
//                    .setPositiveButton("Retry", new DialogInterface.OnClickListener() {
//                        public void onClick(DialogInterface dialogInterface, int i) {
//                            dialogInterface.cancel();
//                            checkPermissions();
//                        }
//                    })
//                    .create()
//                    .show();
        }
    }

    public static void updateStatusText() {
        try {
            reference.runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    String statusText = (BluetoothHandler.connected ? "Connected to device" : "Not connected to device")
                            + "\nNotification Data: \n" + notificationData
                            + "\nSpotify Data> \n" + sReceiver.getSongData();
                    reference.txtView.setText(statusText);
                }
            });
        } catch (Exception e) {
            String statusText = "Connecting";
        }
    }

    class NotificationReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.hasExtra("notification_event")) {
                String temp = intent.getStringExtra("notification_event");
                if (!notificationData.contains(temp)) {
                    temp = intent.getStringExtra("notification_event") + "\n" + notificationData;
                    notificationData = temp.replace("\n\n", "\n");
                }
                updateStatusText();
            }
        }
    }
}