#!/bin/bash

sudo systemctl enable bluetooth.service
sudo systemctl start bluetooth.service

sudo bluetoothctl -- power on

sudo bluetoothctl -- trust 00:0E:EA:CF:6D:E2
sudo bluetoothctl -- pair 00:0E:EA:CF:6D:E2

sudo bluetoothctl -- trust 00:0E:EA:CF:6E:A1
sudo bluetoothctl -- pair 00:0E:EA:CF:6E:A1

sudo bluetoothctl -- trust 00:0E:EA:CF:6D:5C
sudo bluetoothctl -- pair 00:0E:EA:CF:6D:5C

sudo bluetoothctl -- trust 00:0E:EA:CF:6D:2B
sudo bluetoothctl -- pair 00:0E:EA:CF:6D:2B

sudo rfcomm bind rfcomm1 00:0E:EA:CF:6D:E2
sudo chmod 666 /dev/rfcomm1

sudo rfcomm bind rfcomm4 00:0E:EA:CF:6D:2B
sudo chmod 666 /dev/rfcomm2

sudo rfcomm bind rfcomm3 00:0E:EA:CF:6D:5C
sudo chmod 666 /dev/rfcomm3

sudo rfcomm bind rfcomm2 00:0E:EA:CF:6E:A1
sudo chmod 666 /dev/rfcomm4
