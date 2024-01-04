/**
 * Send file specified in filename over data connection, sending
 * control message over control connection
 * Handles case of null or invalid filename
 */
void ftserve_retr(int sock_control, int sock_data, char *filename, char *cur_user, RSA *key, int isShare)
{
	printf("%s\n", filename);
	FILE *fd = NULL;
	char data[MAX_SIZE];
	memset(data, 0, MAX_SIZE);
	size_t num_read;

	char tempZip[MAX_SIZE];
	int isDir = isDirectory(filename);

	if (isDir)
	{
		strcpy(tempZip, filename);
		strcat(tempZip, ".zip");
		printf("%s %s\n", filename, tempZip);
		zipFolder(filename, tempZip, isShare);
		strcpy(filename, tempZip);
		// tell client that we're sending a folder
		send_response(sock_control, 0, key);
	}
	else
		send_response(sock_control, 1, key);

	fd = fopen(filename, "r");

	if (!fd)
	{
		// send error code (550 Requested action not taken)
		send_response(sock_control, 550, key);
	}
	else
	{
		// send okay (150 File status okay)
		send_response(sock_control, 150, key);

		do
		{
			num_read = fread(data, 1, 245, fd);
			if (num_read < 0)
			{
				printf("error in fread()\n");
			}

			// send block
			if (sendEncryptedBlock(sock_data, data, num_read, key) < 0)
				perror("error sending file\n");

		} while (num_read > 0);

		// send message: 226: closing conn, file transfer successful
		send_response(sock_control, 226, key);
		// LOG
		char logstr[MAX_SIZE] = "";
		strcat(logstr, cur_user);
		strcat(logstr, " RETRIEVE ");
		strcat(logstr, filename);
		logger(logstr);

		fclose(fd);
		if (isDir)
			remove(filename);
	}
}