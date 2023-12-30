int renameFile(const char *oldName, const char *newName)
{
    if (oldName == NULL || newName == NULL)
    {
        fprintf(stderr, "Invalid input: oldName and newName cannot be NULL\n");
        return -1;
    }

    if (rename(oldName, newName) != 0)
    {
        perror("Error renaming file");
        return -1;
    }

    printf("File successfully renamed from '%s' to '%s'\n", oldName, newName);
    return 0;
}

void handleError(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void recursiveDelete(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        handleError("Error opening folder");
    }

    struct dirent *entry;
    struct stat info;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char filePath[PATH_MAX];
            snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

            if (stat(filePath, &info) != 0)
            {
                handleError("Error getting file information");
            }

            if (S_ISDIR(info.st_mode))
            {
                recursiveDelete(filePath);
                rmdir(filePath);
            }
            else
            {
                if (remove(filePath) != 0)
                {
                    handleError("Error deleting file");
                }
            }
        }
    }

    closedir(dir);
}

int deleteFolder(const char *path)
{
    recursiveDelete(path);
    if (rmdir(path) != 0)
    {
        perror("Error deleting folder");
        return -1;
    }

    printf("Folder deleted: %s\n", path);
    return 0;
}

int deleteFile(const char *filename)
{
    if (isFile(filename))
    {
        printf("file\n");
        if (remove(filename) == 0)
        {
            printf("File %s deleted successfully.\n", filename);
            return 0; // Success
        }
        else
        {
            perror("Error deleting file");
            return -1; // Error
        }
    }
    else if (isDirectory(filename))
    {
        printf("folder\n");
        if (deleteFolder(filename) == 0)
        {
            printf("Folder %s deleted successfully.\n", filename);
            return 0; // Success
        }
        else
        {
            perror("Error deleting folder");
            return -1; // Error
        }
    }
}

// Function to copy or move a file
// Parameters:
//   sourceFilename: path to the source file
//   destinationFilename: path to the destination file
//   mode: 0 for copy, 1 for move
int copyOrMoveFile(char *sourceFilename, char *destinationFilename, int mode)
{
    FILE *sourceFile, *destinationFile;

    // Open the source file for reading
    sourceFile = fopen(sourceFilename, "rb");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        return -1; // Error
    }

    if (strcmp(destinationFilename, ".") == 0 && mode == 0)
    {
        char source[MAX_SIZE] = "Copy_";
        strcat(source, sourceFilename);
        strcpy(sourceFilename, source);
    }
    if (strcmp(destinationFilename, ".") == 0 && mode == 1)
    {
        return 0;
    }
    // Open the destination file for writing
    strcat(destinationFilename, "/");
    strcat(destinationFilename, sourceFilename);
    destinationFile = fopen(destinationFilename, "wb");
    if (destinationFile == NULL)
    {
        fclose(sourceFile);
        perror("Error opening destination file");
        return -1; // Error
    }

    size_t num_read, num_write;
    char data[MAX_SIZE];
    // Copy the contents of the source file to the destination file
    do
    {
        num_read = fread(data, 1, MAX_SIZE, sourceFile);
        if (num_read < 0)
            printf("error in fread()\n");
        num_write = fwrite(data, 1, num_read, destinationFile);
        if (num_write < 0)
            printf("error in fwrite()\n");
    } while (num_read > 0);

    // Close the files
    fclose(sourceFile);
    fclose(destinationFile);

    if (mode == 0)
    {
        printf("File %s copied to %s successfully.\n", sourceFilename, destinationFilename);
    }
    else if (mode == 1)
    {
        remove(sourceFilename); // Remove the source file if it's a move operation
        printf("File %s moved to %s successfully.\n", sourceFilename, destinationFilename);
    }
    else
    {
        printf("Invalid mode. Use 0 for copy or 1 for move.\n");
        return -1; // Error
    }

    return 0; // Success
}

// Function to recursively copy files and subdirectories
int copyDirectory(char *sourcePath, char *destinationPath)
{
    DIR *dir;
    struct dirent *entry;

    // Open the source directory
    dir = opendir(sourcePath);
    if (dir == NULL)
    {
        perror("Error opening source directory");
        return -1; // Error
    }

    // Create the destination directory
    createDirectory(destinationPath);

    // Iterate through all entries in the source directory
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Create full paths for source and destination
            char sourceFile[PATH_MAX];
            char destinationFile[PATH_MAX];
            sprintf(sourceFile, "%s/%s", sourcePath, entry->d_name);
            sprintf(destinationFile, "%s/%s", destinationPath, entry->d_name);

            if (entry->d_type == DT_DIR)
            {
                // Recursively copy subdirectories
                copyDirectory(sourceFile, destinationFile);
            }
            else
            {
                // Copy files
                FILE *sourceFilePtr, *destinationFilePtr;
                char ch;

                sourceFilePtr = fopen(sourceFile, "rb");
                if (sourceFilePtr == NULL)
                {
                    perror("Error opening source file");
                    closedir(dir);
                    return -1; // Error
                }

                destinationFilePtr = fopen(destinationFile, "wb");
                if (destinationFilePtr == NULL)
                {
                    fclose(sourceFilePtr);
                    perror("Error opening destination file");
                    closedir(dir);
                    return -1; // Error
                }

                while ((ch = fgetc(sourceFilePtr)) != EOF)
                {
                    fputc(ch, destinationFilePtr);
                }

                fclose(sourceFilePtr);
                fclose(destinationFilePtr);
            }
        }
    }

    closedir(dir);
    return 0; // Success
}

// Function to recursively move files and subdirectories
int moveDirectory(char *sourcePath, char *destinationPath)
{
    if (copyDirectory(sourcePath, destinationPath) == 0)
    {
        // If copy is successful, remove the source directory
        deleteFile(sourcePath);
        printf("Directory %s moved to %s successfully.\n", sourcePath, destinationPath);
        return 0; // Success
    }
    else
    {
        printf("Failed to move the directory.\n");
        return -1; // Error
    }
}

void appendLineToSharedFile(const char *username, const char *line)
{
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%s/.shared", username);

    FILE *file = fopen(filePath, "a");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n", line);

    fclose(file);
}

void processUserFolder(const char *userFolderPath, const char *excludedUsername, const char *line)
{
    DIR *dir = opendir(userFolderPath);
    if (dir == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char subFolderPath[MAX_SIZE];
            snprintf(subFolderPath, sizeof(subFolderPath), "%s/%s", userFolderPath, entry->d_name);

            // Skip the excludedUsername
            if (strcmp(entry->d_name, excludedUsername) != 0)
            {
                appendLineToSharedFile(subFolderPath, line);
            }
        }
    }

    closedir(dir);
}

/**
 * Rename file and folder
 * over data connection
 */
void ftserve_rename(int sock_control, int sock_data, char *arg)
{
    char *from, *to;
    if (splitString(arg, &from, &to) == 0)
    {
        // 444 Directory must not contain ".."
        if (containsDoubleDot(from) || containsDoubleDot(to))
        {
            send_response(sock_control, 444);
            free(from);
            free(to);
            return;
        }
        if (renameFile(from, to) == 0)
        {
            send_response(sock_control, 251);
            // LOG
            char logstr[MAX_SIZE] = "RENAME ";
            strcat(logstr, from);
            strcat(logstr, " ");
            strcat(logstr, to);
            log(logstr);
        }
        else
            send_response(sock_control, 451);
        free(from);
        free(to);
    }
    else
        send_response(sock_control, 452);
}

/**
 * Delete file and folder
 * over data connection
 */
void ftserve_delete(int sock_control, int sock_data, char *arg)
{
    if (strcmp(arg, ".shared") == 0)
    {
        send_response(sock_control, 453);
        return;
    }
    // 444 Directory must not contain ".."
    if (containsDoubleDot(arg))
    {
        send_response(sock_control, 444);
        return;
    }
    if (deleteFile(arg) == 0)
    {
        send_response(sock_control, 252);
        // LOG
        char logstr[MAX_SIZE] = "DEL ";
        strcat(logstr, arg);
        log(logstr);
    }
    else
        send_response(sock_control, 453);
}

/**
 * Make new directiory
 * over data connection
 */
void ftserve_mkdir(int sock_control, int sock_data, char *arg)
{
    // 444 Directory must not contain ".."
    if (containsDoubleDot(arg))
    {
        send_response(sock_control, 444);
        return;
    }
    if (strcmp(arg, ".shared") == 0)
    {
        send_response(sock_control, 456);
        return;
    }
    if (createDirectory(arg) == 0)
    {
        send_response(sock_control, 254);
        // LOG
        char logstr[MAX_SIZE] = "MKDIR ";
        strcat(logstr, arg);
        log(logstr);
    }
    else
        send_response(sock_control, 456);
}

/**
 * Move file and folder
 * over data connection
 */
