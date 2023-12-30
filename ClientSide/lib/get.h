/**
 * Do get <filename> command
 */
// Function to extract the username from userpath
char *extractUsername(char *path)
{
	char *lastSlash = strrchr(path, '/');

	if (lastSlash != NULL)
	{
		// Return the substring after the last '/'
		return lastSlash + 1;
	}

	// Return the original path if no '/'
	return path;
}

int ftclient_get(int data_sock, int sock_control, char *arg)
{
	char data[MAX_SIZE];
	int size, stt = 0;
	int isReceiveFile = read_reply(sock_control);

	recv(sock_control, &stt, sizeof(stt), 0);
	if (stt == 550)
	{
		print_reply(stt);
		return -1;
	}

	char folderName[MAX_SIZE];
	strcpy(folderName, arg);
	if (!isReceiveFile)
		strcat(arg, ".zip");
	if (strstr(arg, "user/") != NULL)
		arg = extractUsername(arg);
	FILE *fd = fopen(arg, "w");

	while ((size = recv(data_sock, data, MAX_SIZE, 0)) > 0)
		fwrite(data, 1, size, fd);

	if (size < 0)
		perror("error\n");

	fclose(fd);
	int rep = read_reply(sock_control);
	print_reply(rep);
	if (!isReceiveFile)
	{
		unzipFolder(arg, folderName);
		remove(arg);
	}
	return 0;
}