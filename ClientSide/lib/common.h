#define FTP_CLIENT_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define INVALID_SOCKET -1
#define INVALID_IP -1
#define MAX_SIZE 1024

#define PORT 9000
#define DEFAULT_PORT 3001
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct addrinfo ADDRINFO;

struct command
{
	char arg[MAX_SIZE];
	char code[5];
};

/*Validating IP Address*/
int validate_ip(const char *ip)
{
	int value_1 = -1;
	int value_2 = -1;
	int value_3 = -1;
	int value_4 = -1;
	int count = 0;
	int i = 0;

	while (ip[i] != '\0')
	{
		if (ip[i] == '.')
			count++;
		i++;
	}

	if (count != 3)
		return INVALID_IP;
	else
	{
		sscanf(ip, "%d.%d.%d.%d", &value_1, &value_2, &value_3, &value_4);

		if (value_1 < 0 || value_2 < 0 || value_3 < 0 || value_4 < 0 || value_1 > 255 || value_2 > 255 || value_3 > 255 || value_4 > 255) /* IP Addresses from 0.0.0.0 to 255.255.255.255*/
			return INVALID_IP;
		else
			return 1;
	}
}

/**
 * Read input from command line
 */
void read_input(char *user_input, int size)
{
	memset(user_input, 0, size);
	int n = read(STDIN_FILENO, user_input, size);
	user_input[n] = '\0';

	/* Remove trailing return and newline characters */
	if (user_input[n - 1] == '\n')
		user_input[n - 1] = '\0';
	if (user_input[n - 1] == '\r')
		user_input[n - 1] = '\0';
}

// Function to check if a path corresponds to a file
int isFile(const char *path)
{
	struct stat pathStat;
	if (stat(path, &pathStat) == 0)
	{
		return S_ISREG(pathStat.st_mode);
	}
	return 0; // Return 0 for error or if the path is not a regular file
}

// Function to check if a path corresponds to a directory (folder)
int isDirectory(const char *path)
{
	struct stat pathStat;
	if (stat(path, &pathStat) == 0)
	{
		return S_ISDIR(pathStat.st_mode);
	}
	return 0; // Return 0 for error or if the path is not a directory
}