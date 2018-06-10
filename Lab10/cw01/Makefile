all:
	gcc -Wall -g -lpthread -o server server.c
	gcc -Wall -g -o client client.c
clean:
	rm -f client server socket
server:
	./server /home/woolfy/Sysopy/Lab10/cw01/socket 8083
remote:
	./client klient_net net 127.0.0.1 8083
local:
	./client kilent_local unix /home/woolfy/Sysopy/Lab10/cw01/socket 8083
