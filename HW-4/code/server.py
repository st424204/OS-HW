import socket,redis,json
from redlock import RedLock
def report_json(accounts):
	report = {}
	for account in accounts:
		report[account] = int(r.get(account))
	return json.dumps(report)
def init_process(command):
	command[1] = command[1].lower()
	if r.exists(command[1]) == True :
		return 'error'
	money = int(command[2])
	if 	money<0:
		return 'error'
	r.set(command[1],money)
	return report_json([command[1],])
def save_process(command):
	command[1] = command[1].lower()
	if r.exists(command[1]) == False :
		return 'error'
	money = int(command[2])
	if 	money<0:
		return 'error'
	r.incr(command[1],money)
	return report_json([command[1],])
def load_process(command):
	command[1] = command[1].lower()
	if r.exists(command[1]) == False :
		return 'error'
	money = int(command[2])
	total = int(r.get(command[1]))
	if 	money<0 or money>total:
		return 'error'
	r.decr(command[1],money)
	return report_json([command[1],])
def remit_process(command):
	command[1] = command[1].lower()
	command[2] = command[2].lower()
	if command[1] == command[2] or r.exists(command[1])== False  or r.exists(command[2]) == False :
		return 'error'
	money = int(command[3])
	total = int(r.get(command[1]))
	if 	money<0 or money>total:
		return 'error'
	r.decr(command[1],money)
	r.incr(command[2],money)
	return report_json([command[1],command[2]])
def end_process():
	r.flushdb()
	return 'over'
def process_command(command):
	command = command.decode('utf-8').split(' ')
	type = 	command[0]
	if type == 'init':
		return init_process(command)
	elif type == 'save':
		return save_process(command)
	elif type == 'load':
		return load_process(command)
	elif type == 'remit':
		return remit_process(command)
	else:
		return end_process()

		
		

lock = RedLock("distributed_lock", 
connection_details=[{'host': '192.168.111.128', 'port': 6379, 'db': 0},]
)
r = redis.Redis(host='192.168.111.128',port=6379,db=0)
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind(('0.0.0.0',8888))
s.listen(1)
while(1):
	c,ip = s.accept()
	print(ip)
	while 1:	
		command = c.recv(1024);
		if command:
			lock.acquire()
			ret = process_command(command).encode('utf-8')
			c.sendall(ret)
			lock.release()
		else:
			break;
