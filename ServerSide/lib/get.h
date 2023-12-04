int recvFile(int sock_control, int sock_data, char *filename)
{
	char data[MAX_SIZE];
	int size, stt = 0;

	recv(sock_control, &stt, sizeof(stt), 0);
	// printf("%d\n", stt);
	if (stt == 550)
	{
		printf("can't not open file!\n");
		return -1;
	}
	else
	{

		FILE *fd = fopen(filename, "w");

		while ((size = recv(sock_data, data, MAX_SIZE, 0)) > 0)
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
	return 0;
}