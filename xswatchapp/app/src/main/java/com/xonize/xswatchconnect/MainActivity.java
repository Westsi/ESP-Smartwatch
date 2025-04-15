package com.xonize.xswatchconnect;

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
import android.net.Uri;
import android.os.Build;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationManagerCompat;

import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothPeripheral;
import com.xonize.xswatchconnect.functionality.BluetoothHandler;
import com.xonize.xswatchconnect.functionality.CommandDataReceiver;
import com.xonize.xswatchconnect.functionality.NLService;
import com.xonize.xswatchconnect.functionality.SpotifyReceiver;
import com.xonize.xswatchconnect.functionality.ZoneIDToPosixString;
import com.xonize.xswatchconnect.utils.ByteConversions;
import com.xonize.xswatchconnect.utils.RealPathUtil;
import com.xonize.xswatchconnect.utils.UpdaterUtils;

import org.jetbrains.annotations.NotNull;

import java.io.File;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
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

    private static final int FILE_PICK = 56;
    private final DateFormat dateFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.ENGLISH);
    public static String notificationData = "";
    public static ArrayList<String> notifications = new ArrayList<>();

    private NotificationReceiver nReceiver;
    public static SpotifyReceiver sReceiver;
    public CommandDataReceiver commandDataReceiver;

    public UpdaterDataReceiver updaterDataReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        ZoneIDToPosixString.setupmap();
        reference = this;
        getCodeCacheDir().setReadOnly();
        new File(getFilesDir().getPath() + "/data/com.xonize.xswatchconnect/code_cache/.overlay/base.apk/classes3.dex").setReadOnly();
        // Says it can't resolve method in some cases but compiles and works fine.
        Timber.plant(new Timber.DebugTree());
