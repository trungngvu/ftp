/**
 * Send file specified in filename over data connection, sending
 * control message over control connection
 * Handles case of null or invalid filename
 */
void ftserve_retr(int sock_control, int sock_data, char *filename)
{
	FILE *fd = NULL;
	char data[MAX_SIZE];
	memset(data, 0, MAX_SIZE);
	size_t num_read;

	fd = fopen(filename, "r");

	if (!fd)
	{
		// send error code (550 Requested action not taken)
		send_response(sock_control, 550);
	}
	else
	{
		// send okay (150 File status okay)
		send_response(sock_control, 150);

		do
		{
			num_read = fread(data, 1, MAX_SIZE, fd);

			if (num_read < 0)
			{
				printf("error in fread()\n");
			}

			// send block
			if (send(sock_data, data, num_read, 0) < 0)
				perror("error sending file\n");

		} while (num_read > 0);

		// send message: 226: closing conn, file transfer successful
		send_response(sock_control, 226);

		fclose(fd);
	}
}