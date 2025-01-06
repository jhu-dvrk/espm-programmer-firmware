# ESPM Programmer firmware

This is the firmware for ESPM Programmer, not ESPM.

Modified from Xilinx application note XAPP058.

It reads `espm.xsvf` on the SD card and replays it on the JTAG port at power on.

## Build

1. Install PlatformIO
2. In project root, `pio run -e gd32`

There is support for both STM32F3C8T6 and GD32E3C8T6. All production units have GD32E3C8T6 thanks to the chip shortage. GD32E3C8T6 is much faster.

## Upload

Use ST Link.

You can find the `firmware.bin` in `.pio/build/gd32` and use your favorite tool to upload, or use PlatformIO: `pio run -e gd32 -t upload`

You can also use `st-flash`: `st-flash --reset write firmware.bin 0x8000000`

Once programmed and reset, you should see two yellow LED blinks.

## Blink codes

Every SVF command toggles the blue light. When completed without error, blue light turns on then turn off.

On error, you will see blinking yellow LED. Count the blinks.

```
2: SD card hardware or filesystem problem
3: Can't find/open espm.xsvf
4: XSVF_ERROR_UNKNOWN
5: XSVF_ERROR_TDOMISMATCH
6: XSVF_ERROR_MAXRETRIES   /* TDO mismatch after max retries */
7: XSVF_ERROR_ILLEGALCMD
8: XSVF_ERROR_ILLEGALSTATE
9: XSVF_ERROR_DATAOVERFLOW  /* Data > lenVal MAX_LEN buffer size*/
```


