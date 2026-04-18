// DART-OS Init-System --------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date: Created: 2026.01.20
//
// Description: See doc/init_system_application.md for more details.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "init_system_stdio.h"
#include "shutdown_interrupt.h"

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// C Standard Library
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a GPIO line string into a GPIO line number.
 * @param str The string to convert.
 * @return The line number, if successful, -1 otherwise.
 */
int getGpioLine (char* str)
{
	char* end;
	unsigned long line = strtoul (str, &end, 0);
	if (end == str)
		return -1;

	if (line > UINT_MAX)
		return -1;

	return line;
}

/**
 * @brief Executes and waits for the termination of a pre-execution / post-execution application.
 * @param application The application to run.
 * @return 0 if successful, the error code otherwise.
 */
int execPrePostExecApplication (char* application)
{
	printf (STDIO_PREFIX "Executing application '%s'...\n", application);

	// Executure the pre-exec application
	pid_t initPid = fork ();
	if (initPid == 0)
	{
		char* argv [] = { application, NULL };
		execvp (application, argv);

		// execvp only returns on failure.
		int code = errno;
		fprintf (stderr, STDIO_PREFIX "Failed to execute application '%s': %s.\n", application, strerror (code));
		return errno;
	}

	waitpid (initPid, NULL, 0);

	return 0;
}

/**
 * @brief Executes an application. Note this does not wait for application termination.
 * @param path The path of the application to execute.
 * @param pid Buffer to write the PID of the application into.
 */
void execApplication (char* path, pid_t* pid)
{
	printf (STDIO_PREFIX "Executing application '%s'.\n", path);
	*pid = fork ();
	if (*pid == 0)
	{
		char* argv [] = { path, NULL };
		execvp (path, argv);
		int code = errno;

		// execvp only returns on failure.
		// - Note: This is in the context of the child application, not the init-system itself, so we cannot allow the
		//   process to keep running.
		fprintf (stderr, STDIO_PREFIX "Failed to execute application '%s': %s.\n", path, strerror (code));
		exit (errno);
	}
}

/**
 * @brief Executes an array of applications. Note this does not wait for application termination.
 * @param applicationPathes The array containing the path of each application to execute.
 * @param applicationPids The array to write to PID of each application into.
 * @param applicationCount The size of @c applicationPathes and @c applicationPids .
 */
void execApplications (char** applicationPathes, pid_t* applicationPids, size_t applicationCount)
{
	for (size_t index = 0; index < applicationCount; ++index)
		execApplication (applicationPathes [index], &applicationPids [index]);
}

/**
 * @brief Checks the status of an application, printing a warning on early termination.
 * @param path The path of the application to check.
 * @param pid The PID of the application to check. Set to 0 if the application has terminated.
 */
void checkApplication (char* path, pid_t* pid)
{
	if (waitpid (*pid, NULL, WNOHANG) != 0)
	{
		fprintf (stderr, STDIO_PREFIX "Warning: Process '%s' terminated early.\n", path);
		*pid = 0;
	}
}

/**
 * @brief Checks the status of an array of applications, printing a warning on an early termination.
 * @param applicationPathes The array containing the path of each application to check.
 * @param applicationPids The array containing the PID of each application to check.
 * @param applicationCount The size of @c applicationPathes and @c applicationPids .
 */
void checkApplications (char** applicationPathes, pid_t* applicationPids, size_t applicationCount)
{
	// Check if each individual application has exited
	for (size_t index = 0; index < applicationCount; ++index)
		checkApplication (applicationPathes [index], &applicationPids [index]);
}

/**
 * @brief Sends the termination signal to an application.
 * @param applicationPath The application path.
 * @param applicationPid The application PID.
 */
void terminateApplication (char* applicationPath, pid_t* applicationPid)
{
	if (applicationPid != 0)
	{
		if (kill (*applicationPid, SIGTERM) != 0)
		{
			int code = errno;
			fprintf (stderr, STDIO_PREFIX "Failed to terminate process '%s': %s.\n", applicationPath, strerror (code));
			errno = 0;
		}
	}
}

/**
 * @brief Sends the kill signal to an application.
 * @param applicationPath The application path.
 * @param applicationPid The application PID.
 */
void killApplication (char* applicationPath, pid_t* applicationPid)
{
	if (applicationPid != 0)
	{
		if (kill (*applicationPid, SIGKILL) != 0)
		{
			int code = errno;
			fprintf (stderr, STDIO_PREFIX "Failed to terminate process '%s': %s.\n", applicationPath, strerror (code));
			errno = 0;
		}
	}
}

/**
 * @brief Waits for an array of applications to terminate
 * @param applicationPathes The array of applications to wait for.
 * @param applicationPids The array of PIDs of each application.
 * @param applicationCount The size of @c applicationPathes and @c applicationPids .
 * @param timeStart The start of the termination sequence.
 */
void waitForApplicationTerminations (char** applicationPathes, pid_t* applicationPids, size_t applicationCount, struct timespec* timeStart)
{
	// Block until all child applications have terminated.
	int pid;
	do
	{
		// Wait for a child application to exit.
		pid = wait (NULL);
		if (pid == -1 && errno == ECHILD)
			break;

		// Print the application that exited and its time.
		struct timespec timeCurrent;
		clock_gettime (CLOCK_MONOTONIC, &timeCurrent);
		float timeDiff = (timeCurrent.tv_sec - timeStart->tv_sec) * 1e3f + (timeCurrent.tv_nsec - timeStart->tv_nsec) * 1e-6f;

		for (size_t index = 0; index < applicationCount; ++index)
			if (applicationPids [index] == pid)
				printf ("Application '%s' terminated in %f ms.\n", applicationPathes [index], timeDiff);
	} while (true);
}

/**
 * @brief Terminates an array of applications.
 * @param applicationPathes The array containing the path of each application to terminate.
 * @param applicationPids The array containing the PID of each application to terminate.
 * @param applicationCount The size of @c applicationPathes and @c applicationPids .
 * @param timeStart The start of the termination sequence.
 */
void terminateApplications (char** applicationPathes, pid_t* applicationPids, size_t applicationCount, struct timespec* timeStart)
{
	// Send the termination signal to each application
	for (size_t index = 0; index < applicationCount; ++index)
		terminateApplication (applicationPathes [index], &applicationPids [index]);

	// Wait for the applications to terminate
	waitForApplicationTerminations (applicationPathes, applicationPids, applicationCount, timeStart);
}

// Entrypoints ----------------------------------------------------------------------------------------------------------------

int main (int argc, char** argv)
{
	// Validate the application usage.
	if (argc != 7)
	{
		fprintf (stderr, "Invalid usage. Usage: init-system <GPIO Chip> <GPIO Line> <Pre-Execution Application> "
			"<Post-Execution Application> <can-mdf-logger path> <dashboard-gui path>\n");
		return -1;
	}

	// Set standard output to unbuffered. Normally, system services write blocks of data to the system journal (~8kB
	// typically). We want to see output immediately, so here we disable this.
	if (setvbuf (stdout, NULL, _IONBF, 0) != 0)
	{
		perror ("Failed to disable standard output buffering");
		return -1;
	}

	// Get the GPIO chip and GPIO line from standard arguments.
	char* gpioChip = argv [1];
	char* gpioLineStr = argv [2];
	int gpioLine = getGpioLine (gpioLineStr);
	if (gpioLine < 0)
	{
		fprintf (stderr, STDIO_PREFIX "Invalid GPIO line '%s'.\n", gpioLineStr);
		return EINVAL;
	}

	// Initialize the shutdown interrupt GPIO. When the device is powering down, this will trigger the init-system to terminate
	// all child applications.
	shutdownInterrupt_t* interrupt = shutdownInterruptInit ("init-system", gpioChip, gpioLine);
	if (interrupt == NULL)
		return errno;

	// Execute the pre-exec application.
	if (execPrePostExecApplication (argv [3]) != 0)
		return errno;

	// Execute the applications.

	#define APPLICATION_COUNT 2
	char** applicationsPathes = &argv [5];
	pid_t applicationPids [APPLICATION_COUNT];

	char* canMdfLoggerPath = applicationsPathes [0];
	pid_t* canMdfLoggerPid = &applicationPids [0];
	execApplication (canMdfLoggerPath, canMdfLoggerPid);

	char* dashboardGuiPath = applicationsPathes [1];
	pid_t* dashboardGuiPid = &applicationPids [1];
	execApplication (dashboardGuiPath, dashboardGuiPid);

	// Wait briefly, then check for any early terminations
	nanosleep (&(struct timespec) { .tv_nsec = 10000000 }, NULL);
	checkApplication (canMdfLoggerPath, canMdfLoggerPid);
	checkApplication (canMdfLoggerPath, canMdfLoggerPid);

	// Wait for the shutdown interrupt to indicate the device is shutting down.
	if (shutdownInterruptPoll (interrupt) != 0)
	{
		perror (STDIO_PREFIX "Failed to poll shutdown interrupt");
		return errno;
	}
	printf (STDIO_PREFIX "Terminating...\n");

	// Time the shutdown sequence
	struct timespec timeStart;
	clock_gettime (CLOCK_MONOTONIC, &timeStart);

	// Terminate all the remaining applications.
	// - Here we terminate the can-mdf-logger because it must stop gracefully, but we kill the dashboard-gui, because it does
	//   not need to stop gracefully.
	terminateApplication (canMdfLoggerPath, canMdfLoggerPid);
	killApplication (dashboardGuiPath, dashboardGuiPid);
	waitForApplicationTerminations (applicationsPathes, applicationPids, APPLICATION_COUNT, &timeStart);

	// Release the shutdown GPIO
	shutdownInterruptDealloc (interrupt);

	// Execute the post-exec application.
	if (execPrePostExecApplication (argv [4]) != 0)
		return errno;

	// Finish timing and print to stdout
	struct timespec timeEnd;
	clock_gettime (CLOCK_MONOTONIC, &timeEnd);
	float timeDiff = (timeEnd.tv_sec - timeStart.tv_sec) * 1e3f + (timeEnd.tv_nsec - timeStart.tv_nsec) * 1e-6f;
	printf (STDIO_PREFIX "All processes terminated in %f ms.\n", timeDiff);

	return 0;
}