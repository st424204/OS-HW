import socket,sys,redis,json,select
from redlock import Redlock

def money_check(s):
	try:
		n = int(s)
	except ValueError:
		return -1
	else:
		return n

r = redis.Redis(host='node1',port=6379,db=0)
dlm = Redlock([{"host": "node1", "port": 6379, "db": 0}, ])

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_address = ('node1',8888)
sock.bind(server_address)
sock.listen(1)

while 1:
	connection , client_address = sock.accept()
	try:
		while 1:
			data = connection.recv(1024)
			if data == 'end':
				keys = r.keys('*')
				data = dict()
				for key in keys:
					data[key] = r.get(key)
				data = json.dumps(data);
				connection.sendall('end')
				connection.sendall(str(len(data)))
				connection.sendall(data)
				break
			elif data:
				args = data.split()
				data = "error"
				if args[0]=='init':
					account = args[1].lower()
					money = money_check(args[2])
					if money >= 0 and r.exists(account) == False :
						account_lock = dlm.lock(account,1000)
						while account_lock == False:
							account_lock = dlm.lock(account,1000)
						r.set(account,money)
						dlm.unlock(account_lock)
						data = "ok"
				elif args[0]=='save':
					account = args[1].lower()
					money = money_check(args[2])
					if money >= 0 and r.exists(account):
						account_lock = dlm.lock(account,1000)
						while account_lock == False:
							account_lock = dlm.lock(account,1000)
						r.incr(account,money)
						dlm.unlock(account_lock)
						data = "ok"
				elif args[0]=='load':
					account = args[1].lower()
					money = money_check(args[2])
					if money >= 0 and r.exists(account) and int(r.get(account)) >= money :
						account_lock = dlm.lock(account,1000)
						while account_lock == False:
							account_lock = dlm.lock(account,1000)
                        			r.decr(account,money)
						dlm.unlock(account_lock)
                        			data = "ok"
				elif args[0]=='remit':
					account_A = args[1].lower()
					account_B = args[2].lower()
					money = money_check(args[3])
					if money >= 0 and r.exists(account_A) and r.exists(account_B) and int(r.get(account_A)) >= money and account_A!=account_B:
						succ = 1
						account_A_lock = dlm.lock(account_A,1000)
						while account_A_lock == False:
							account_A_lock = dlm.lock(account_A,1000)
							
						account_B_lock = dlm.lock(account_B,1000)
						while account_B_lock == False:
							account_B_lock = dlm.lock(account_B,1000)
						r.decr(account_A,money)
						r.incr(account_B,money)
						dlm.unlock(account_A_lock)
						dlm.unlock(account_B_lock)
						data = "ok"
				connection.sendall(data)
			else:
				break;
	finally:
		connection.close()


