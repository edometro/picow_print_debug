#!/bin/bash

while true; do
  curl -s http://192.168.40.1/usb | tee -a output.txt
  usleep 50000   # 50ms
done
