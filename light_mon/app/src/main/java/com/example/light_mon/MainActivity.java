package com.example.light_mon;

import static java.lang.Thread.sleep;
import static java.nio.charset.StandardCharsets.*;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.View;


import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothManager;

import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import java.util.Vector;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MainActivity extends AppCompatActivity {

    public static final String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB"; //SPP UUID
    private final static int REQUEST_ENABLE_BT = 1;
    private static final String TAG = "MY_APP_DEBUG_TAG";
    private final Lock[] first_lock = new Lock[4];
    private final Lock[] second_lock = new Lock[4];

    private Handler mHandler, handler;

    TextView tv1;
    TextView tv2;
    TextView tv3;
    TextView tv4;

    String mac1 = "00:0E:EA:CF:6D:E2";
    String mac2 = "00:0E:EA:CF:6E:A1";
    String mac3 = "00:0E:EA:CF:6D:5C";
    String mac4 = "00:0E:EA:CF:6D:2B";

    String mac[] = {mac1, mac2, mac3, mac4};

    Vector<Vector<String>> read_data = new Vector<>(Arrays.asList(
            new Vector<String>(),
            new Vector<String>(),
            new Vector<String>(),
            new Vector<String>()
    ));

    Vector<Vector<String>> process_data = new Vector<>(Arrays.asList(
            new Vector<String>(),
            new Vector<String>(),
            new Vector<String>(),
            new Vector<String>()
    ));

    BluetoothManager bluetoothManager;
    BluetoothAdapter bluetoothAdapter;

    Charset encoding = UTF_8;
    private boolean started = false;
    private boolean date_set = false;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //final TextView Text = (TextView) findViewById(R.id.textView);


        final Button start_button = findViewById(R.id.start_button);
        final Button date_button = findViewById(R.id.date_button);

        //tv=new TextView[4];
        tv1 = (TextView) findViewById(R.id.tv1);
        tv2 = (TextView) findViewById(R.id.tv2);
        tv3 = (TextView) findViewById(R.id.tv3);
        tv4 = (TextView) findViewById(R.id.tv4);

        mHandler = new Handler() {
            @Override
            public void handleMessage (Message msg){
                String data = new String((String) msg.obj);
                tv1.setText(data);
            }
        };
//----------------------------------------------
        bluetoothManager = getSystemService(BluetoothManager.class);
        bluetoothAdapter = bluetoothManager.getAdapter();
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
        }
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();

        BluetoothDevice[] myDevice = new BluetoothDevice[4];

        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (int i = 0; i < 4; i++) {
                for (BluetoothDevice device : pairedDevices) {
                    String deviceName = device.getName();
                    String deviceHardwareAddress = device.getAddress(); // MAC address
                    if (Objects.equals(deviceHardwareAddress, mac[i])) {
                        myDevice[i] = device;
                    }
                }
            }
        }

