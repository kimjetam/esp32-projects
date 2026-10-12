# esp32-projects

## Flashing the Full Binary Image

Install the `esptool` Python package:

```bash
pip install esptool
```

### Read the Flash Image

Read the full flash image from the ESP32 and save it to a binary file:

```bash
esptool --chip esp32 --port COM3 --baud 115200 read_flash 0x0 ALL backup_firmware.bin
```

### Write the Flash Image

Write the saved binary image to another ESP32:

```bash
esptool --chip esp32 --port COM3 --baud 115200 write_flash 0x0 backup_firmware.bin
```

For more info, refer to the [documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/scripting.html#esptool.cmds.write_flash).