void upload(int data_sock, char *filename, int sock_control)
{
	FILE *fd = NULL;
	char data[MAX_SIZE];
	memset(data, 0, MAX_SIZE);
	size_t num_read;
	int stt;

	char tempZip[MAX_SIZE];
	int isDir = isDirectory(filename);

	if (isDir)
	{
		strcpy(tempZip, filename);
		strcat(tempZip, ".zip");
		zipFolder(filename, tempZip);
		strcpy(filename, tempZip);
		// tell server that we're sending a folder
		send_response(sock_control, 0);
	}
	else
		send_response(sock_control, 1);

	fd = fopen(filename, "r");

	if (!fd)
	{
		// send error code (550 Requested action not taken)
		printf("550 Requested action not taken\n");
		stt = 550;
		send(sock_control, &stt, sizeof(stt), 0);
	}
	else
	{
		// send okay (150 File status okay)
		stt = 150;
		send(sock_control, &stt, sizeof(stt), 0);

		do
		{
			num_read = fread(data, 1, MAX_SIZE, fd);

			if (num_read < 0)
			{
				printf("error in fread()\n");
			}

			// send block
			send(data_sock, data, num_read, 0);

		} while (num_read > 0);
		fclose(fd);
		if (isDir)
			remove(filename);
	}
}