void ftserve_move(int sock_control, int sock_data, char *arg)
{
    char *from, *to;
    if (splitString(arg, &from, &to) == 0)
    {
        // 444 Directory must not contain ".."
        if (containsDoubleDot(from))
        {
            send_response(sock_control, 444);
            free(from);
            free(to);
            return;
        }
        if (isFile(from))
            if (copyOrMoveFile(from, to, 1) == 0)
            {
                send_response(sock_control, 253);
                // LOG
                char logstr[MAX_SIZE] = "MOVE ";
                strcat(logstr, from);
                strcat(logstr, " ");
                strcat(logstr, to);
                log(logstr);
            }
            else
                send_response(sock_control, 454);
        else if (isDirectory(from))
        {
            strcat(to, "/");
            strcat(to, from);
            if (moveDirectory(from, to) == 0)
            {
                send_response(sock_control, 253);
                // LOG
                char logstr[MAX_SIZE] = "MOVE ";
                strcat(logstr, from);
                strcat(logstr, " ");
                strcat(logstr, to);
                log(logstr);
            }
            else
                send_response(sock_control, 454);
        }
        else
            send_response(sock_control, 454);
        free(from);
        free(to);
    }
    else
        send_response(sock_control, 455);
}

/**
 * Copy file and folder
 * over data connection
 */
void ftserve_copy(int sock_control, int sock_data, char *arg)
{
    char *from, *to;
    if (splitString(arg, &from, &to) == 0)
    {
        // 444 Directory must not contain ".."
        if (containsDoubleDot(from) || containsDoubleDot(to))
        {
            send_response(sock_control, 444);
            free(from);
            free(to);
            return;
        }
        if (isFile(from))
            if (copyOrMoveFile(from, to, 0) == 0)
            {
                send_response(sock_control, 253);
                // LOG
                char logstr[MAX_SIZE] = "COPY ";
                strcat(logstr, from);
                strcat(logstr, " ");
                strcat(logstr, to);
                log(logstr);
            }
            else
                send_response(sock_control, 454);
        else if (isDirectory(from))
        {
            strcat(to, "/");
            strcat(to, from);
            if (copyDirectory(from, to) == 0)
            {
                send_response(sock_control, 253);
                // LOG
                char logstr[MAX_SIZE] = "COPY ";
                strcat(logstr, from);
                strcat(logstr, " ");
                strcat(logstr, to);
                log(logstr);
            }
            else
                send_response(sock_control, 454);
        }
        else
            send_response(sock_control, 454);
        free(from);
        free(to);
    }
    else
    {
        send_response(sock_control, 455);
        free(from);
        free(to);
    }
}

/**
 * Share file with other users
 */
void ftserve_share(int sock_control, int sock_data, char *arg, char *cur_user)
{
    char *user, *dir;
    char shared_dir[MAX_SIZE] = "";
    strcat(shared_dir, root_dir);
    strcat(shared_dir, "/user/");
    char file_dir[MAX_SIZE];
    getcwd(file_dir, sizeof(file_dir));
    if (splitString(arg, &user, &dir) == 0)
    {
        // 444 Directory must not contain ".."
        if (containsDoubleDot(dir))
        {
            send_response(sock_control, 444);
            free(user);
            free(dir);
            return;
        }
        // 464 Must not share to yourself
        if (strcmp(user, cur_user) == 0)
        {
            send_response(sock_control, 464);
            free(user);
            free(dir);
            return;
        }
        strcat(file_dir, "/");
        strcat(file_dir, dir);
        // 463 File not found
        if (!isFile(file_dir) && !isDirectory(file_dir))
        {
            send_response(sock_control, 463);
            free(user);
            free(dir);
            return;
        }

        strcpy(file_dir, file_dir + strlen(root_dir));
        // If share to all user
        if (strcmp(user, "everyone") == 0 || strcmp(user, "all") == 0)
        {
            processUserFolder(shared_dir, cur_user, file_dir);
        }
        else // Share to specific user
        {
            FILE *shared;
            strcat(shared_dir, user);
            strcat(shared_dir, "/.shared");
            shared = fopen(shared_dir, "a");
            // 462 User not found
            if (!shared)
            {
                send_response(sock_control, 462);
                free(user);
                free(dir);
                return;
            }
            fprintf(shared, "%s\n", file_dir);
            fclose(shared);
        }
        // 261 Shared successfully
        send_response(sock_control, 261);
        // LOG
        char logstr[MAX_SIZE] = "";
        strcat(logstr, cur_user);
        strcat(logstr, " SHARE ");
        strcat(logstr, user);
        strcat(logstr, " ");
        strcat(logstr, dir);
        log(logstr);

        free(user);
        free(dir);
    }
    else
        send_response(sock_control, 461);
}