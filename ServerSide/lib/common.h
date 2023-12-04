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
#include <openssl/sha.h>

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
#include <openssl/sha.h>

#define INVALID_SOCKET -1
#define INVALID_IP -1
#define MAX_SIZE 1024
#define PORT 9000
#define DEFAULT_PORT 3000
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

// Currently logged in username
char cur_user[MAX_SIZE];

/**
 * Trim whitespace and line ending
 * characters from a string
 */
void trimstr(char *str, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (isspace(str[i]))
			str[i] = 0;
		if (str[i] == '\n')
			str[i] = 0;
	}
}