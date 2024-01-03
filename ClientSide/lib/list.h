/**
 * Do list commmand
 */
int ftclient_list(int sock_data, int sock_ctrl)
{
	int num_recvd;		// number of bytes received with recv()
	char buf[MAX_SIZE]; // hold a filename received from server
	int tmp = 0;

	memset(buf, 0, sizeof(buf));
	while ((num_recvd = receiveDecrypted(sock_data, buf, private_key)) > 0)
	{
		printf("%s", buf);
		memset(buf, 0, sizeof(buf));
	}
	return 0;
}