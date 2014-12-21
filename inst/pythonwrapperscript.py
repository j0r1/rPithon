import sys, os, json, platform

# We'll use a helper class to create a new object so that we can attach several
# properties to this single object, and can avoid cluttering the global 
# namespace
class TmpHelperClass(object): pass
reallyReallyLongAndUnnecessaryPrefix = TmpHelperClass()
del TmpHelperClass

# This script used one argument:
#  - on Windows this is the handle of the pipe to which results should be 
#    written (in hexadecimal)
#  - on Other platforms this is a file descriptor (in decimal)
if platform.system() == "Windows":
    import msvcrt
    reallyReallyLongAndUnnecessaryPrefix.resultChannel = os.fdopen(msvcrt.open_osfhandle(int(sys.argv[1],16),os.O_APPEND), "w")
else:
    reallyReallyLongAndUnnecessaryPrefix.resultChannel = os.fdopen(int(sys.argv[1]), "w")

# To be sure that both python and this script was started successfully,
# we send back the string which the C++ part is expecting
reallyReallyLongAndUnnecessaryPrefix.resultChannel.write("RPITHON\n")
reallyReallyLongAndUnnecessaryPrefix.resultChannel.flush()

while True:

    # From stdin, we're expecting a line with two values, separated by a
    # comma. The first value is the command number, the second the length
    # of the data that follows.

    reallyReallyLongAndUnnecessaryPrefix.l = sys.stdin.readline()
    if len(reallyReallyLongAndUnnecessaryPrefix.l) == 0:
        #print "No more data, exiting"
        break

    reallyReallyLongAndUnnecessaryPrefix.parts = reallyReallyLongAndUnnecessaryPrefix.l.strip().split(",")
    reallyReallyLongAndUnnecessaryPrefix.command = int(reallyReallyLongAndUnnecessaryPrefix.parts[0])
    reallyReallyLongAndUnnecessaryPrefix.argLength = int(reallyReallyLongAndUnnecessaryPrefix.parts[1])

    # Read the data from stdin
    reallyReallyLongAndUnnecessaryPrefix.argData = sys.stdin.read(reallyReallyLongAndUnnecessaryPrefix.argLength)
    if len(reallyReallyLongAndUnnecessaryPrefix.argData) != reallyReallyLongAndUnnecessaryPrefix.argLength:
        #print "Read insufficient data"
       break

    #print reallyReallyLongAndUnnecessaryPrefix.argData
    if reallyReallyLongAndUnnecessaryPrefix.command == 1: # CMD_EXEC, execute a command
        try:
            exec(reallyReallyLongAndUnnecessaryPrefix.argData)
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write("0,0\n")
        except Exception as reallyReallyLongAndUnnecessaryPrefix_e:
            reallyReallyLongAndUnnecessaryPrefix.errStr = str(reallyReallyLongAndUnnecessaryPrefix_e)
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write("1,%d\n" % len(reallyReallyLongAndUnnecessaryPrefix.errStr))
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write(reallyReallyLongAndUnnecessaryPrefix.errStr)
    
        reallyReallyLongAndUnnecessaryPrefix.resultChannel.flush()

    elif reallyReallyLongAndUnnecessaryPrefix.command == 2: # CMD_GETVAR, evaluate a variable
        try:
            # The front end expects json data in a particular way
            reallyReallyLongAndUnnecessaryPrefix.data = json.dumps([eval(reallyReallyLongAndUnnecessaryPrefix.argData)])

            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write("0,%d\n" % len(reallyReallyLongAndUnnecessaryPrefix.data))
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write(reallyReallyLongAndUnnecessaryPrefix.data)
        except Exception as reallyReallyLongAndUnnecessaryPrefix_e:
            reallyReallyLongAndUnnecessaryPrefix.errStr = str(reallyReallyLongAndUnnecessaryPrefix_e)
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write("1,%d\n" % len(reallyReallyLongAndUnnecessaryPrefix.errStr))
            reallyReallyLongAndUnnecessaryPrefix.resultChannel.write(reallyReallyLongAndUnnecessaryPrefix.errStr)
        
        reallyReallyLongAndUnnecessaryPrefix.resultChannel.flush()

sys.exit(0)

