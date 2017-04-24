import socket
import sys
import redis
import json


def money_check(s):
	try:
    		n = int(s)
	except ValueError:
    		return -1
	else:
		return n


r = redis.Redis(host='localhost',port=6379,db=0)

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

server_address = ('localhost',8888)

print >>sys.stderr,'starting up on %s port %s'%server_address
sock.bind(server_address)
sock.listen(1)

while 1:
	print >> sys.stderr,'waiting for a connection'
	connection , client_address = sock.accept()
	try:
		print >> sys.stderr,'connection from',client_address
		while 1:
			data = connection.recv(1024)
			if data == 'end':
				connection.sendall(json.dumps(r.hgetall('mybank')))
				print >> sys.stderr,'no more data from',client_address
				break
			elif data:
				print >> sys.stderr,'received "%s"'%data
				args = data.split()
				if args[0]=='init':
					account = args[1].lower()
					money = money_check(args[2])
					data = "error"
					succ = 0
					if money >= 0 and r.hexists('mybank',account) == False :
						r.hset('mybank',account,money)
						succ = 1
						data = "%s,%s,%d"%(account,r.hget('mybank',account),succ)
				elif args[0]=='save':
					account = args[1].lower()
					money = money_check(args[2])
					data = "error"
                                        succ = 0
                                        if money >= 0 and r.hexists('mybank',account):
                                        	r.hincrby('mybank',account,money)
                                                succ = 1
                                                data = "%s,%s,%d"%(account,r.hget('mybank',account),succ)

				elif args[0]=='load':
					account = args[1].lower()
					money = money_check(args[2])
					data = "error"
                                        succ = 0
					if money >= 0 and r.hexists('mybank',account) and int(r.hget('mybank',account)) >= money :
                                        	r.hincrby('mybank',account,money*-1)
                                                succ = 1
                                                data = "%s,%s,%d"%(account,r.hget('mybank',account),succ)
				elif args[0]=='remit':
					account_A = args[1].lower()
					account_B = args[2].lower()
					money = money_check(args[3])
					data = "error"
                                        succ = 0
					if money >= 0 and r.hexists('mybank',account_A) and r.hexists('mybank',account_B) and int(r.hget('mybank',account_A)) >= money :
						succ = 1
						r.hincrby('mybank',account_A,money*-1)
						r.hincrby('mybank',account_B,money)
						data = "%s,%s"%(account_A,r.hget('mybank',account_A))
						data += ",%s,%s,%d"%(account_B,r.hget('mybank',account_B),succ)
				else:
					data = "error"
				print >> sys.stderr,'sending "%s" back to the client'%(data)
				connection.sendall(data)
			else:
				print >> sys.stderr,'no more data from',client_address
				break;
	finally:
		connection.close()


