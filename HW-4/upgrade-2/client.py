import socket,sys,operator,json,threading,select,time

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
	for i in range(len(messages)-1):
		time.sleep(delay)
		sockets[i%3].sendall(messages[i]+"\n")
	for i in range(3):
		sockets[i].sendall('end')

messages = open(sys.argv[1],'r').read()
messages = messages.split('\n')

threading.Thread(target=sendcommand,args=(messages,sockets,.01)).start()

total = 0
succ = 0
output = dict()
while sockets :
	readable, writable, exceptional = select.select(sockets, [], [])
	for s in readable:
		data = s.recv(1024)
		if data :
			data_list = data.split('\n')
            for data in data_list:
				if data and data != 'error' :
					data = json.loads(data)
					output.update(data)
					succ = succ + 1
					total = total + 1
				elif data:
					total = total + 1
		else:
			s.close()
			sockets.remove(s)

output = sorted(output.items(), key=cmp,reverse=False)
for account,money in output:
	print '%s : %s'%(account,money)
print '\nsuccess rate : (%d/%d)'%(succ,total)
