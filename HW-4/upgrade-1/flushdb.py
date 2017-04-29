import redis
r = redis.Redis(host='node1',port=6379,db=0)
r.flushdb()
