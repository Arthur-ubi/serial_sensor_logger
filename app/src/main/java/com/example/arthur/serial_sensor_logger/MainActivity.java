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
import android.os.Handler;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import com.hoho.android.usbserial.driver.*;

public class MainActivity extends AppCompatActivity {
    private TextView status_values;
    UsbSerialDriver usb;
    byte buf[] = new byte[2048];
    int num;
    boolean temp_flg;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        verifyStoragePermissions(this);

        //STARTボタン
        Button buttonSave_s = findViewById(R.id.start_button);
        buttonSave_s.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { //Dialog();
                temp_flg = true;

            }

        });

        //センサデータ表示
        status_values = (TextView)findViewById(R.id.status_value);

        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
        usb = UsbSerialProber.acquire(manager);
        if (usb != null) {
            try {
                usb.open();
                usb.setBaudRate(115200);
                start_read_thread(); // シリアル通信を読むスレッドを起動
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void start_read_thread() {

        new Thread(new Runnable() {
            public void run() {
                try {
                    while (true) {
                        num = usb.read(buf, buf.length);
                        if (num > 0)
                            saveFile("XXX.csv", new String(buf, 0, num));
                            runOnUiThread(new Runnable(){//メインスレッド以外操作のスレッドでUIを操作
                                @Override
                                public void run(){
                                    status_values.setText(new String(buf, 0, num));
                                }
                            });
                            Log.v("arduino", new String(buf, 0, num)); // Arduinoから受信した値をlogcat出力
                        Thread.sleep(0);//TODO データ欠落がないか確認(全15項目)
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

            System.out.println("保存が完了しました");


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
