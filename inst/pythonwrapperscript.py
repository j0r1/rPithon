import sys

print "Python process:", sys.argv
sys.exit(0)

while True:
    l = sys.stdin.readline()
    if len(l) == 0:
        break

    try:
        exec(l)
    except Exception as e:
        print "Error:", e

