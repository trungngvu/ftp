#include "common.h"
#include "log.h"
#include "connect.h"
#include "auth.h"
#include "list.h"
#include "cwd.h"
#include "find.h"
#include "pwd.h"
#include "zip.h"
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

	char user_dir[MAX_SIZE] = "user/";
	char *cur_user;

	// Send welcome message
	send_response(sock_control, 220);

	// receive Login or Register
	ftserve_recv_cmd(sock_control, cmd, arg, "");

	// Register user
	if (strcmp(cmd, "REG ") == 0)
	{
		if (ftserve_register(sock_control))
		{
			send_response(sock_control, 230);
			// Receive login command
			ftserve_recv_cmd(sock_control, cmd, arg, "");
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
		if (ftserve_login(sock_control, user_dir) == 1)
		{
			send_response(sock_control, 230);
		}
		else
		{
			send_response(sock_control, 430);
			exit(0);
		}
	}
	int isShare = 0;

	while (1)
	{
		// Wait for command
		cur_user = extractUsername(user_dir);
		int rc = ftserve_recv_cmd(sock_control, cmd, arg, cur_user);

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
				ftserve_list(sock_data, sock_control, isShare);
				// LOG
				char logstr[MAX_SIZE] = "";
				strcat(logstr, cur_user);
				strcat(logstr, " LIST");
				log(logstr);
			}
			else if (strcmp(cmd, "CWD ") == 0)
			{ // change directory
				ftpServer_cwd(sock_control, arg, user_dir, &isShare);
			}
			else if (strcmp(cmd, "FIND") == 0)
			{ // find file
				ftserve_find(sock_control, sock_data, arg, cur_user, user_dir);
			}
			else if (strcmp(cmd, "SHRE") == 0)
			{ // share file
				ftserve_share(sock_control, sock_data, arg, cur_user);
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
				ftserve_move(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "CPY ") == 0)
			{ // rename file and folder
				ftserve_copy(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "MKDR") == 0)
			{ // RETRIEVE: get file
				ftserve_mkdir(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "PWD ") == 0)
			{ // print working directory
				ftpServer_pwd(sock_control, sock_data, user_dir, isShare);
			}
			else if (strcmp(cmd, "RETR") == 0)
			{ // RETRIEVE: send file
				if (isShare)
				{
					char file_name[MAX_SIZE];
					strcpy(file_name, root_dir);
					strcat(file_name, arg);
					ftserve_retr(sock_control, sock_data, file_name, cur_user);
				}
				else
					ftserve_retr(sock_control, sock_data, arg, cur_user);
			}
			else if (strcmp(cmd, "STOR") == 0)
			{ // STOR: store file
				printf("Receiving ...\n");
				recvFile(sock_control, sock_data, arg, cur_user);
			}
			// Close data connection
			close(sock_data);
		}
	}
}
