#include "common.h"
#include "RSA.h"
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

	// Exchange public key with user
	RSA *user_public_key;
	readKey();
	strcpy(arg, rsa_key_to_string(public_key, 0));
	send(sock_control, arg, sizeof(arg), 0);
	recv(sock_control, arg, sizeof(arg), 0);
	user_public_key = string_to_rsa_key(arg, 0);

	// Send welcome message
	send_response(sock_control, 220, user_public_key);

	// receive Login or Register
	ftserve_recv_cmd(sock_control, cmd, arg, "", user_public_key);

	// Register user
	if (strcmp(cmd, "REG ") == 0)
	{
		if (ftserve_register(sock_control, user_public_key))
		{
			send_response(sock_control, 230, user_public_key);
			// Receive login command
			ftserve_recv_cmd(sock_control, cmd, arg, "", user_public_key);
		}
		else
		{
			send_response(sock_control, 430, user_public_key);
			exit(0);
		}
	}
	// Authenticate user
	if (strcmp(cmd, "LGIN") == 0)
	{
		if (ftserve_login(sock_control, user_dir, user_public_key) == 1)
		{
			send_response(sock_control, 230, user_public_key);
		}
	}
	int isShare = 0;

	while (1)
	{
		// Wait for command
		cur_user = extractUsername(user_dir);
		int rc = ftserve_recv_cmd(sock_control, cmd, arg, cur_user, user_public_key);

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
				ftserve_list(sock_data, sock_control, isShare, user_public_key);
				// LOG
				char logstr[MAX_SIZE] = "";
				strcat(logstr, cur_user);
				strcat(logstr, " LIST");
				logger(logstr);
			}
			else if (strcmp(cmd, "CWD ") == 0)
			{ // change directory
				ftpServer_cwd(sock_control, arg, user_dir, &isShare, user_public_key);
			}
			else if (strcmp(cmd, "FIND") == 0)
			{ // find file
				ftserve_find(sock_control, sock_data, arg, cur_user, user_dir, user_public_key);
			}
			else if (strcmp(cmd, "SHRE") == 0)
			{ // share file
				ftserve_share(sock_control, sock_data, arg, cur_user, user_public_key);
			}
			else if (strcmp(cmd, "RENM") == 0)
			{ // rename file and folder
				ftserve_rename(sock_control, sock_data, arg, user_public_key);
			}
			else if (strcmp(cmd, "DEL ") == 0)
			{ // rename file and folder
				ftserve_delete(sock_control, sock_data, arg, user_public_key);
			}
			else if (strcmp(cmd, "MOV ") == 0)
			{ // rename file and folder
				ftserve_move(sock_control, sock_data, arg, user_public_key);
			}
			else if (strcmp(cmd, "CPY ") == 0)
			{ // rename file and folder
				ftserve_copy(sock_control, sock_data, arg, user_public_key);
			}
			else if (strcmp(cmd, "MKDR") == 0)
			{ // RETRIEVE: get file
				ftserve_mkdir(sock_control, sock_data, arg, user_public_key);
			}
			else if (strcmp(cmd, "PWD ") == 0)
			{ // print working directory
				ftpServer_pwd(sock_control, sock_data, user_dir, isShare, user_public_key);
			}
			else if (strcmp(cmd, "RETR") == 0)
			{ // RETRIEVE: send file
				if (isShare)
				{
					char share_dir[MAX_SIZE];
					strcpy(share_dir, user_dir);
					strcat(share_dir, "/.shared");
					if (!containsExactString(share_dir, arg))
					{
						send_response(sock_control, 1, user_public_key);
						send_response(sock_control, 550, user_public_key);
					}
					else
					{
						char file_name[MAX_SIZE];
						strcpy(file_name, root_dir);
						strcat(file_name, arg);
						ftserve_retr(sock_control, sock_data, file_name, cur_user, user_public_key, 1);
					}
				}
				else
					ftserve_retr(sock_control, sock_data, arg, cur_user, user_public_key, 0);
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
