#!/bin/bash

while true; do
  curl -s http://192.168.40.1/usb | tee -a output.txt
  sleep 0.05 # 50ms
done
