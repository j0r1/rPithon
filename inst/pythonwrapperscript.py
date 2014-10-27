import sys
import os
import json

print "Python process:", sys.argv
resultDesc = int(sys.argv[1])
print "Result descriptor:", resultDesc

resultChannel = os.fdopen(resultDesc, "w")

resultChannel.write("RPYTHON2\n")
resultChannel.flush()

while True:
    l = sys.stdin.readline()
    if len(l) == 0:
        break

    parts = l.split(",")
    print parts

    command = int(parts[0])
    argLength = int(parts[1])

    argData = sys.stdin.read(argLength)
    print argData

    if command == 1: # CMD_EXEC
        try:
            exec(l)
            resultChannel.write("0,0\n")
            resultChannel.flush()
        except Exception as e:
            errStr = str(e)
            resultChannel.write("1,%d\n" % len(errStr))
            resultChannel.write(errStr)
            resultChannel.flush()

    elif command == 2: # CMD_GETVAR
        try:
            v = eval(argData)
            resultChannel.write("0,%d\n" % len(v))
            resultChannel.write(v)
            resultChannel.flush()
        except Exception as e:
            errStr = str(e)
            resultChannel.write("1,%d\n" % len(errStr))
            resultChannel.write(errStr)
            resultChannel.flush()

sys.exit(0)
