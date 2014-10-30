import sys
import msvcrt
import os
import time

print sys.argv

handle = int(sys.argv[1],16)
print "Handle: 0x%x" % handle
desc = msvcrt.open_osfhandle(handle, os.O_APPEND)
print desc
resultFile = os.fdopen(desc, "w")

print "HeyHeyHey"
print "Reading line:"
l = raw_input()
print "Input was:",l

resultFile.write("Response:\n")
resultFile.flush()

time.sleep(10)
print "Exiting..."
