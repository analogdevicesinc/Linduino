#!/bin/bash
BASE=../
TARGET=LTSketchbook
echo $BASE/$TARGET
astyle --recursive --options=Linduino.style "$BASE/$TARGET/*.cpp" "$BASE/$TARGET/*.h" "$BASE/$TARGET/*.ino"
