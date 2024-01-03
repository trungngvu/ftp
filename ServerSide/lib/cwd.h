// Function to check if a directory is a subdirectory of the base directory
int isSubdirectory(const char *baseDir, const char *dir)
{
    size_t baseDirLen = strlen(baseDir);

    if (strncmp(baseDir, dir, baseDirLen) == 0)
    {
        if (dir[baseDirLen] == '\0' || dir[baseDirLen] == '/')
            return 1; // It is a subdirectory
    }

    return 0; // It is not a subdirectory
}

// Function to replace '~' with another string
char *replaceTilde(char *originalString, const char *replacement)
{
    // Check if the original string is NULL
    if (originalString == NULL)
    {
        return NULL;
    }

    // Find the number of occurrences of '~' in the original string
    int originalLength = strlen(originalString);
    int tildeCount = 0;
    for (int i = 0; i < originalLength; i++)
    {
        if (originalString[i] == '~')
        {
            tildeCount++;
        }
    }

    // Calculate the length of the new string after replacement
    int newLength = originalLength + (strlen(replacement) - 1) * tildeCount;

    // Allocate memory for the new string
    char *result = (char *)malloc(newLength + 1);

    // Perform the replacement
    int resultIndex = 0;
    for (int i = 0; i < originalLength; i++)
    {
        if (originalString[i] == '~')
        {
            // Copy the replacement string
            strcpy(&result[resultIndex], replacement);
            resultIndex += strlen(replacement);
        }
        else
        {
            // Copy the original character
            result[resultIndex] = originalString[i];
            resultIndex++;
        }
    }

    // Add null terminator at the end of the new string
    result[resultIndex] = '\0';

    return result;
}

/**
 * Change directory
 * Return -1 on error, 0 on success
 */
int ftpServer_cwd(int sock_control, char *folderName, char *user_dir, int *isShared, RSA *key)
{
    char cur_dir[MAX_SIZE];
    char prev_dir[MAX_SIZE];
    getcwd(prev_dir, sizeof(prev_dir));
    char *cur_user;
    cur_user = extractUsername(user_dir);
    folderName = replaceTilde(folderName, user_dir);

    if (strcmp(prev_dir, user_dir) == 0 && strcmp(folderName, "shared") == 0)
    {
        send_response(sock_control, 250, key); // 250 Directory successfully changed.
        *isShared = 1;
        return 0;
    }
    if (*isShared && strcmp(folderName, "..") == 0)
    {
        send_response(sock_control, 250, key); // 250 Directory successfully changed.
        *isShared = 0;
        return 0;
    }
    if (*isShared)
    {
        send_response(sock_control, 550, key); // 550 Requested action not taken
        return -1;
    }

    if (chdir(folderName) == 0) // change directory
    {
        getcwd(cur_dir, sizeof(cur_dir));
        if (!isSubdirectory(user_dir, cur_dir))
        {
            chdir(prev_dir);
            send_response(sock_control, 551, key); // 551 Directory out of user scope
        }
        else
        {
            send_response(sock_control, 250, key); // 250 Directory successfully changed.
            // LOG
            char logstr[MAX_SIZE] = "";
            strcat(logstr, cur_user);
            strcat(logstr, " CD ");
            strcat(logstr, folderName);
            logger(logstr);
        }
    }
    else
    {
        send_response(sock_control, 550, key); // 550 Requested action not taken
    }
    return 0;
}