#!/usr/bin/env bash

make || exit 1
./qemu.sh