//        Timber.d("CREATED MAINACTIVITY");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        measurementValue = findViewById(R.id.deviceconnectiondata);
        txtView = findViewById(R.id.infodata);
        txtView.setText("Hello!");
        registerReceiver(stepcountDataReceiver, new IntentFilter(BluetoothHandler.MEASUREMENT_STEPCOUNT), RECEIVER_EXPORTED);
        startService(new Intent(this, NLService.class));

        if (!(NotificationManagerCompat.getEnabledListenerPackages(getApplicationContext()).contains(getApplicationContext().getPackageName()))) {
            Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
            startActivity(intent);

        }

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(NLService.NOTIFICATION_ACTION);
        registerReceiver(nReceiver, filter, RECEIVER_EXPORTED);

        commandDataReceiver = new CommandDataReceiver();
        IntentFilter cfilter = new IntentFilter();
        cfilter.addAction(BluetoothHandler.COMMAND_CHANNEL);
        registerReceiver(commandDataReceiver, cfilter, RECEIVER_EXPORTED);

        updaterDataReceiver = new UpdaterDataReceiver();
        IntentFilter ufilter = new IntentFilter();
        ufilter.addAction(BluetoothHandler.UPDATER_CHANNEL);
        registerReceiver(updaterDataReceiver, ufilter, RECEIVER_EXPORTED);

        sReceiver = new SpotifyReceiver();
        IntentFilter sfilter = new IntentFilter();
        sfilter.addAction("com.spotify.music.playbackstatechanged");
        sfilter.addAction("com.spotify.music.metadatachanged");
        sfilter.addAction("com.spotify.music.queuechanged");
        registerReceiver(sReceiver, sfilter, RECEIVER_EXPORTED);

        //get the current notifications by broadcasting an intent
        Intent i = new Intent(NLService.GET_NOTIFICATION_INTENT);
        i.putExtra("command", "list");
        sendBroadcast(i);

        updateStatusText();

        Button selectFile = findViewById(R.id.select_file);
        selectFile.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Timber.d("Selecting File!!!!!");
                if (checkExternal()) {
                    Timber.d("external all good!");
                    Intent chooseFile = new Intent(Intent.ACTION_GET_CONTENT);
                    chooseFile.setType("*/*");
                    chooseFile = Intent.createChooser(chooseFile, "Choose a file");
                    startActivityForResult(chooseFile, FILE_PICK);
                } else {
                    requestExternal();
                    // TODO: check and request external functions, onActivityResult (search in other for FILE_PICK)
                }
            }
        });

        Button uploadUpdate = findViewById(R.id.upload_update);
        uploadUpdate.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Timber.d("Uploading Update!!");
                /*
                * From looking through source, what I can gather:
                * generate() splits the selected update file into parts that can be sent OTA (len < MTU-3)
                * val parts is the number of parts
                * 0xFD byte signals to the device that it is about to start sending, device prepares update file
                * 0xFE byte signals that the file size is being sent, the shifts are just to make it fit into uint8 on receiving
                * 0xFF byte signals that the number of parts is being sent, along with the MTU - again, bitshifts to fit into uint8
                *
                * After this, the watch is in "update mode" and will then request the first part
                * - see onDataReceived comments below for more info
                * */

                /*
                * Functions to Write:
                * generate()
                * sendData(byte[])
                * byteArrayOfInts(int[])
                * */

                /*
                startOta = System.currentTimeMillis()
                clearData()
                val parts = generate()
                FG.parts = parts
                if (FG().sendData(byteArrayOfInts(0xFD))) {
                    Toast.makeText(this, "Uploading file", Toast.LENGTH_SHORT).show()
                    buttonUpload.visibility = View.INVISIBLE

                    val len = File(this.cacheDir, UPDATE_FILE).length()
                    FG().sendData(byteArrayOfInts(0xFE, ((len shr 24) and 0xFF).toInt(), ((len shr 16) and 0xFF).toInt(), ((len shr 8) and 0xFF).toInt(), ((len) and 0xFF).toInt()))

                    FG().sendData(
                            byteArrayOfInts(
                                    0xFF,
                                    parts / 256,
                                    parts % 256,
                                    mtu / 256,
                                    mtu % 256
                            )
                    )
                    //FG().sendData(this, 0)
                    start = System.currentTimeMillis()
                */

                UpdaterUtils.clearData();
                int parts = UpdaterUtils.generateParts();
                updaterDataReceiver.parts = parts;
                if (updaterDataReceiver.sendData(ByteConversions.byteArrayOfInts(0xFD))) {
                    Toast.makeText(MainActivity.reference.getApplicationContext(), "Uploading File", Toast.LENGTH_SHORT).show();
                    uploadUpdate.setVisibility(View.INVISIBLE);

                    long len = new File(MainActivity.reference.getCacheDir(), UpdaterUtils.UPDATE_FILE).length();
                    updaterDataReceiver.sendData(ByteConversions.byteArrayOfInts(
                            0xFE,
                            (int) (len >> 24) & 0xFF,
                            (int) (len >> 16) & 0xFF,
                            (int) (len >> 8) & 0xFF,
                            (int) (len) & 0xFF
                    ));

                    updaterDataReceiver.sendData(ByteConversions.byteArrayOfInts(
                            0xFF,
                            parts / 256,
                            parts % 256,
                            BluetoothHandler.MTU / 256,
                            BluetoothHandler.MTU % 256
                    ));
                } else {
                    Toast.makeText(MainActivity.reference.getApplicationContext(), "Device not connected", Toast.LENGTH_SHORT).show();
                }

            }
        });
    }

    /*
    * This is what is called when the device requests an update part
    * If the byte is 0xF1, this indicates a request for a specific part - this is encoded in bytes 1 and 2, bitshifted again
    * As far as I can see, 0xAA is never sent, so IDK why that is there
    * 0xF2 is the completion byte, this is the device saying it has done downloading
    * 0x0F is the output message from the update actually being performed
    *
    *
    *
    * Functions to Write:
    * sendData(context, int)
    *
    *
    override fun onDataReceived(data: Data) {

        if (data.getByte(0) == (0xF1).toByte()) {
            val next = ((data.getByte(1)!!.toPInt()) * 256) + (data.getByte(2)!!.toPInt())
            sendData(context, next)
        }
        if (data.getByte(0) == (0xAA).toByte()){
            fastMode = data.getByte(1) == (0x01).toByte()
            Toast.makeText(context, "Fastmode: $fastMode", Toast.LENGTH_SHORT).show()
            if (fastMode) {
                uploadData(context)
            } else {
                sendData(context, 0)
            }
        }
        if (data.getByte(0) == (0xF2).toByte()) {
            Timber.w("Transfer complete")
            Toast.makeText(context, "Transfer Complete", Toast.LENGTH_SHORT).show()
            notifyProgress("Finishing up", 100, context)
            ProgressReceiver().getProgress(100, "Transfer Complete")
            Handler().postDelayed({
                cancelNotification(SERVICE_ID2, context)
            }, 2000)
        }
        if (data.getByte(0) == (0x0F).toByte()){
            val textArray = ByteArray(data.size()-1)
            for (x in 1 until data.size()){
                textArray[x-1] = data.getByte(x)!!
            }
            Timber.e(String(textArray))
            ProgressReceiver().getProgress(101, String(textArray))
        }

        MN().onDataReceived(data)

    }
    * */



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

    public BluetoothPeripheral getPeripheral(String peripheralAddress) {
        BluetoothCentralManager central = BluetoothHandler.getInstance(getApplicationContext()).central;
        return central.getPeripheral(peripheralAddress);
    }

    public static void updateNotifications() {
        notificationData = "";
        notifications.clear();
//        Timber.d("Updating Notifications");
        Intent i = new Intent(NLService.GET_NOTIFICATION_INTENT);
        i.putExtra("command", "list");
        reference.sendBroadcast(i);
    }

    private void checkPermissions() {
        String[] missingPermissions = getMissingPermissions(getRequiredPermissions());
        if (missingPermissions.length > 0) {
            requestPermissions(missingPermissions, ACCESS_LOCATION_REQUEST);
        } else {
            permissionsGranted();
        }
    }

    private boolean checkExternal() {
        String[] externalPerms = new String[]{Manifest.permission.READ_MEDIA_AUDIO, Manifest.permission.READ_MEDIA_VIDEO, Manifest.permission.READ_MEDIA_IMAGES};
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            for (String p : externalPerms) {
                if (ActivityCompat.checkSelfPermission(reference.getApplicationContext(), p) != PackageManager.PERMISSION_GRANTED) {
                    return false;
                }
            }
            return true;
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && ActivityCompat.checkSelfPermission(
                reference.getApplicationContext(),
                Manifest.permission.READ_EXTERNAL_STORAGE
        ) != PackageManager.PERMISSION_GRANTED) {
            return false;
        }
        return true;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Timber.d("RequestCode=" + requestCode + ", ResultCode=" + resultCode + ", Data=" + (data != null) + String.valueOf(data));
        if (resultCode == MainActivity.RESULT_OK) {
            if (data != null && requestCode == FILE_PICK) {
                Uri selectedFile = data.getData();
                String[] filePathColumn = new String[]{MediaStore.Files.FileColumns.DATA};
                if (selectedFile != null) {
                    String filePath = RealPathUtil.getRealPath(this, selectedFile);
                    if (filePath != null) {
                        Timber.d("onActivityResult saving file!");
                        UpdaterUtils.saveFile(new File(filePath), null);
                    }
                }
            }
        }
    }

    private void requestExternal() {
        String[] externalPerms = new String[]{Manifest.permission.READ_MEDIA_AUDIO, Manifest.permission.READ_MEDIA_VIDEO, Manifest.permission.READ_MEDIA_IMAGES};
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ActivityCompat.requestPermissions(
                    this,
                    externalPerms,
                    20
            );
        } else {
            ActivityCompat.requestPermissions(
                    this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    20
            );
        }
    }

    private String[] getMissingPermissions(String[] requiredPermissions) {
        List<String> missingPermissions = new ArrayList<>();
        for (String requiredPermission : requiredPermissions) {
            if (getApplicationContext().checkSelfPermission(requiredPermission) != PackageManager.PERMISSION_GRANTED) {
                missingPermissions.add(requiredPermission);
            }
        }
        return missingPermissions.toArray(new String[0]);
    }

    private String[] getRequiredPermissions() {
        int targetSdkVersion = getApplicationInfo().targetSdkVersion;
        if (targetSdkVersion >= Build.VERSION_CODES.S) {
            return new String[]{Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.FOREGROUND_SERVICE};
        } else if (targetSdkVersion >= Build.VERSION_CODES.Q) {
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
            Timber.d("GRANT_RESULTS=%s", Arrays.toString(grantResults));
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
                            + "\nSpotify Data: \n" + sReceiver.getSongData();
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
                if (!notifications.contains(temp)) {
                    notifications.add(temp);
                }
                updateStatusText();
            }
        }
    }
}