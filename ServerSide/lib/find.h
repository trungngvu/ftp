/**
 * Find path of the file in the dirPath
 * Return dirPath if found, NULL if not
 */
char *searchInDirectory(char *dirPath, char *fileName)
{
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
                char *result = searchInDirectory(path, fileName);
                if (result != NULL)
                {
                    closedir(dir);
                    return result;
                }
            }
        }
        else
        {
            if (strcmp(entry->d_name, fileName) == 0)
            {
                char *filePath = (char *)malloc(PATH_MAX);
                snprintf(filePath, PATH_MAX, "%s/%s\n", dirPath, entry->d_name);
                closedir(dir);
                return filePath;
            }
        }
    }

    closedir(dir);
    return NULL;
}

/**
 * Send path of the file in the dirPath
 * over data connection
 * Return -1 on error, 0 on success
 */
void ftserve_find(int sock_control, int sock_data, char *filename)
{
    char curr_dir[MAX_SIZE - 2];
    memset(curr_dir, 0, MAX_SIZE);
    getcwd(curr_dir, sizeof(curr_dir));
    char *foundDir = searchInDirectory(curr_dir, filename);
    // File found
    if (foundDir != NULL)
    {
        send_response(sock_control, 241);
        if (send(sock_data, foundDir, strlen(foundDir), 0) < 0)
        {
            perror("error");
            send_response(sock_control, 550);
        }
    }
    // File not found
    else
        send_response(sock_control, 441);
}