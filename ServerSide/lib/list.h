/**
 * Send list of files in current directory
 * over data connection
 * Return -1 on error, 0 on success
 */
int ftserve_list(int sock_data, int sock_control, int isShare)
{
    struct dirent **output = NULL;
    char curr_dir[MAX_SIZE], msgToClient[MAX_SIZE];
    memset(curr_dir, 0, MAX_SIZE);
    memset(msgToClient, 0, MAX_SIZE);

    if (isShare)
    {
        FILE *share = fopen(".shared", "r");

        if (share == NULL)
        {
            perror("Error opening .shared");
            return -1;
        }
        // Read the content of the file and print it
        char temp[MAX_SIZE];
        while (fgets(temp, sizeof(temp), share) != NULL)
        {
            strcat(msgToClient, temp);
        }
        if (send(sock_data, msgToClient, strlen(msgToClient), 0) < 0)
        {
            perror("error");
        }
        return 0;
    }

    getcwd(curr_dir, sizeof(curr_dir));
    int n = scandir(curr_dir, &output, NULL, NULL);
    if (n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (strcmp(output[i]->d_name, ".") != 0 && strcmp(output[i]->d_name, "..") != 0)
            {
                if (strcmp(output[i]->d_name, ".shared") == 0)
                    strcpy(output[i]->d_name, "shared");
                strcat(msgToClient, output[i]->d_name);
                strcat(msgToClient, "  ");
            }
        }
    }
    strcat(msgToClient, "\n");
    if (send(sock_data, msgToClient, strlen(msgToClient), 0) < 0)
    {
        perror("error");
    }

    return 0;
}