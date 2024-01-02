/**
 * Send list of files in current directory
 * over data connection
 * Return -1 on error, 0 on success
 */
void ftpServer_pwd(int sock_control, int sock_data, char *user_dir, int isShare, RSA *key)
{
    char curr_dir[MAX_SIZE - 10], msgToClient[MAX_SIZE];
    memset(curr_dir, 0, MAX_SIZE);
    memset(msgToClient, 0, MAX_SIZE);

    getcwd(curr_dir, sizeof(curr_dir));
    strcpy(curr_dir, curr_dir + strlen(user_dir));
    if (!isShare)
        sprintf(msgToClient, "~%s\n", curr_dir);
    else
        sprintf(msgToClient, "~%s/shared\n", curr_dir);
    if (sendEncrypted(sock_data, msgToClient, key) < 0)
    {
        perror("error");
        send_response(sock_control, 550, key);
    }
    // LOG
    char logstr[MAX_SIZE] = "PWD ";
    strcat(logstr, curr_dir);
    logger(logstr);
    send_response(sock_control, 212, key);
}