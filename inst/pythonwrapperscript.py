import sys, os, json

reallyReallyLongAndUnnecessaryPrefix_resultChannel = os.fdopen(int(sys.argv[1]), "w")
reallyReallyLongAndUnnecessaryPrefix_resultChannel.write("RPYTHON2\n")
reallyReallyLongAndUnnecessaryPrefix_resultChannel.flush()

while True:

    reallyReallyLongAndUnnecessaryPrefix_l = sys.stdin.readline()
    if len(reallyReallyLongAndUnnecessaryPrefix_l) == 0:
        #print "No more data, exiting"
        break

    reallyReallyLongAndUnnecessaryPrefix_parts = reallyReallyLongAndUnnecessaryPrefix_l.strip().split(",")
    reallyReallyLongAndUnnecessaryPrefix_command = int(reallyReallyLongAndUnnecessaryPrefix_parts[0])
    reallyReallyLongAndUnnecessaryPrefix_argLength = int(reallyReallyLongAndUnnecessaryPrefix_parts[1])

    reallyReallyLongAndUnnecessaryPrefix_argData = sys.stdin.read(reallyReallyLongAndUnnecessaryPrefix_argLength)
    if len(reallyReallyLongAndUnnecessaryPrefix_argData) != reallyReallyLongAndUnnecessaryPrefix_argLength:
        #print "Read insufficient data"
        break

    if reallyReallyLongAndUnnecessaryPrefix_command == 1: # CMD_EXEC
        try:
            exec(reallyReallyLongAndUnnecessaryPrefix_argData)
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write("0,0\n")
        except Exception as reallyReallyLongAndUnnecessaryPrefix_e:
            reallyReallyLongAndUnnecessaryPrefix_errStr = str(reallyReallyLongAndUnnecessaryPrefix_e)
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write("1,%d\n" % len(reallyReallyLongAndUnnecessaryPrefix_errStr))
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write(reallyReallyLongAndUnnecessaryPrefix_errStr)
        
        reallyReallyLongAndUnnecessaryPrefix_resultChannel.flush()

    elif reallyReallyLongAndUnnecessaryPrefix_command == 2: # CMD_GETVAR
        try:
            data = json.dumps([eval(reallyReallyLongAndUnnecessaryPrefix_argData)])

            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write("0,%d\n" % len(data))
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write(data)
        except Exception as reallyReallyLongAndUnnecessaryPrefix_e:
            reallyReallyLongAndUnnecessaryPrefix_errStr = str(reallyReallyLongAndUnnecessaryPrefix_e)
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write("1,%d\n" % len(reallyReallyLongAndUnnecessaryPrefix_errStr))
            reallyReallyLongAndUnnecessaryPrefix_resultChannel.write(reallyReallyLongAndUnnecessaryPrefix_errStr)
        
        reallyReallyLongAndUnnecessaryPrefix_resultChannel.flush()

sys.exit(0)

