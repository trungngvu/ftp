#include "common.h"
#include "connect.h"
#include "auth.h"
#include "list.h"
#include "cwd.h"
#include "find.h"
#include "pwd.h"
#include "retr.h"
#include "get.h"
#include "file.h"

/**
 * Child process handles connection to client
 */
void ftserve_process(int sock_control)
{
	int sock_data;
	char cmd[5];
	char arg[MAX_SIZE];

	// Send welcome message
	send_response(sock_control, 220);

	// receive Login or Register
	ftserve_recv_cmd(sock_control, cmd, arg);

	// Register user
	if (strcmp(cmd, "REG ") == 0)
	{
		if (ftserve_register(sock_control))
		{
			send_response(sock_control, 230);
			// Receive login command
			ftserve_recv_cmd(sock_control, cmd, arg);
		}
		else
		{
			send_response(sock_control, 430);
			exit(0);
		}
	}
	// Authenticate user
	if (strcmp(cmd, "LGIN") == 0)
	{
		if (ftserve_login(sock_control) == 1)
		{
			send_response(sock_control, 230);
		}
		else
		{
			send_response(sock_control, 430);
			exit(0);
		}
	}

	while (1)
	{
		// Wait for command
		int rc = ftserve_recv_cmd(sock_control, cmd, arg);

		if ((rc < 0) || (rc == 221))
		{
			break;
		}
		if (rc == 200)
		{
			// Open data connection with client
			if ((sock_data = ftserve_start_data_conn(sock_control)) < 0)
			{
				close(sock_control);
				exit(1);
			}
			// Execute command
			if (strcmp(cmd, "LIST") == 0)
			{ // Do list
				ftserve_list(sock_data, sock_control);
			}
			else if (strcmp(cmd, "CWD ") == 0)
			{ // change directory
				ftpServer_cwd(sock_control, arg);
			}
			else if (strcmp(cmd, "FIND") == 0)
			{ // find file
				ftserve_find(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "RENM") == 0)
			{ // rename file and folder
				ftserve_rename(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "DEL ") == 0)
			{ // rename file and folder
				ftserve_delete(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "MOV ") == 0)
			{ // rename file and folder
				ftserve_moveFile(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "CPY ") == 0)
			{ // rename file and folder
				ftserve_copyFile(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "PWD ") == 0)
			{ // print working directory
				ftpServer_pwd(sock_control, sock_data);
			}
			else if (strcmp(cmd, "RETR") == 0)
			{ // RETRIEVE: get file
				ftserve_retr(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "STOR") == 0)
			{ // RETRIEVE: get file
				printf("Receving ...\n");
				recvFile(sock_control, sock_data, arg);
			}
			// Close data connection
			close(sock_data);
		}
	}
}
