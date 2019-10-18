package com.example.arthur.serial_sensor_logger;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import java.io.*;
import android.hardware.usb.*;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.text.DateFormat;
import java.util.List;

import com.hoho.android.usbserial.driver.*;

public class MainActivity extends AppCompatActivity {
    private TextView status_values;
    private TextView usb_status;
    UsbSerialDriver usb;
    byte buf[] = new byte[2048];
    int num;
    boolean log_flg = false;
    boolean save_flg = false;
    UsbSerialPort port;

    //現在時刻を使用する
    Calendar calendar = Calendar.getInstance();
    DateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmS");
    String date = dateFormat.format(calendar.getTime()) + ".csv";

    //受信バッファ格納用
    List<String>receiveDataList = new ArrayList<>();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        verifyStoragePermissions(this);

        //usb接続状態表示
        status_values = (TextView)findViewById(R.id.status_value);
        status_values.setText(new String("Disconnected!"));

        //STARTボタン
        Button buttonStart = findViewById(R.id.start_button);
        buttonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { //Dialog();
                log_flg = true;

            }

        });

        //STOPボタン
        Button buttonStop = findViewById(R.id.stop_button);
        buttonStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { //Dialog();
                log_flg = false;
                save_flg = true;

            }

        });

        //センサデータ表示
        //status_values = (TextView)findViewById(R.id.status_value);
        
        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);

        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(manager);
        if (availableDrivers.isEmpty()) {
            return;
        }
        UsbSerialDriver driver = availableDrivers.get(0);
        UsbDeviceConnection connection = manager.openDevice(driver.getDevice());
        if (connection == null) {
            // You probably need to call UsbManager.requestPermission(driver.getDevice(), ..)
            return;
        }
        port = driver.getPorts().get(0);
        if (port != null) {
            status_values.setText(new String("Connected"));
            try {
                port.open(connection);
                port.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
                start_read_thread(); // シリアル通信を読むスレッドを起動

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        /*
        usb = UsbSerialProber.acquire(manager);
        if (usb != null) {
            status_values.setText(new String("Connected"));
            try {
                usb.open();
                usb.setBaudRate(115200);

                start_read_thread(); // シリアル通信を読むスレッドを起動
            } catch (IOException e) {
                e.printStackTrace();
            }
        }*/
    }

    public void start_read_thread() {

        new Thread(new Runnable() {
            public void run() {
                try {
                    while (true) {
                    num = port.read(buf, buf.length);
                    if (num > 0 && log_flg) {
                        receiveDataList.add(new String(buf, 0, num));
                        //receiveDataList.add("\n");
                        //saveFile(date, new String(buf, 0, num));
                        //System.out.println(new String(buf, 0, num));
                        runOnUiThread(new Runnable() {//メインスレッド以外操作のスレッドでUIを操作
                            // @Override
                            public void run() {
                                status_values.setText(new String(buf, 0, num));
                            }
                        });

                        //Log.v("data_from_arduino", new String(buf, 0, num)); // Arduinoから受信した値をlogcat出力
                        //System.out.println(receiveDataList);
                        Thread.sleep(0);
                    }
                    else if(num > 0 && save_flg){
                            //System.out.println(receiveDataList);
                            saveFile(date, receiveDataList.toString());
                            System.out.println("log saved");
                            save_flg = false;
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public void saveFile(String file, String str/*float value*/) {

        // try-with-resources
        try {
            FileOutputStream fileOutputstream = new FileOutputStream(new File("/storage/emulated/0/serial_sensor_logger/" + file), true);
            fileOutputstream.write(str.getBytes());
            //System.out.println("保存が完了しました");

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    //外部ストレージに保存するためのパーミッション確認
    private static final int REQUEST_EXTERNAL_STORAGE_CODE = 0x01;
    private static String[] mPermissions = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };

    private static void verifyStoragePermissions(Activity activity) {
        int readPermission = ContextCompat.checkSelfPermission(activity, mPermissions[0]);
        int writePermission = ContextCompat.checkSelfPermission(activity, mPermissions[1]);

        if (writePermission != PackageManager.PERMISSION_GRANTED ||
                readPermission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                    activity,
                    mPermissions,
                    REQUEST_EXTERNAL_STORAGE_CODE
            );
        }
    }
}
