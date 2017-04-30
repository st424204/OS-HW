import socket
import sys
import operator
import json
import threading,select,time

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
	if c == '':
		c = "0"
	return (b,int(c))


sockets = [0,0,0]
for i in range(3):
	sockets[i] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sockets[i].connect(('node%d'%(i+1),8888))
	sockets[i].setblocking(0)

def sendcommand(messages,sockets,delay):
	for i in range(len(messages)):
		time.sleep(delay)
		sockets[i%3].sendall(messages[i])


messages = open(sys.argv[1],'r').read()
messages = messages.split('\n')

threading.Thread(target=sendcommand,args=(messages,sockets,.01)).start()

total = 0
succ = 0
output = ""
while output == "" :
	readable, writable, exceptional = select.select(sockets,[], [])
	for s in readable:
		data = s.recv(1024)
		if data == 'end' :
			data = s.recv(1024)
			data = s.recv(int(data))
			output = json.loads(data)
		else: 
			if data != 'error' :
				succ = succ + 1
			total = total + 1

for i in range(3):
	sockets[i].close()
output = sorted(output.items(), key=cmp,reverse=False)
for account,money in output:
	print '%s : %s'%(account,money)
print '\nsuccess rate : (%d/%d)'%(succ,total)
