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
#include <openssl/sha.h>
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

// Currently logged in user directory
char root_dir[MAX_SIZE];
RSA *public_key;
RSA *private_key;
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

/**
 * Function to split arg
 * eg input="name1 name2" into str1="name1",str2="name2"
 * return 0 when success, return -1 on error
 */
int splitString(char *input, char **str1, char **str2)
{
	if (input == NULL || strlen(input) == 0)
	{
		// Return error for empty string
		return -1;
	}

	// Find the first occurrence of a space in the string
	char *spacePos = strchr(input, ' ');

	if (spacePos == NULL || *(spacePos + 1) == '\0')
	{
		// Return error if there is no space or there is no character after space
		return -1;
	}

	// Calculate the length of the first substring
	size_t len1 = spacePos - input;

	// Allocate memory for the first substring and copy it
	*str1 = (char *)malloc(len1 + 1);
	if (*str1 == NULL)
	{
		// Return error if memory allocation fails
		return -1;
	}
	strncpy(*str1, input, len1);
	(*str1)[len1] = '\0'; // Null-terminate the string

	// Find the first non-space character after the space
	char *nonSpacePos = spacePos + 1;
	while (*nonSpacePos != '\0' && *nonSpacePos == ' ')
	{
		nonSpacePos++;
	}

	if (*nonSpacePos == '\0')
	{
		// Return error if there are no characters after the space
		free(*str1);
		return -1;
	}

	// Calculate the length of the second substring
	size_t len2 = strlen(nonSpacePos);

	// Allocate memory for the second substring and copy it
	*str2 = (char *)malloc(len2 + 1);
	if (*str2 == NULL)
	{
		// Return error if memory allocation fails
		free(*str1);
		return -1;
	}
	strcpy(*str2, nonSpacePos);

	return 0; // Success
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

// Function to lock or unlock a user in the .auth file (0 for unlock, 1 for lock)
void toggleUserLock(const char *username, int lockStatus)
{
	// Open the .auth file in read mode
	FILE *file = fopen(".auth", "r");

	// Check if the file opened successfully
	if (file == NULL)
	{
		printf("Error opening file!\n");
		return;
	}

	// Temporary variables to store data read from the file
	char currentUsername[100];
	char password[100];
	int isLocked;

	// Create a temporary file to store updated information
	FILE *tempFile = fopen("temp.auth", "w");

	// Check if the temporary file opened successfully
	if (tempFile == NULL)
	{
		printf("Error creating temporary file!\n");
		fclose(file);
		return;
	}

	// Read lines from the .auth file
	while (fscanf(file, "%s %s %d", currentUsername, password, &isLocked) == 3)
	{
		// Check if the current line corresponds to the given username
		if (strcmp(currentUsername, username) == 0)
		{
			// Update the lock status
			fprintf(tempFile, "%s %s %d\n", currentUsername, password, lockStatus);
		}
		else
		{
			// Copy the line as is to the temporary file
			fprintf(tempFile, "%s %s %d\n", currentUsername, password, isLocked);
		}
	}

	// Close both files
	fclose(file);
	fclose(tempFile);

	// Replace the original .auth file with the temporary file
	remove(".auth");
	rename("temp.auth", ".auth");

	printf("User '%s' has been %s.\n", username, lockStatus == 1 ? "locked" : "unlocked");
}

// Function to create a directory
int createDirectory(const char *path)
{
	int status = 0;
	status = mkdir(path, 0755); // 0755 provides read, write, execute permissions for owner, and read, execute permissions for group and others

	if (status == 0)
	{
		printf("Directory %s created successfully.\n", path);
		return 0; // Success
	}
	else
	{
		perror("Error creating directory");
		return -1; // Error
	}
}

// Function to extract the username from userpath
char *extractUsername(char *path)
{
	char *lastSlash = strrchr(path, '/');

	if (lastSlash != NULL)
	{
		// Return the substring after the last '/'
		return lastSlash + 1;
	}

	// Return the original path if no '/'
	return path;
}

int containsDoubleDot(const char *str)
{
	int length = strlen(str);

	for (int i = 0; i < length - 1; i++)
	{
		if (str[i] == '.' && str[i + 1] == '.')
		{
			return 1; // Found ".." in the string
		}
	}

	return 0; // Not found
}