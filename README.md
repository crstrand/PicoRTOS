# RP2040 Pico FreeRTOS Application Template

This is a template you may use to start a FreeRTOS application for the RP2040 Pico board.<br>
It only uses FreeRTOS single core.<br>
The template app will only echo characters on one (or more if configured) USB serial ports.<br>
FreeRTOS Symmetrical MultiProcessing (SMP) is a whole OTHER ~~pain~~ adventure

## Prerequisites
2 RaspberryPi RP2040 Picos<br><br>
Learn cmake.  I'm not kidding.  It will be SUPER painfull if you don't.<br><br>
Follow some RP2040 Pico tutorials to get your build environment set up. Like this one from [DigiKey](https://youtu.be/B5rQSoOmR5w)<br>
[This one](https://www.digikey.ca/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0) will help setup VSCode and a spare Pico as a picoprobe debugger (VERY handy)<br><br>
Learn FreeRTOS.<br>
Follow some FreeRTOS YouTube tutorials like this one from [Learn Embedded Systems](https://youtu.be/jCZxStjzGA8)

## Getting started
Clone [RP2040 SDK](https://github.com/raspberrypi/pico-sdk.git) <br>
and [FreeRTOS Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel.git) <br>
into generic and accessible directories.  I used `C:\Users\Public\...`<br>
Set environment variables: `PICO_SDK_PATH` and `FREERTOS_KERNEL_PATH` to point to the appropriate directories.<br>
pico_sdk_import.cmake will then take care of finding the Pico SDK files for you.<br>
FreeRTOS_Kernel_import.cmake will include those files for you.

## Gotchas
If you enable any of the "hook" options in FreeRTOSConfig.h, you MUST create them in freertos_hook.c or you will get errors.

## Compiling
In a MinGW bash window (Git Bash):<br>
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
```
the cmake command above will look one directory up for the CMakeLists.txt file and create all the files required to 'make' the application in the current directory (`./build`).<br>
While in `./build`
```bash
make -j4  # use 4 threads to compile.  You can try more (8?) if you like
```
If all went well, you should be rewarded with 6 files:<br>

```
picoRTOS.bin
picoRTOS.dis
picoRTOS.elf
picoRTOS.elf.map
picoRTOS.hex
picoRTOS.uf2
```
If you don't know what to do with those files by now, you really shouldn't be trying this :innocent: