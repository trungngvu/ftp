void sha256(const char *input, char *output)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Calculate SHA-256 hash
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);

    // Encode hash in hexadecimal
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(output + 2 * i, "%02x", hash[i]);
    }
    output[2 * SHA256_DIGEST_LENGTH] = '\0';
}

/**
 * Authenticate a user's credentials
 * Return 1 if authenticated, 0 if not
 */
int ftserve_check_user(char *user, char *pass)
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    char curDir[MAX_SIZE];
    int isLock;
    char *pch;
    char buf[MAX_SIZE];
    char *line = NULL;
    size_t num_read;
    size_t len = 0;
    FILE *fd;
    int auth = 0;

    fd = fopen(".auth", "r");
    if (fd == NULL)
    {
        perror("file not found");
        exit(1);
    }

    while ((num_read = getline(&line, &len, fd)) != -1)
    {
        memset(buf, 0, MAX_SIZE);
        strcpy(buf, line);

        pch = strtok(buf, " ");
        strcpy(username, pch);

        if (pch != NULL)
        {
            pch = strtok(NULL, " ");
            strcpy(password, pch);
            pch = strtok(NULL, " ");
            trimstr(pch, (int)strlen(pch));
            isLock = atoi(pch);
        }

        // remove end of line and whitespace
        trimstr(password, (int)strlen(password));

        char outputBuffer[65];
        sha256(pass, outputBuffer);

        if ((strcmp(user, username) == 0) && (strcmp(outputBuffer, password) == 0 && (isLock == 0)))
        {
            auth = 1;
            toggleUserLock(user, 1);
            strcat(user_dir, username);
            chdir(user_dir);
            getcwd(curDir, sizeof(curDir));
            strcpy(user_dir, curDir);
            strcpy(cur_user, user);
            break;
        }
    }
    free(line);
    fclose(fd);
    return auth;
}

/**
 * Check if db has existing username
 * Return 1 if authenticated, 0 if not
 */
int ftserve_check_username(char *user)
{
    char username[MAX_SIZE];
    char *pch;
    char buf[MAX_SIZE];
    char *line = NULL;
    size_t num_read;
    size_t len = 0;
    FILE *fd;
    int check = 0;

    fd = fopen(".auth", "r");
    if (fd == NULL)
    {
        perror("file not found");
        exit(1);
    }

    while ((num_read = getline(&line, &len, fd)) != -1)
    {
        memset(buf, 0, MAX_SIZE);
        strcpy(buf, line);

        pch = strtok(buf, " ");
        strcpy(username, pch);

        if (strcmp(user, username) == 0)
        {
            check = 1;
            break;
        }
    }
    free(line);
    fclose(fd);
    return check;
}

/**
 * Log in connected client
 */
int ftserve_login(int sock_control)
{
    char buf[MAX_SIZE];
    char user[MAX_SIZE];
    char pass[MAX_SIZE];
    memset(user, 0, MAX_SIZE);
    memset(pass, 0, MAX_SIZE);
    memset(buf, 0, MAX_SIZE);

    // Wait to receive username
    if ((recv_data(sock_control, buf, sizeof(buf))) == -1)
    {
        perror("recv error\n");
        exit(1);
    }

    strcpy(user, buf + 5); // 'USER ' has 5 char

    // tell client we're ready for password
    send_response(sock_control, 331);

    // Wait to receive password
    memset(buf, 0, MAX_SIZE);
    if ((recv_data(sock_control, buf, sizeof(buf))) == -1)
    {
        perror("recv error\n");
        exit(1);
    }

    strcpy(pass, buf + 5); // 'PASS ' has 5 char

    return (ftserve_check_user(user, pass));
}

/**
 * Log in connected client
 */
int ftserve_register(int sock_control)
{
    char buf[MAX_SIZE];
    char user[MAX_SIZE];
    char pass[MAX_SIZE];
    char userDir[MAX_SIZE] = "user/";
    memset(user, 0, MAX_SIZE);
    memset(pass, 0, MAX_SIZE);
    memset(buf, 0, MAX_SIZE);

    // Wait to receive username
    if ((recv_data(sock_control, buf, sizeof(buf))) == -1)
    {
        perror("recv error\n");
        exit(1);
    }

    strcpy(user, buf + 5); // 'USER ' has 5 char

    while (ftserve_check_username(user))
    {
        // tell client username already exist
        send_response(sock_control, 431);
        // Wait to receive username
        if ((recv_data(sock_control, buf, sizeof(buf))) == -1)
        {
            perror("recv error\n");
            exit(1);
        }
        strcpy(user, buf + 5); // 'USER ' has 5 char
    }

    // tell client we're ready for password
    send_response(sock_control, 331);

    // Wait to receive password
    memset(buf, 0, MAX_SIZE);
    if ((recv_data(sock_control, buf, sizeof(buf))) == -1)
    {
        perror("recv error\n");
        exit(1);
    }

    strcpy(pass, buf + 5); // 'PASS ' has 5 char}

    FILE *fptr;

    fptr = fopen(".auth", "a");
    if (fptr == NULL)
    {
        perror("file not found");
        exit(1);
    }
    fprintf(fptr, "%s ", user);
    char outputBuffer[65];
    sha256(pass, outputBuffer);
    fprintf(fptr, "%s 0\n", outputBuffer);

    // Make new user folder
    strcat(userDir, user);
    createDirectory(userDir);
    strcat(userDir, "/.shared");
    FILE *shared;
    shared = fopen(userDir, "w");
    fclose(shared);

    fclose(fptr);
    return 1;
}