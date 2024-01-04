int recvFile(int sock_control, int sock_data, char *filename, char *cur_user)
{
	char data[MAX_SIZE], stt[10] = "0";
	int size;
	int isReceiveFile = read_reply(sock_control);

	receiveDecrypted(sock_control, stt, private_key);
	// printf("%d\n", stt);
	if (strcmp(stt, "550") == 0)
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

	while ((size = receiveDecrypted(sock_data, data, private_key)) > 0)
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
	logger(logstr);

	printf("Receive file completed!\n");
	return 0;
}