#!/bin/sh
set -x
swiftc main.swift -o MacOS/HelloApp \
  -sdk $(xcrun --show-sdk-path) \
  -framework SwiftUI \
  -parse-as-library
