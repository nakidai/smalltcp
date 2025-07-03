#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

#define SOCKETS_MAX 128
#define BUFFER_SIZE 512

#define lengthof(X) (sizeof(X) / sizeof(*(X)))

main(argc, argv) char **argv;
{
	struct sockaddr_in addr = {AF_INET, htons(8604), htonl(INADDR_ANY)};
	struct pollfd ppeers[SOCKETS_MAX] = {0};
	int peers[SOCKETS_MAX] = {0}, client, i, j, written, readed;
	char buf[BUFFER_SIZE] = {0};

	for (
		argc > 1 && (addr.sin_port = htons(atoi(argv[1]))),
		argc > 2 && inet_pton(AF_INET, argv[1], &addr.sin_addr) == -1 && err(1, "inet_pton()"),
		(peers[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1 && err(1, "socket()"),
		setsockopt(peers[0], SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1 && err(1, "setsockopt()"),
		bind(peers[0], (void *)&addr, sizeof(addr)) == -1 && err(1, "bind()"),
		listen(peers[0], 16) == -1 && err(1, "listen()"),
		signal(SIGPIPE, SIG_IGN);
	;)
	{
		for (written = i = 0; i < lengthof(peers) ? (peers[i] && ((ppeers[written++] = (struct pollfd){peers[i], POLLIN}), 0), 1) : 0; ++i)
			if (i + 1 == lengthof(peers))
				for (
				client = 0, j = 1, poll(ppeers, written, -1) == -1 && err(1, "poll");
				ppeers[0].revents & POLLIN &&
				(client ? client : (client = accept(peers[0], 0, 0))) == -1
				?	(warn("accept()"), 0)
				:	(
					j < lengthof(peers)
					?	(!peers[j] ? ((peers[j] = client), 0) : 1)
					:	1
				);
				++j
				)
					if (j + 1 == lengthof(peers))
						warnx("dropping %d as peer list is full", client),
						close(client);
		for (i = 1; i < written; ++i)
			if (ppeers[i].revents & POLLIN)
				for (
				readed = read(ppeers[i].fd, buf, sizeof(buf)),
				j = 1;
				j < lengthof(peers)
				?	(
					readed == -1 || !readed
					?	(
						ppeers[i].fd == peers[j]
						?	(close(peers[j]), peers[j] = 0)
						:	1
					)
					:	(peers[j] && ppeers[i].fd != peers[j] && write(peers[j], buf, readed), 1)
				)
				:	0;
				++j
				);
	}
}
