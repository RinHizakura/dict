#!/usr/bin/python

import os
import time

os.chdir("/home/rin/Github/system-software/dict/")
## Open file


for i in range(10):
    fp = open('input.txt', "r")
    t = 0
    line = fp.readline()
    while line:
        s = line.split(',')[0]
        
        start = time.time()
        os.system("./test_common --bench CPY s " + s + " > /dev/null") 
        end = time.time()
        t += end - start

        line = fp.readline()
    fp.close()
    print(i,",",t)
