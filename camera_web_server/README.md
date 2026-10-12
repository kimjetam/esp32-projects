# Camera Web Server

## Common issues

When experiencing problems with esp32 cam device, try setting different value of XCLK (8 or 18), 20 is default. Also try setting CLK 2x and Auto PCLK.

References:
- https://github.com/espressif/arduino-esp32/issues/4655#issuecomment-763287559
- https://www.reddit.com/r/esp32/comments/101behv/esp32_cam_extremly_unstable_and_slow/
- https://www.reddit.com/r/esp32/comments/r9g5jc/fixing_ymmv_the_poor_frame_rate_on_the_esp32cam/  