#!/bin/sh
QT_VERSION=6.5.0
BUILD_DIR=../build_macOS

# build application
cmake -DCMAKE_PREFIX_PATH=/Users/florian/Qt/$QT_VERSION/macos -S .. -B $BUILD_DIR -G Ninja
cd $BUILD_DIR
ninja

# create DMG using macdeployqt tool
/Users/florian/Qt/$QT_VERSION/macos/bin/macdeployqt Accountancy.app -dmg -always-overwrite