cp -r ../LTSketchbook LTSketchbook
find ./ -name *.ino -exec sh -c 'mv "$1" "${1%.iso}.cpp"' _ {} \;
cppcheck LTSketchbook -q --enable=warning 2>cppcheck.log
rm -rf LTCSketchbook