/**
 * Do get <filename> command
 */
int ftclient_get(int data_sock, int sock_control, char *arg)
{
	char data[MAX_SIZE];
	int size;
	FILE *fd = fopen(arg, "w");

	while ((size = recv(data_sock, data, MAX_SIZE, 0)) > 0)
	{
		fwrite(data, 1, size, fd);
	}

	if (size < 0)
	{
		perror("error\n");
	}

	fclose(fd);
	return 0;
}