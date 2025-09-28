while ($true) {
  curl http://192.168.40.1/usb | Tee-Object -FilePath output.txt -Append
  Start-Sleep -Milliseconds 50
}