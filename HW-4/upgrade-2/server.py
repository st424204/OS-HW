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
						account_lock = RedLock("distributed_lock",resource=account,connection_details=[
							{'host': 'node1', 'port': 6379, 'db': 0},
						])
						account_lock.acquire()
						r.set(account,money)
						account_lock.release()
						data = "ok"
				elif args[0]=='save':
					account = args[1].lower()
					money = money_check(args[2])
					if money >= 0 and r.exists(account):
						account_lock = RedLock("distributed_lock",resource=account,connection_details=[
							{'host': 'node1', 'port': 6379, 'db': 0},
						]))
						account_lock.acquire()
						r.incr(account,money)
						account_lock.release()
						data = "ok"
				elif args[0]=='load':
					account = args[1].lower()
					money = money_check(args[2])
					if money >= 0 and r.exists(account) and int(r.get(account)) >= money :
						account_lock = RedLock("distributed_lock",resource=account,connection_details=[
							{'host': 'node1', 'port': 6379, 'db': 0},
						]))
						account_lock.acquire()
						r.decr(account,money)
						account_lock.release()
						data = "ok"
				elif args[0]=='remit':
					account_A = args[1].lower()
					account_B = args[2].lower()
					money = money_check(args[3])
					if money >= 0 and r.exists(account_A) and r.exists(account_B) and int(r.get(account_A)) >= money and account_A!=account_B:
						succ = 1
						account_A_lock = RedLock("distributed_lock",resource=account_A,connection_details=[
							{'host': 'node1', 'port': 6379, 'db': 0},
						]))
						account_B_lock = RedLock("distributed_lock",resource=account_B,connection_details=[
							{'host': 'node1', 'port': 6379, 'db': 0},
						]))
						account_A_lock.acquire()
						account_B_lock.acquire()
						r.decr(account_A,money)
						r.incr(account_B,money)
						account_A_lock.release()
						account_B_lock.release()
						data = "ok"
				connection.sendall(data)
			else:
				break;
	finally:
		connection.close()


