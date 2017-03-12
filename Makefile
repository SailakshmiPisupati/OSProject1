server:
	gcc server.c -o server.out -lpthread
client:
	gcc client.c -o client.out
runserver:
	./server.out 8023
runclient:
	./client.out 127.0.0.1 8023
releaseport:
	sudo fuser -k 8023/tcp