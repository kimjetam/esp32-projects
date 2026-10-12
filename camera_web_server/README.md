# ESP32-CAM Web Server

A web server for an ESP32-CAM that lets you access the camera over Wi-Fi from a web browser.

## Features

* Connects the ESP32-CAM to a Wi-Fi network.
* Provides browser-based access to the camera.
* Allows you to view the camera stream over your local network.

## Setup

1. Open the project in your Arduino IDE or compatible development environment.
2. Configure your Wi-Fi credentials in the project.
3. Select the correct ESP32 board and serial port.
4. Connect the ESP32-CAM to your computer and upload the firmware.
5. Open the serial monitor and wait for the device to connect to Wi-Fi.
6. Open the IP address printed in the serial monitor in a browser on the same network.

**Note:** Keep your Wi-Fi credentials private. Do not commit real passwords to a public repository.

## Troubleshooting

### Slow or unstable camera stream

If the camera stream is slow, laggy, or unstable, try changing the camera's XCLK frequency.

In the ESP32 camera configuration, look for:

```cpp
config.xclk_freq_hz = 20000000;
```

Try changing the value to:

```cpp
config.xclk_freq_hz = 8000000;
```

If the problem persists, try:

```cpp
config.xclk_freq_hz = 18000000;
```

The default in this example is 20 MHz. A lower clock frequency may improve stability on some ESP32-CAM modules, although the best value depends on the hardware and camera configuration.

**Observed result:** Reducing XCLK from 20 MHz to 8 MHz resolved severe streaming lag in one tested setup. Copying the firmware flash image from a working device to the affected device did not resolve the issue, suggesting that the problem was related to the individual hardware or its operating conditions rather than simply the firmware image.

### Other settings to try

If changing XCLK does not help, experiment with the following camera settings, if supported by your firmware or camera configuration:

* **CLK 2x:** Try enabling or disabling it.
* **Auto PCLK:** Try enabling or disabling automatic pixel-clock adjustment.

Change one setting at a time and compare the results. Record the configuration that works best for your particular board.

### Wi-Fi connection problems

* Verify the Wi-Fi credentials.
* Check that the ESP32-CAM is within reliable range of the router.
* Confirm that your phone or computer is connected to the same local network.
* Check the serial monitor for connection errors and the assigned IP address.

### Camera initialization failures

* Verify that the selected camera model and pin configuration match your hardware.
* Check the camera module and its connections.
* Try a different XCLK frequency if initialization or streaming remains unreliable.

## References

* [ESP32 Arduino issue #4655 — camera stability discussion](https://github.com/espressif/arduino-esp32/issues/4655#issuecomment-763287559)
* [ESP32-CAM extremely unstable and slow — Reddit discussion](https://www.reddit.com/r/esp32/comments/101behv/esp32_cam_extremly_unstable_and_slow/)
* [Fixing poor frame rate on the ESP32-CAM — Reddit discussion](https://www.reddit.com/r/esp32/comments/r9g5jc/fixing_ymmv_the_poor_frame_rate_on_the_esp32cam/)
