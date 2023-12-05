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

int deleteFile(const char *filename)
{
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

int splitString(char *input, char **str1, char **str2)
{
    if (input == NULL || strlen(input) == 0)
    {
        // Return error for empty string
        return -1;
    }

    // Find the first occurrence of a space in the string
    char *spacePos = strchr(input, ' ');

    if (spacePos == NULL || *(spacePos + 1) == '\0')
    {
        // Return error if there is no space or there is no character after space
        return -1;
    }

    // Calculate the length of the first substring
    size_t len1 = spacePos - input;

    // Allocate memory for the first substring and copy it
    *str1 = (char *)malloc(len1 + 1);
    if (*str1 == NULL)
    {
        // Return error if memory allocation fails
        return -1;
    }
    strncpy(*str1, input, len1);
    (*str1)[len1] = '\0'; // Null-terminate the string

    // Find the first non-space character after the space
    char *nonSpacePos = spacePos + 1;
    while (*nonSpacePos != '\0' && *nonSpacePos == ' ')
    {
        nonSpacePos++;
    }

    if (*nonSpacePos == '\0')
    {
        // Return error if there are no characters after the space
        free(*str1);
        return -1;
    }

    // Calculate the length of the second substring
    size_t len2 = strlen(nonSpacePos);

    // Allocate memory for the second substring and copy it
    *str2 = (char *)malloc(len2 + 1);
    if (*str2 == NULL)
    {
        // Return error if memory allocation fails
        free(*str1);
        return -1;
    }
    strcpy(*str2, nonSpacePos);

    return 0; // Success
}

// Function to copy or move a file
// Parameters:
//   sourceFilename: path to the source file
//   destinationFilename: path to the destination file
//   mode: 0 for copy, 1 for move
int copyOrMoveFile(char *sourceFilename, char *destinationFilename, int mode)
{
    FILE *sourceFile, *destinationFile;
    char ch;

    // Open the source file for reading
    sourceFile = fopen(sourceFilename, "rb");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        return -1; // Error
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

    // Copy the contents of the source file to the destination file
    while ((ch = fgetc(sourceFile)) != EOF)
    {
        fputc(ch, destinationFile);
    }

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

// Function to check if a path corresponds to a file
int isFile(const char *path)
{
    struct stat pathStat;
    if (stat(path, &pathStat) == 0)
    {
        return S_ISREG(pathStat.st_mode);
    }
    return 0; // Return 0 for error or if the path is not a regular file
}

// Function to check if a path corresponds to a directory (folder)
int isDirectory(const char *path)
{
    struct stat pathStat;
    if (stat(path, &pathStat) == 0)
    {
        return S_ISDIR(pathStat.st_mode);
    }
    return 0; // Return 0 for error or if the path is not a directory
}

// Function to create a directory
int createDirectory(const char *path)
{
    int status = 0;

#if defined(_WIN32) || defined(_WIN64)
    status = _mkdir(path);
#else
    status = mkdir(path, 0755); // 0755 provides read, write, execute permissions for owner, and read, execute permissions for group and others
#endif

    if (status == 0)
    {
        printf("Directory %s created successfully.\n", path);
        return 0; // Success
    }
    else
    {
        perror("Error creating directory");
        return -1; // Error
    }
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
    // strcat(destinationPath, "/");
    // strcat(destinationPath, sourcePath);
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
#if defined(_WIN32) || defined(_WIN64)
        _rmdir(sourcePath);
#else
        rmdir(sourcePath);
#endif

        printf("Directory %s moved to %s successfully.\n", sourcePath, destinationPath);
        return 0; // Success
    }
    else
    {
        printf("Failed to move the directory.\n");
        return -1; // Error
    }
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
        if (renameFile(from, to) == 0)
            send_response(sock_control, 251);
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
    if (deleteFile(arg) == 0)
        send_response(sock_control, 252);
    else
        send_response(sock_control, 453);
}

/**
 * Move file
 * over data connection
 */
void ftserve_moveFile(int sock_control, int sock_data, char *arg)
{
    char *from, *to;
    if (splitString(arg, &from, &to) == 0)
    {
        if (isFile(from))
            if (copyOrMoveFile(from, to, 1) == 0)
                send_response(sock_control, 253);
            else
                send_response(sock_control, 454);
        if (isDirectory(from))
            if (moveDirectory(from, to) == 0)
                send_response(sock_control, 253);
            else
                send_response(sock_control, 454);
        free(from);
        free(to);
    }
    else
        send_response(sock_control, 455);
}

/**
 * Copy file
 * over data connection
 */
void ftserve_copyFile(int sock_control, int sock_data, char *arg)
{
    char *from, *to;
    if (splitString(arg, &from, &to) == 0)
    {
        if (isFile(from))
            if (copyOrMoveFile(from, to, 1) == 0)
                send_response(sock_control, 253);
            else
                send_response(sock_control, 454);
        if (isDirectory(from))
            if (copyDirectory(from, to) == 0)
                send_response(sock_control, 253);
            else
                send_response(sock_control, 454);
        free(from);
        free(to);
    }
    else
        send_response(sock_control, 455);
}