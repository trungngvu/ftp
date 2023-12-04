/**
 * Send list of files in current directory
 * over data connection
 * Return -1 on error, 0 on success
 */
int ftserve_list(int sock_data, int sock_control)
{
    struct dirent **output = NULL;
    char curr_dir[MAX_SIZE], msgToClient[MAX_SIZE];
    memset(curr_dir, 0, MAX_SIZE);
    memset(msgToClient, 0, MAX_SIZE);

    getcwd(curr_dir, sizeof(curr_dir));
    int n = scandir(curr_dir, &output, NULL, NULL);
    if (n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (strcmp(output[i]->d_name, ".") != 0 && strcmp(output[i]->d_name, "..") != 0)
            {
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