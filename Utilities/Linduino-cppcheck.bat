cd Linduino-check
del /S /Q LTSketchbook
xcopy ..\..\LTSketchbook LTSketchbook /e
cd LTSketchbook
FOR /R %%x IN (*.ino) DO ren "%%x" *.cpp
cd ..
cppcheck\cppcheckgui LTSketchbook
DEL /S /Q LTSketchbook
cd ..
