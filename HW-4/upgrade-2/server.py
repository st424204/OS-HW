import socket,sys,redis,json,select
from redlock import RedLock

def money_check(s):
	try:
		n = int(s)
	except ValueError:
		return -1
	else:
		return n

r = redis.Redis(host='node1',port=6379,db=0)
lock = RedLock("lock",connection_details=[
	{'host': 'node1', 'port': 6379, 'db': 0},
])

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_address = ('node1',8888)
sock.bind(server_address)
sock.listen(1)

while 1:
	connection , client_address = sock.accept()
	over = 0
	try:
		while 1:
			data = connection.recv(1024)
			if data and over == 0:
				data_list = data.split('\n')
				for data in data_list:
					if data == 'end':
						over = 1
						r.flushdb()
						break
					if len(data) == 0:
						break
					lock.acquire()
					args = data.split()
					data = "error"
					if args[0]=='init':
						account = args[1].lower()
						money = money_check(args[2])
						if money >= 0 and r.exists(account) == False :
							r.set(account,money)
							data = dict()
							data[account] = r.get(account)
							data = json.dumps(data);
					elif args[0]=='save':
						account = args[1].lower()
						money = money_check(args[2])
						if money >= 0 and r.exists(account):
							r.incr(account,money)
							data = dict()
							data[account] = r.get(account)
							data = json.dumps(data);
					elif args[0]=='load':
						account = args[1].lower()
						money = money_check(args[2])
						if money >= 0 and r.exists(account) and int(r.get(account)) >= money :
							r.decr(account,money)
							data = dict()
							data[account] = r.get(account)
							data = json.dumps(data);
					elif args[0]=='remit':
						account_A = args[1].lower()
						account_B = args[2].lower()
						money = money_check(args[3])
						if money >= 0 and r.exists(account_A) and r.exists(account_B) and int(r.get(account_A)) >= money and account_A!=account_B:
							r.decr(account_A,money)
							r.incr(account_B,money)
							data = dict()
							data[account_A] = r.get(account_A)
							data[account_B] = r.get(account_B)
							data = json.dumps(data);

					connection.sendall(data+"\n")
					lock.release()
			else:
				break;
	finally:
		connection.close()


