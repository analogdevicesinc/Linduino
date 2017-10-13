# -*- coding: utf-8 -*-
"""
Created on Thu Jan 07 15:14:54 2016

@author: MSajikumar
"""

# This script is used to run through all Linduino files, and find a string

import re
import os


pattern = re.compile('i2c_poll')
inputdir = "C:/Users/MSajikumar/Documents/Linduino/LTSketchbook/libraries"

for root, subfolders, files in os.walk(inputdir): #Getting a list of the full paths of files
    for filenames in files:
        path = (os.path.join(root, filenames)) #Joining root and filenames
        strg = open(path).read() #Opening the files for reading only
        if re.search(pattern, strg):#If we find the pattern ....
            print path
            
            
#            for proc in subprocess:
#                proc.wait()
