/**
 * Create listening socket on remote host
 * Returns -1 on error, socket fd on success
 */
int socket_create(int port)
{
	int sockfd;
	SOCKADDR_IN sock_addr;

	// create new socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket() error");
		return -1;
	}

	// set local address info
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind
	int flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
	if (bind(sockfd, (SOCKADDR *)&sock_addr, sizeof(sock_addr)) < 0)
	{
		close(sockfd);
		perror("bind() error");
		return -1;
	}

	// begin listening for incoming TCP requests
	if (listen(sockfd, 5) < 0)
	{
		close(sockfd);
		perror("listen() error");
		return -1;
	}
	return sockfd;
}

/**
 * Create new socket for incoming client connection request
 * Returns -1 on error, or fd of newly created socket
 */
int socket_accept(int sock_listen)
{
	int sockfd;
	SOCKADDR_IN client_addr;
	int len = sizeof(client_addr);

	// Wait for incoming request, store client info in client_addr
	sockfd = accept(sock_listen, (SOCKADDR *)&client_addr, &len);

	if (sockfd < 0)
	{
		perror("accept() error");
		return -1;
	}
	return sockfd;
}

/**
 * Open data connection
 */
int ftclient_open_conn(int sock_con)
{
	int sock_listen = socket_create(DEFAULT_PORT);

	// send an ACK on control conn
	int ack = 1;
	if ((send(sock_con, &ack, sizeof(ack), 0)) < 0)
	{
		printf("client: ack write error :%d\n", errno);
		exit(1);
	}

	int sock_conn = socket_accept(sock_listen);
	close(sock_listen);
	return sock_conn;
}