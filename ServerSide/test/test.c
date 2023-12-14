#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to zip a folder
void zipFolder(const char *folderPath, const char *zipPath)
{
    char command[1000];
    sprintf(command, "zip -r %s %s", zipPath, folderPath);

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
    sprintf(command, "unzip %s -d %s", zipPath, extractPath);

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

int main()
{
    const char *sourceFolder = "c";
    const char *zipFile = "c.zip";
    const char *extractFolder = "cc";

    // Zip the folder
    zipFolder(sourceFolder, zipFile);

    // Unzip the folder
    unzipFolder(zipFile, extractFolder);

    return 0;
}