//------------------------------------------------

        start_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (started == false) {
                    started = true;
                    date_set=true;
                    startRead(myDevice);
                } else {
                    //started = false;
                    //desligar as coisas
                }
                //      Text.setText("1");
            }
        });

        date_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (date_set == false) {
                    Log.d("akiii", "teste");
                    setDate(myDevice);
                }
            }
        });
    }

    protected void startRead(BluetoothDevice[] myDevice) {

        for (int i = 0; i < 4; i++) {
            first_lock[i] = new ReentrantLock();
            second_lock[i] = new ReentrantLock();
        }

        ConnectThread[] thread_start = new ConnectThread[4];
        HandleData[] thread_data = new HandleData[4];

        for (int i = 0; i < 4; i++) {
            thread_start[i] = new ConnectThread(myDevice[i], i);
            thread_start[i].run();
            thread_data[i] = new HandleData(i);
            thread_data[i].run();
        }
         PrintData print_thread = new PrintData();
         print_thread.run();

    }

    protected void setDate(BluetoothDevice[] myDevice) {

        ConnectThread[] thread_start = new ConnectThread[4];
        for (int i = 0; i < 4; i++) {
            thread_start[i] = new ConnectThread(myDevice[i], i, true);
            thread_start[i].run();
        }
    }

    // Defines several constants used when transmitting messages between the
    // service and the UI.
    private interface MessageConstants {
        public static final int MESSAGE_READ = 0;
        public static final int MESSAGE_WRITE = 1;
        public static final int MESSAGE_TOAST = 2;

        // ... (Add other message types here as needed.)
    }
    TextView returnTV(int i)
    {
        if(i==0)
        {
            return tv1;
        }
        else if(i==1)
        {
            return tv2;
        }
        else if(i==2)
        {
            return tv3;
        }
        else
        {
            return tv4;
        }
    }

    void printTV(int i,String data)
    {
        TextView tv= returnTV(i);
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            public void run() {
                tv.setText(data);
            }
        });

    }
    private class PrintData extends Thread {
        String[] data = new String[4];
        String data_final = null;
        Handler h = new Handler();
        int tempo = 500;

        public PrintData() {
            for (int i = 0; i < 4; i++) {
                data[i] = null;
            }
            h.postDelayed(r, tempo);
        }

        final Runnable r = new Runnable() {
            public void run() {
                do_this();
                h.postDelayed(r, tempo);
            }
        };

        public void run() {

                 h.postDelayed(r, tempo);
        }

        void do_this()
        {
            data_final = "";
            for (int i = 0; i < 4; i++) {
                if (second_lock[i].tryLock()) {
                    try {
                        if (!process_data.get(i).isEmpty()) {
                            data[i] = process_data.get(i).get(0);
                            process_data.get(i).remove(0);
                        }
                    } finally {
                        second_lock[i].unlock();
                    }
                }
                data_final += data[i];
            }
            for (int i = 0; i < 4; i++) {
                int finalI = i;
                    printTV(i,new String("Light Detector ") + String.valueOf(i+1)+ new String("\n") + data[i]);
            }

            try {
                sleep(10);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

    }
//-----------------------------------------------------------------------
    private class HandleData extends Thread {
        Vector<String> datas;
        Vector<String> datas_process;
        String data;
        boolean s;
        boolean e;
        private int Thread_ID;
        String intermediaria;
        Handler h= new Handler();
        int tempo = 100;

        final Runnable r = new Runnable() {
            public void run() {
                do_this();
                h.postDelayed(r,tempo);
            }
        };

        public HandleData(int id) {
            Thread_ID = id;
            data = "";
            s = false;
            e = false;
            datas = read_data.get(Thread_ID);
            datas_process = process_data.get(Thread_ID);
        }

        public void run() {
            h.postDelayed(r, tempo);
        }

        public void do_this()
        {
                //data="";
                intermediaria="";
                first_lock[Thread_ID].lock();
                if (!datas.isEmpty()) {
                    intermediaria = datas.get(0);
                    data = data.concat(intermediaria);
                   // printTV(3,data);
                    datas.remove(0);

                    /*new Handler(Looper.getMainLooper()).post(new Runnable() {
                        public void run() {
                            //OverviewFragment.refresh(getApplicationContext());
                            //System.out.println("yay");
                            tv.setText(data);
                        }
                    });*/
                }
                first_lock[Thread_ID].unlock();

                if (!s) {
                    if (data.contains("START")) {
                        s = true;
                        data = data.substring(data.indexOf("START")+6,data.length()-1);

                    }
                }
                if (!e && s) {
                    if (data.contains("END")) {

                        second_lock[Thread_ID].lock();
                        datas_process.add(data.substring(0, data.indexOf("END") - 1));
                        second_lock[Thread_ID].unlock();
                        data = "";//data.substring(data.indexOf("END"), data.length() - 1);
                        //printTV(0,data.substring(data.indexOf("END"), data.length() - 1));
                       // printTV(0,data.substring(0, data.indexOf("END") - 1));
                        s = false;

                    }
                }
                try {
                    sleep((long) 10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }


        }
    }


    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;
        private byte[] mmBuffer; // mmBuffer store for the stream
        private int Thread_ID;
        Handler h = new Handler();
        int numBytes;
        int size=1024*10;
        double time = 1500;
        boolean set_time=false;

        final Runnable r = new Runnable() {
            public void run() {
                do_this();
                h.postDelayed(r, (long) time);
            }
        };

        public ConnectedThread(BluetoothSocket socket, int id,boolean b) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            Thread_ID = id;
            set_time=b;
            // Get the input and output streams; using temp objects because
            // member streams are final.
            try {
                tmpIn = socket.getInputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating input stream", e);
            }
            try {
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating output stream", e);
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public ConnectedThread(BluetoothSocket socket, int id) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            Thread_ID = id;
            // Get the input and output streams; using temp objects because
            // member streams are final.
            try {
                tmpIn = socket.getInputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating input stream", e);
            }
            try {
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating output stream", e);
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            if(set_time==true)
            {
                write_tempo();
            }
            else {
                mmBuffer = new byte[size];
                // bytes returned from read()
                h.postDelayed(r, (long) time);
            }

        }
        public void do_this()
        {
                //Log.d("teste","teste");
                try {
                    // Read from the InputStream.
                    numBytes = mmInStream.read(mmBuffer, 0, size);
                    //if (numBytes > 0) {
                        if (first_lock[Thread_ID].tryLock()) {
                            try {
                                read_data.get(Thread_ID).add(new String(mmBuffer, UTF_8));
                                //Log.d("TAG",new String(mmBuffer, UTF_8));
                            } finally {
                                first_lock[Thread_ID].unlock();
                            }
                        }
                   // }
                    // Send the obtained bytes to the UI activity.
                    // readMsg = mHandler.obtainMessage(MessageConstants.MESSAGE_READ, numBytes, -1, mmBuffer);//readMsg.sendToTarget();
                } catch (IOException e) {
                    Log.d(TAG, "Input stream was disconnected", e);
                    //break;
                }
               // Log.d("TAG","estou aki");
                try {
                    sleep((long) time);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                //
           // h.postDelayed(r, 10);
        }


        // Call this from the main activity to send data to the remote device.
        public void write_tempo(){//()byte[] bytes) {
           // try {
                long n = System.currentTimeMillis();
                printTV(0, String.valueOf(n));

                ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
                buffer.putLong(n);
            try {
                mmOutStream.write(buffer.array());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // Call this method from the main activity to shut down the connection.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the connect socket", e);
            }
        }
    }

    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;
        private int ThreadID;
        boolean tempo = false;

        public ConnectThread(BluetoothDevice device, int id) {
            // Use a temporary object that is later assigned to mmSocket
            // because mmSocket is final.
            BluetoothSocket tmp = null;
            mmDevice = device;
            ThreadID = id;
            try {
                // Get a BluetoothSocket to connect with the given BluetoothDevice.
                // MY_UUID is the app's UUID string, also used in the server code.
                tmp = device.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
            } catch (IOException e) {
                Log.e("TAG", "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public ConnectThread(BluetoothDevice device, int id, boolean b) {
            // Use a temporary object that is later assigned to mmSocket
            // because mmSocket is final.
            tempo=b;
            BluetoothSocket tmp = null;
            mmDevice = device;
            ThreadID = id;
            try {
                // Get a BluetoothSocket to connect with the given BluetoothDevice.
                // MY_UUID is the app's UUID string, also used in the server code.
                tmp = device.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
            } catch (IOException e) {
                Log.e("TAG", "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            // Cancel discovery because it otherwise slows down the connection.
            bluetoothAdapter.cancelDiscovery();

            try {
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                mmSocket.connect();
            } catch (IOException connectException) {
                // Unable to connect; close the socket and return.
                try {
                    mmSocket.close();
                } catch (IOException closeException) {
                    Log.e("TAG", "Could not close the client socket", closeException);
                }

                return;
            }

            // The connection attempt succeeded. Perform work associated with
            // the connection in a separate thread.
            manageMyConnectedSocket(mmSocket);
        }

        void manageMyConnectedSocket(BluetoothSocket mmSocket) {
            if(tempo)
            {
                c_thread[ThreadID] = new ConnectedThread(mmSocket, ThreadID, tempo);
            }
            else {
                c_thread[ThreadID] = new ConnectedThread(mmSocket, ThreadID);
            }
            c_thread[ThreadID].run();
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e("TAG", "Could not close the client socket", e);
            }
        }
    }
    ConnectedThread[] c_thread =  new ConnectedThread[4];


}