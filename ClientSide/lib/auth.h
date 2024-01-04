/**
 * Get login details from user and
 * send to server for authentication
 */
void ftclient_login(int sock_control)
{
	struct command cmd;
	char user[MAX_SIZE];
	memset(user, 0, MAX_SIZE);

	// Send LOGIN command to server
	strcpy(cmd.code, "LGIN");
	strcpy(cmd.arg, "");
	ftclient_send_cmd(&cmd, sock_control);

	// Wait for go-ahead
	char wait[10];
	receiveDecrypted(sock_control, wait, private_key);

	// Send USER command to server
	int rep;
	do
	{
		// Get username from user
		printf("Name: ");
		fflush(stdout);
		read_input(user, MAX_SIZE);
		strcpy(cmd.code, "USER");
		strcpy(cmd.arg, user);
		ftclient_send_cmd(&cmd, sock_control);
		rep = read_reply(sock_control);
		if (rep == 431)
			printf("431 Username does not exist or user has been locked.\n");
	} while (rep == 431);

	int try = 1;
	do
	{
		// Get password from user
		fflush(stdout);
		char *pass = getpass("Password: ");
		// Send PASS command to server
		strcpy(cmd.code, "PASS");
		strcpy(cmd.arg, pass);
		ftclient_send_cmd(&cmd, sock_control);
		rep = read_reply(sock_control);
		if (rep == 431)
		{
			printf("431 Password incorrect. Attempt left: %d\n", 3 - try);
			try++;
		}
	} while (try <= 3 && rep == 431);

	if (try == 4)
	{
		printf("432 Too many attempts, your account has been locked!\n");
		exit(0);
	}

	switch (rep)
	{
	case 430:
		printf("430 User in use or account has been locked.\n");
		exit(0);
	case 230:
		printf("230 Successful login.\n");
		break;
	}
}

/**
 * Register new user
 */
void ftclient_register(int sock_control)
{
	struct command cmd;
	char user[MAX_SIZE];
	memset(user, 0, MAX_SIZE);

	// Send REG command to server
	strcpy(cmd.code, "REG");
	ftclient_send_cmd(&cmd, sock_control);

	// Wait for go-ahead
	char wait[10];
	receiveDecrypted(sock_control, wait, private_key);

	// Send USER command to server
	int rep;
	do
	{
		// Get username from user
		printf("Name: ");
		fflush(stdout);
		read_input(user, MAX_SIZE);
		strcpy(cmd.code, "USER");
		strcpy(cmd.arg, user);
		ftclient_send_cmd(&cmd, sock_control);
		rep = read_reply(sock_control);
		if (rep == 431)
			printf("431 Username already exist.\n");
	} while (rep == 431);

	// Get password from user
	fflush(stdout);
	char *pass = getpass("Password: ");

	// Send PASS command to server
	strcpy(cmd.code, "PASS");
	strcpy(cmd.arg, pass);
	ftclient_send_cmd(&cmd, sock_control);

	// wait for response
	int retcode = read_reply(sock_control);
	switch (retcode)
	{
	case 431:
		printf("431 Username already exist.\n");
		exit(0);
	case 230:
		printf("230 Successfully registered.\n");
		break;
	default:
		perror("error reading message from server");
		exit(1);
		break;
	}
}