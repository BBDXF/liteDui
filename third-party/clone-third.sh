#!/bin/bash

echo "clone yoga, C++ 20, v3.2.1"
git clone -b v3.2.1 --depth=1 https://github.com/facebook/yoga.git
echo "delete 'add_subdirectory(tests)' in yoga/CMakeLists.txt to disable tests"

# download skia
# https://github.com/HumbleUI/SkiaBuild/releases/download/m143-da51f0d60e/Skia-m143-da51f0d60e-linux-Release-x64.zip
curl -L -o skia.zip https://github.com/HumbleUI/SkiaBuild/releases/download/m143-da51f0d60e/Skia-m143-da51f0d60e-linux-Release-x64.zip
mkdir -p skia
unzip skia.zip -d skia
rm skia.zip

