# M5Stack BLEChorder
BLE Chordpad App

This is a M5Stack app which provides functions to play chords via Bluetooth Low Energy MIDI.

[![photo](images/1.thumb.jpg)](images/1.jpg)

[🎥Walkthrough Video](https://youtu.be/04s6wCNV70E)

## Note
Currently, it works only when connected to iOS😢   

It uses 1.3M byte of program memory.  
If you use M5Stack 4MB-flash model, change the partition to "Huge App (3MB No OTA)".

Contributors welcome!

# How to Install

## Using SDUpdater(or LovyanLauncher) (recommended)
1. Install [SDUpdater](https://github.com/tobozo/M5Stack-SD-Updater) or [LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher) into your M5Stack.
2. Download M5Chorder_0.1.0_bin.zip from [Releases](https://github.com/wararyo/M5Chorder/releases).
3. Prepare an SD Card and copy files like the following.
4. Insert the SD into the M5Stack and execute.

```
(SD Root)
/
├BLEChorder.bin
├json
│└BLEChorder.json
└jpg
  ├BLEChorder.jpg
  └BLEChorder_gh.jpg
```

## Using PlatformIO
1. Install Visual Studio Code.
2. Install PlatformIO.
3. Clone the repository.
4. Open repository folder in VSCode and run "PlatformIO: Upload".

## Using esptool (untested)
1. Install esptool.
2. Download M5Chorder_0.1.0_bin.zip from [Releases](https://github.com/wararyo/M5Chorder/releases).
3. Run the following.

``` sh
# Case of 16MB-flash model and of default 16MB partition
esptool --chip esp32 --port /dev/tty.SLAB_USBtoUART --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_freq 40m 0x10000 BLEChorder.bin
```
