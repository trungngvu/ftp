void log(char *cmd)
{
    // Log
    char logDir[MAX_SIZE];
    strcpy(logDir, root_dir);
    strcat(logDir, "/.log");
    FILE *log = fopen(logDir, "a");
    if (log == NULL)
    {
        perror("Error opening log file");
        return;
    }
    // Get current timestamp
    time_t t = time(NULL);
    // Convert to local time
    struct tm *local_time = localtime(&t);
    // Format the timestamp
    char timestamp[20]; // Adjust the size based on your needs
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    // Print the timestamp in a human-readable format
    fprintf(log, "%s %s\n", timestamp, cmd);
    fclose(log);
}