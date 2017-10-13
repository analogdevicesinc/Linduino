# -*- coding: utf-8 -*-
"""
Created on Thu Jan 07 15:14:54 2016

@author: MSajikumar
"""

# This script runs is used to run through all Linduino files, compile them and return errors.

import os
import subprocess
arduinoProg = "C:/Program Files (x86)/arduino-1.6.4/arduino.exe"
inputdir = "C:/Users/MSajikumar/Documents/Linduino/LTSketchbook/Part Number/3000"

for root, subfolders, files in os.walk(inputdir): #Getting a list of the full paths of files
    for filenames in files:
        inofiles = (os.path.join(root, filenames)) #Joining root and filenames
        if inofiles.endswith('.ino'):
            print inofiles
            proc = subprocess.call([arduinoProg, inofiles, "--verify"])
            print proc
            
            
#            for proc in subprocess:
#                proc.wait()
