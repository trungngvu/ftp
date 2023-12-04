/**
 * Do list commmand
 */
int ftclient_list(int sock_data, int sock_ctrl)
{
	size_t num_recvd;	// number of bytes received with recv()
	char buf[MAX_SIZE]; // hold a filename received from server
	int tmp = 0;

	memset(buf, 0, sizeof(buf));
	while ((num_recvd = recv(sock_data, buf, MAX_SIZE, 0)) > 0)
	{
		printf("%s", buf);
		memset(buf, 0, sizeof(buf));
	}

	if (num_recvd < 0)
	{
		perror("error");
	}
	return 0;
}