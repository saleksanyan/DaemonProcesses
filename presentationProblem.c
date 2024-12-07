#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char *argv[]) {
    // Fork the process to create a child process
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE); // If fork fails, exit
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); // Parent process exits
    }

    // Child process continues

    // Create a new session and detach from terminal
    if (setsid() < 0) {
        exit(EXIT_FAILURE); // If setsid fails, exit
    }

    // Fork again to prevent acquiring a terminal
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE); // If fork fails, exit
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); // Parent process exits
    }

    // Now, the process is fully detached and can run as a daemon

    // Get the directory of the executable
    char exePath[1024];
    if (realpath(argv[0], exePath) == NULL) {
        perror("Error getting executable path");
        exit(EXIT_FAILURE);
    }

    // Extract the directory from the path
    char *dir = dirname(exePath);

    // Create the log file path (in the same directory as the executable)
    char logFilePath[1024];
    snprintf(logFilePath, sizeof(logFilePath), "%s/daemon_log.txt", dir);

    // Open log file to write current time
    FILE *logFile = fopen(logFilePath, "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE); // If opening file fails, exit
    }

    // Main loop: Write current time to log every 5 seconds
    while (1) {
        time_t now = time(NULL);
        char *timeStr = ctime(&now);
        if (timeStr == NULL) {
            perror("Error getting current time");
            exit(EXIT_FAILURE);
        }
        fprintf(logFile, "Daemon running at %s", timeStr);
        fflush(logFile); // Flush output to file
        fsync(fileno(logFile)); // Ensure it is written to disk
        sleep(5); // Sleep for 5 seconds
    }

    fclose(logFile); // Close the log file (this won't be reached)
    return 0;
}

