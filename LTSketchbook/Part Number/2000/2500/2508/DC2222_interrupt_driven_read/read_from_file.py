# -*- coding: utf-8 -*-
"""
Created on Thu Mar 16 15:04:50 2017

@author: MSajikumar
"""

# read each line of file
# into string of 10 characters
# group them in 2s and convert to hex numbers.

import matplotlib.pyplot as plt

def code_to_voltage(code, vref):
    voltage = code * 1.0 / 2147483648 * vref
    return voltage

voltage_list = []
f = open('teraterm.txt', 'r')
for line in f:
    data_string = line
    data_string = '0x'+ data_string
    print '\nData received: %s' % data_string,
    
    data = int(data_string, 0)
    code = data >> 8
    last_byte = data & 0xFF
    voltage = code_to_voltage(code, 5)
    print 'Voltage calculated: %f V' % voltage
    voltage_list.append(voltage)
    
    if (last_byte == 0x85): 
        DF = 256 
    elif (last_byte == 0xA5): 
        DF = 1024
    elif (last_byte == 0xC5): 
        DF = 4096
    elif (last_byte == 0xE5): 
        DF = 16384
    else:
        DF = 0
    print 'DF : %d' % DF

plt.plot(voltage_list)
plt.axis([0, 500, -5, 5])
plt.xlabel('Samples')
plt.ylabe('Voltage (V)')