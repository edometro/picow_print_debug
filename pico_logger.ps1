while ($true) {
  curl http://192.168.4.1/usb | Tee-Object -FilePath output.txt -Append
  Start-Sleep -Milliseconds 50
}
