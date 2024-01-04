void zipFolder(char *folderPath, char *zipPath, int isShare)
{
    char command[1000];
    if (isShare)
    {
        char tempZip[500];
        strcpy(tempZip, extractUsername(zipPath));
        sprintf(command, "cd %s && zip -r -y %s/../%s .", folderPath, folderPath, tempZip);
    }
    else
        sprintf(command, "cd %s && zip -r -y ../%s .", folderPath, zipPath);

    int result = system(command);

    if (result == 0)
    {
        printf("Folder zipped successfully.\n");
    }
    else
    {
        printf("Error zipping folder.\n");
    }
}

// Function to unzip a folder
void unzipFolder(const char *zipPath, const char *extractPath)
{
    char command[1000];
    sprintf(command, "unzip -o %s -d %s", zipPath, extractPath);

    int result = system(command);

    if (result == 0)
    {
        printf("Folder unzipped successfully.\n");
    }
    else
    {
        printf("Error unzipping folder.\n");
    }
}