Const ForReading = 1
Const ForWriting = 2

Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objFile = objFSO.OpenTextFile(Wscript.Arguments.Item(0), ForReading)

strText = objFile.ReadAll
objFile.Close
strNewText = Replace(strText, chr(10)+"void", chr(10)+"static void")
strNewText = Replace(strNewText, chr(10)+"int", chr(10)+"static int")
strNewText = Replace(strNewText, chr(10)+"uint", chr(10)+"static uint")
strNewText = Replace(strNewText, chr(10)+"float", chr(10)+"static float")

Set objStdOut = WScript.StdOut
objStdOut.Write(strNewText)
