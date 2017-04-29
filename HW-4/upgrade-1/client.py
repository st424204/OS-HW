#
import socket
import sys
import operator
import json
import re
r = re.compile("([a-zA-Z]*)([0-9]*)")

def cmp(x):
        a = r.match(x[0])
        try:
                b = a.group(1)
        except IndexError:
                b = ""
        try:
                c = a.group(2)
        except IndexError:
                c = "0" 
        if c=='':
                c="0"
        return (b,int(c))


sockets = [socket.socket(socket.AF_INET, socket.SOCK_STREAM) for i in range(3)]
for i in range(3):
     sockets[i].connect(('node%d'%(i+1),8888))
output = dict()
input = open(sys.argv[1],'r').read()
input = input.split('\n')
total = 0
succ = 0
count = 0
try:
	for message in input:
#		print >> sys.stderr,'sending "%s"'%message
		sock = sockets[count%3]
		sock.sendall(message)
		data = sock.recv(1024)
		if message == 'end' :
			output = json.loads(data)
                        break

		elif data == 'error':
			total = total + 1
		else:
			succ = succ + 1
			total = total + 1

finally:
#	print >> sys.stderr,'closing socket'
	output = sorted(output.items(), key=cmp,reverse=True)
#	print output
	for a,b in output:
		print '%s : %s'%(a,b)
	print '\nsuccess rate : (%d/%d)'%(succ,total)
	for i in range(3):
		sockets[i].close()
