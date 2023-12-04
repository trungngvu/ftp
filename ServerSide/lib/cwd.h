/**
 * Change directory
 * Return -1 on error, 0 on success
 */
int ftpServer_cwd(int sock_control, char *folderName)
{
    if (chdir(folderName) == 0) // change directory
    {
        send_response(sock_control, 250); // 250 Directory successfully changed.
    }
    else
    {
        send_response(sock_control, 550); // 550 Requested action not taken
    }
    return 0;
}