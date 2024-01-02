// Structure to hold the result (number of files and file paths)
typedef struct
{
    int count;
    char **files;
} SearchResult;

/**
 * Find path of the file in the dirPath
 * Return a SearchResult structure containing the count and file paths
 */
SearchResult searchInDirectory(char *dirPath, char *fileName)
{
    SearchResult result = {0, NULL};

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(dirPath)) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // Ignore "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char path[PATH_MAX];
                snprintf(path, PATH_MAX, "%s/%s", dirPath, entry->d_name);
                SearchResult subdirResult = searchInDirectory(path, fileName);

                // Merge the results
                result.count += subdirResult.count;
                result.files = realloc(result.files, result.count * sizeof(char *));
                for (int i = 0; i < subdirResult.count; ++i)
                {
                    result.files[result.count - subdirResult.count + i] = subdirResult.files[i];
                }

                free(subdirResult.files);
            }
        }
        else
        {
            if (strcmp(entry->d_name, fileName) == 0)
            {
                result.count++;
                result.files = realloc(result.files, result.count * sizeof(char *));
                result.files[result.count - 1] = (char *)malloc(PATH_MAX);
                snprintf(result.files[result.count - 1], PATH_MAX, "%s/%s", dirPath, entry->d_name);
            }
        }
    }

    closedir(dir);
    return result;
}

/**
 * Send path of the file in the dirPath
 * over data connection
 * Return -1 on error, 0 on success
 */
void ftserve_find(int sock_control, int sock_data, char *filename, char *cur_user, char *user_dir, RSA *key)
{
    char curr_dir[MAX_SIZE - 2], msg[MAX_SIZE] = "~";
    memset(curr_dir, 0, MAX_SIZE);
    getcwd(curr_dir, sizeof(curr_dir));
    SearchResult result = searchInDirectory(curr_dir, filename);

    // LOG
    char logstr[MAX_SIZE] = "";
    strcat(logstr, cur_user);
    strcat(logstr, " FIND ");
    strcat(logstr, filename);
    logger(logstr);

    // File found
    if (result.count > 0)
    {
        send_response(sock_control, 241, key);
        send_response(sock_control, result.count, key);
        for (int i = 0; i < result.count; ++i)
        {
            strcpy(result.files[i], result.files[i] + strlen(user_dir));
            strcat(msg, result.files[i]);
            strcat(msg, "\n");
            if (sendEncrypted(sock_data, msg, key) < 0)
            {
                perror("error");
                send_response(sock_control, 550, key);
            }
            free(result.files[i]); // Free each file path
        }
    }
    // File not found
    else
        send_response(sock_control, 441, key);
    free(result.files); // Free the array of file paths
}