package com.example.arthur.serial_sensor_logger;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import java.io.*;
import android.hardware.usb.*;
import android.util.Log;
import android.widget.TextView;
import android.os.Handler;

import com.hoho.android.usbserial.driver.*;

public class MainActivity extends AppCompatActivity {
    private TextView status_values;
    UsbSerialDriver usb;
    byte buf[] = new byte[1024];
    int num;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
                            runOnUiThread(new Runnable(){
                                @Override
                                public void run(){
                                    status_values.setText(new String(buf, 0, num));
                                }
                            });
                            Log.v("arduino", new String(buf, 0, num)); // Arduinoから受信した値をlogcat出力
                        Thread.sleep(10);//TODO データ欠落がないか確認(全15項目)
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
}
