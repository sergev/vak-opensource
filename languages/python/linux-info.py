#!/usr/bin/env python3
#
# Print information about the current computer.
#
import os
import re
from socket import gethostname
from platform import freedesktop_os_release

def gethost():
    '''Extract host name'''
    try:
        return gethostname()
    except:
        return "NA"

def processor():
    '''Extract first processor name'''
    try:
        with open("/proc/cpuinfo", 'r') as the_file:
            info = the_file.read().strip()
        for line in info.split("\n"):
            if "model name" in line:
                return re.sub(".*model name.*:", "", line, 1).strip()
    except:
        return "N/A"

def manufacturer():
    '''Extract manufacturer'''
    try:
        with open("/sys/class/dmi/id/sys_vendor", 'r') as the_file:
            info = the_file.read().strip()
        return info
    except:
        return "N/A"

def model():
    '''Extract model'''
    try:
        with open("/sys/class/dmi/id/product_name", 'r') as the_file:
            info = the_file.read().strip()
        return info
    except:
        return "N/A"

def distribution():
    '''Format distribution'''
    try:
        return freedesktop_os_release()['PRETTY_NAME']
    except:
        return "N/A"

print("        Host:", gethost())
print("Manufacturer:", manufacturer())
print("       Model:", model())
print("   Processor:", processor())
print("Distribution:", distribution())
