#
import socket
import sys
import operator
import json

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_address = ('localhost',8888)
#print >> sys.stderr,'connecting to %s port %s'%server_address
sock.connect(server_address)
output = dict()
input = open(sys.argv[1],'r').read()
input = input.split('\n')
total = 0
succ = 0
try:
	for message in input:
#		print >> sys.stderr,'sending "%s"'%message
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
	output = sorted(output.items(), key=lambda x: int(x[1]),reverse=True)
#	print output
	for a,b in output:
		print '%s : %s'%(a,b)
	print '\nsuccess rate : (%d/%d)'%(succ,total)
	sock.close()
