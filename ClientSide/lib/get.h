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
	char data[MAX_SIZE], stt[10] = "0";
	int size;
	int isReceiveFile = read_reply(sock_control);

	receiveDecrypted(sock_control, stt, private_key);
	if (strcmp(stt, "550") == 0)
	{
		print_reply(atoi(stt));
		return -1;
	}

	char folderName[MAX_SIZE];
	strcpy(folderName, arg);
	if (!isReceiveFile)
		strcat(arg, ".zip");
	if (strstr(arg, "user/") != NULL)
		arg = extractUsername(arg);
	FILE *fd = fopen(arg, "w");

	while ((size = receiveDecrypted(data_sock, data, private_key)) > 0)
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