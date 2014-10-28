import sys, select, os, json

#print "Python process:", sys.argv
resultDesc = int(sys.argv[1])
#print "Result descriptor:", resultDesc

resultChannel = os.fdopen(resultDesc, "w")

resultChannel.write("RPYTHON2\n")
resultChannel.flush()

count = 0

while True:

    count += 1
    print "Reading line", count

    l = sys.stdin.readline()
    if len(l) == 0:
        print "No more data, exiting"
        break

    parts = l.strip().split(",")
    #print parts

    command = int(parts[0])
    argLength = int(parts[1])

    print "Reading %d bytes" % argLength
    argData = sys.stdin.read(argLength)
    if len(argData) != argLength:
        print "Read insufficient data"
        break

    #print argData

    if command == 1: # CMD_EXEC
        #print "CMD_EXEC"
        try:
            #print "Trying exec(%s)" % argData
            exec(argData)
            #print "Writing result"
            resultChannel.write("0,0\n")
            resultChannel.flush()
        except Exception as e:
            #print "Exception", e
            errStr = str(e)
            resultChannel.write("1,%d\n" % len(errStr))
            resultChannel.write(errStr)
            resultChannel.flush()

    elif command == 2: # CMD_GETVAR
        #print "CMD_GETVAR"
        try:
            #print "Getting variable"
            v = eval(argData)
            #print "Writing result:", v

            data = json.dumps([v])

            resultChannel.write("0,%d\n" % len(data))
            resultChannel.write(data)
            resultChannel.flush()
        except Exception as e:
            #print "Exception", e
            errStr = str(e)
            resultChannel.write("1,%d\n" % len(errStr))
            resultChannel.write(errStr)
            resultChannel.flush()

sys.exit(0)
