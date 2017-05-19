import _thread
import socket,sys,json,time

def runsend(delay):
	global command,ss
	index = 0
	for c in command:
		s = ss[index]
		time.sleep(delay)
		s.sendall(c.encode('utf-8'))
		index = (index+1)%3

ss = [socket.socket(socket.AF_INET,socket.SOCK_STREAM),
socket.socket(socket.AF_INET,socket.SOCK_STREAM),
socket.socket(socket.AF_INET,socket.SOCK_STREAM)]
ss[0].connect(('192.168.111.128',8888))
ss[1].connect(('192.168.111.129',8888))
ss[2].connect(('192.168.111.130',8888))
command = open(sys.argv[1],'r').readlines()
total = len(command)-1
index = 0
error = 0
report = {}
_thread.start_new_thread(runsend,(float(sys.argv[2]), ) )
for c in command:
	s = ss[index]
	recv = s.recv(1024).decode('utf-8')
	if recv == 'error':
		error = error+1
	elif recv != 'over':
		report.update(json.loads(recv))
	index = (index+1)%3
	
ss[0].close()
ss[1].close()
ss[2].close()

report = sorted(report.items(),key=lambda x:x[0],reverse=False)

for key,value in report:
	print('%s : %d'%(key,value))
print('success rate : (%d/%d)'%(total-error,total))
