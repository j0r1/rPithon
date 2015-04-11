import sys, os, json, platform, traceback

# We'll use a helper class to create a new object so that we can attach several
# properties to this single object, and can avoid cluttering the global 
# namespace
class TmpHelperClass(object):

    def __init__(self):
        # This script used one argument:
        #  - on Windows this is the handle of the pipe to which results should be 
        #    written (in hexadecimal)
        #  - on Other platforms this is a file descriptor (in decimal)
        if platform.system() == "Windows":
            import msvcrt
            self.resultChannel = os.fdopen(msvcrt.open_osfhandle(int(sys.argv[1],16),os.O_APPEND), "wb")
        else:
            self.resultChannel = os.fdopen(int(sys.argv[1]), "wb")

    @staticmethod
    def getExceptionDescription(ex):
        #return str(ex)
        _, _, ex_traceback = sys.exc_info()
        if ex_traceback is None:
            ex_traceback = ex.__traceback__
        tb_lines = traceback.format_exception(ex.__class__, ex, ex_traceback)
        return "%s" % ''.join(tb_lines)

    if sys.version_info[0] == 2:
        def write(self, s):
            self.resultChannel.write(s)
            self.resultChannel.flush()

        def writeData(self, status, data = None):
            if data is not None:
                firstLine = "%d,%d\n" % (status, len(data))
                self.resultChannel.write(firstLine)
                self.resultChannel.write(data)
            else:
                firstLine = "%d,0\n" % status
                self.resultChannel.write(firstLine)
        
            self.resultChannel.flush()

        def read(self, amount = -1):
            return sys.stdin.read(amount)

        def readline(self):
            return sys.stdin.readline()

    else: # assume it's python 3
        def write(self, s):
            self.resultChannel.write(str.encode(s))
            self.resultChannel.flush()

        def writeData(self, status, data = None):
            if data is not None:
                byteData = str.encode(data)
                firstLine = "%d,%d\n" % (status, len(byteData))
                self.resultChannel.write(str.encode(firstLine))
                self.resultChannel.write(byteData)
            else:
                firstLine = "%d,0\n" % status
                self.resultChannel.write(str.encode(firstLine))
        
            self.resultChannel.flush()

        def read(self, amount = -1):
            return sys.stdin.buffer.read(amount)

        def readline(self):
            return bytes.decode(sys.stdin.buffer.readline())


reallyReallyLongAndUnnecessaryPrefix = TmpHelperClass()
del TmpHelperClass

# To be sure that both python and this script was started successfully,
# we send back the string which the C++ part is expecting
reallyReallyLongAndUnnecessaryPrefix.write("RPITHON\n")

while True:

    # From stdin, we're expecting a line with two values, separated by a
    # comma. The first value is the command number, the second the length
    # of the data that follows.

    reallyReallyLongAndUnnecessaryPrefix.l = reallyReallyLongAndUnnecessaryPrefix.readline()
    if len(reallyReallyLongAndUnnecessaryPrefix.l) == 0:
        #print "No more data, exiting"
        break

    reallyReallyLongAndUnnecessaryPrefix.parts = reallyReallyLongAndUnnecessaryPrefix.l.strip().split(",")
    reallyReallyLongAndUnnecessaryPrefix.command = int(reallyReallyLongAndUnnecessaryPrefix.parts[0])
    reallyReallyLongAndUnnecessaryPrefix.argLength = int(reallyReallyLongAndUnnecessaryPrefix.parts[1])

    # Read the data from stdin
    reallyReallyLongAndUnnecessaryPrefix.argData = reallyReallyLongAndUnnecessaryPrefix.read(reallyReallyLongAndUnnecessaryPrefix.argLength)
    if len(reallyReallyLongAndUnnecessaryPrefix.argData) != reallyReallyLongAndUnnecessaryPrefix.argLength:
        #print "Read insufficient data"
       break

    #print reallyReallyLongAndUnnecessaryPrefix.argData
    try:
        if reallyReallyLongAndUnnecessaryPrefix.command == 1: # CMD_EXEC, execute a command
            exec(reallyReallyLongAndUnnecessaryPrefix.argData)
            reallyReallyLongAndUnnecessaryPrefix.writeData(0)

        elif reallyReallyLongAndUnnecessaryPrefix.command == 2: # CMD_GETVAR, evaluate a variable
            # The front end expects json data in a particular way
            reallyReallyLongAndUnnecessaryPrefix.data = json.dumps([eval(reallyReallyLongAndUnnecessaryPrefix.argData)])
            reallyReallyLongAndUnnecessaryPrefix.writeData(0, reallyReallyLongAndUnnecessaryPrefix.data)

    except Exception as reallyReallyLongAndUnnecessaryPrefix_e:
        reallyReallyLongAndUnnecessaryPrefix.errStr = reallyReallyLongAndUnnecessaryPrefix.getExceptionDescription(reallyReallyLongAndUnnecessaryPrefix_e)
        reallyReallyLongAndUnnecessaryPrefix.writeData(1, reallyReallyLongAndUnnecessaryPrefix.errStr)
        
sys.exit(0)

