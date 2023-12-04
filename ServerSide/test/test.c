#include <stdio.h>
#include <stdlib.h>

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

int main()
{
    const char *oldFileName = "test.exe";
    const char *newFileName = "renamed_example.txt";

    if (renameFile(oldFileName, newFileName) == 0)
    {
        // File renaming was successful
        // Add your code here to perform further actions if needed
    }
    else
    {
        // Handle the error, if any
        // Add your code here to handle the error case
    }

    return 0;
}
