# esp32-projects

## Flashing the full binary image

Use esptool python package. Install it by using `pip install esptool`. Then use these commands:

```
# read the flash image from device into a binary file
esptool --chip esp32 --port COM3 --baud 460800 read_flash 0x0 ALL backup_firmware.bin

# write the flash image into new device
esptool --chip esp32 --port COM3 --baud 115200 write_flash 0x0 backup_firmware.bin
```

