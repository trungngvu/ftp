int recvFile(int sock_control, int sock_data, char *filename, char* cur_user)
{
	char data[MAX_SIZE];
	int size, stt = 0;
	int isReceiveFile = read_reply(sock_control);

	recv(sock_control, &stt, sizeof(stt), 0);
	// printf("%d\n", stt);
	if (stt == 550)
	{
		printf("can't not open file!\n");
		return -1;
	}
	if (strcmp(filename, ".shared") == 0)
	{
		printf("User should not upload .shared!\n");
		return -1;
	}

	char folderName[MAX_SIZE];
	strcpy(folderName, filename);
	if (!isReceiveFile)
		strcat(filename, ".zip");
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
	if (!isReceiveFile)
	{
		unzipFolder(filename, folderName);
		remove(filename);
	}
	// LOG
	char logstr[MAX_SIZE] = "";
	strcat(logstr, cur_user);
	strcat(logstr, " STOR ");
	strcat(logstr, filename);
	log(logstr);
	return 0;
}