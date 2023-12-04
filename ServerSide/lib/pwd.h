/**
 * Send list of files in current directory
 * over data connection
 * Return -1 on error, 0 on success
 */
void ftpServer_pwd(int sock_control, int sock_data)
{
    char curr_dir[MAX_SIZE - 2], msgToClient[MAX_SIZE];
    memset(curr_dir, 0, MAX_SIZE);
    memset(msgToClient, 0, MAX_SIZE);

    getcwd(curr_dir, sizeof(curr_dir));
    sprintf(msgToClient, "%s\n", curr_dir);
    if (send(sock_data, msgToClient, strlen(msgToClient), 0) < 0)
    {
        perror("error");
        send_response(sock_control, 550);
    }
    send_response(sock_control, 212);
}