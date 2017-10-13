del Linduino-cpplint.out
cd Linduino-check
del /S /Q LTSketchbook
xcopy ..\..\LTSketchbook LTSketchbook /e
cd LTSketchbook
FOR /R %%x IN (*.ino) DO ren "%%x" *.cpp
FOR /R %%x IN (*.ino) DO C:\python27\python.exe ..\cpplint\cpplint.py --filter=-whitespace,-build/include_order,-readability/braces "%%x" >> ..\..\Linduino-cpplint.out 2>&1
FOR /R %%x IN (*.cpp) DO C:\python27\python.exe ..\cpplint\cpplint.py --filter=-whitespace,-build/include_order,-readability/braces "%%x" >> ..\..\Linduino-cpplint.out 2>&1
FOR /R %%x IN (*.c) DO C:\python27\python.exe ..\cpplint\cpplint.py --filter=-whitespace,-build/include_order,-readability/braces "%%x" >> ..\..\Linduino-cpplint.out 2>&1
FOR /R %%x IN (*.h) DO C:\python27\python.exe ..\cpplint\cpplint.py --filter=-whitespace,-build/include_order,-readability/braces "%%x" >> ..\..\Linduino-cpplint.out 2>&1
cd ..
DEL /S /Q LTSketchbook
cd ..
ECHO Output has been written to Linduino-cpplint.out
