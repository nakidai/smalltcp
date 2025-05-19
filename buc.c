#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

char buf[512]; progress, all;
atoi(), err(), exit(), read(), write();

#define	readbuf(sock)	((((progress = read((sock), buf, sizeof(buf))) == -1) && err(1, "read()") || !progress && exit(0)), progress)
#define	sendbuf(sock, size)	(write(sock, buf, size) == -1 && err(1, "write()"))

main(argc, argv) char **argv; {
	struct sockaddr_in addr = {AF_INET, htons(argc > 2 ? atoi(argv[1]) : 8604), inet_addr(argc > 1 ? argv[1] : "127.0.0.1")};
	struct pollfd fds[2] = {{0, POLLIN}, {0, POLLIN}};
	for (	(fds[1].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 && err(1, "socket()"),
	     	connect(fds[1].fd, (void *)&addr, sizeof(addr)) == -1 && err(1, "connect()");
	;)   	poll(fds, 2, -1) == -1 && err(1, "poll()"),
	     	fds[0].revents & POLLIN && sendbuf(fds[1].fd, readbuf(0)),
	     	fds[1].revents & POLLIN && sendbuf(1, readbuf(fds[1].fd));
}
