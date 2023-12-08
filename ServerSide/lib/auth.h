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

// Function to remove duplicates from the file
void removeDuplicates(FILE *file)
{
    char line[256];
    char uniqueLines[10000][256]; // Assuming a maximum of 10000 lines and each line is not more than 256 characters

    int numLines = 0;
    fseek(file, 0, SEEK_SET); // Move the file pointer to the beginning

    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character
        strtok(line, "\n");

        int isDuplicate = 0;

        // Check if the line is a duplicate
        for (int i = 0; i < numLines; ++i)
        {
            if (strcmp(line, uniqueLines[i]) == 0)
            {
                isDuplicate = 1;
                break;
            }
        }

        // If not a duplicate, add it to the unique lines array
        if (!isDuplicate)
        {
            strcpy(uniqueLines[numLines], line);
            numLines++;
        }
    }

    // Move the file pointer to the beginning and truncate the file
    freopen(NULL, "w", file);

    // Write the unique lines back to the file
    for (int i = 0; i < numLines; ++i)
    {
        if (strcmp(uniqueLines[i], "\n") == 0)
            continue;
        fprintf(file, "%s\n", uniqueLines[i]);
    }
}

// Function to clean up the file
void cleanUpFile(const char *filename)
{
    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char line[256];
    FILE *tempFile = tmpfile();

    // Copy valid lines to a temporary file
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character
        strtok(line, "\n");
        char dir[MAX_SIZE] = "";
        strcat(dir, root_dir);
        strcat(dir, line);

        if (isFile(dir))
        {
            fprintf(tempFile, "%s\n", line);
        }
    }

    // Move the file pointer to the beginning and truncate the original file
    freopen(NULL, "w", file);

    // Copy the content from the temporary file back to the original file
    fseek(tempFile, 0, SEEK_SET);
    while (fgets(line, sizeof(line), tempFile))
    {
        fprintf(file, "%s\n", line);
    }

    // Close the files
    fclose(file);
    fclose(tempFile);

    // Remove duplicates from the cleaned file
    file = fopen(filename, "r+");
    if (file != NULL)
    {
        removeDuplicates(file);
        fclose(file);
    }
}

/**
 * Authenticate a user's credentials
 * Return 1 if authenticated, 0 if not
 */
int ftserve_check_user(char *user, char *pass, char *user_dir)
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    char curDir[MAX_SIZE];
    char shared[MAX_SIZE] = "";
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
            // Lock user to prevent concurrent login
            toggleUserLock(user, 1);

            // Change dir to user root dir
            strcat(user_dir, username);
            chdir(user_dir);

            // Save user root dir to a global variable for future use
            getcwd(curDir, sizeof(curDir));
            strcpy(user_dir, curDir);

            // Clean up user's .shared file
            strcat(shared, root_dir);
            strcat(shared, "/user/");
            strcat(shared, user);
            strcat(shared, "/.shared");
            cleanUpFile(shared);
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
int ftserve_login(int sock_control, char * user_dir)
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

    return (ftserve_check_user(user, pass, user_dir));
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