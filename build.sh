#!/bin/bash

appname=$(ls *.bin | awk -F "." {'print $1'})

minGW32-make -j8
if [ $? -eq 0 ]; then
  # success
  filedate=$(date -r ${appname}.bin "+%Y%d%m %H:%M:%S")
  filesize=$(du -h "${appname}.bin" | cut -f1)
  githash=$(git rev-parse --short HEAD)
  gitdirty=$(git diff --quiet || echo '+dirty')
  if [ $? -ne 0 ]; then
    printf "\n${appname}.bin ${filesize} ${filedate} (no git info available)}\n\n"
  else
    printf "\n${appname}.bin ${filesize} ${filedate} g${githash}${gitdirty}\n\n"
  fi
  if [ -d /d ]; then
    printf "copying ${appname}.uf2 to Pico\n\n"
    cp ${appname}.uf2 /d
  else
    printf "*****************************************************\n"
    printf " Pico not in bootloader mode. Hold BOOT and restart.\n"
    printf "*****************************************************\n"
  fi
else
  printf "build failed\n"
fi
