#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINE 80 /* The maximum length command */

int main(void) {
	char * args[MAXLINE / 2 + 1]; /* command line with max 40 arguments */
	int should_run = 1; /* flag to determine when to exit program */
	printf("CS149 Shell from Lovejit Kharod\n");

	while (should_run) {
		for (int i = 0; i < MAXLINE / 2 + 1; i++) /* initialize args */
			args[i] = NULL;
		printf("Lovejit-496>"); /* prompt */
		fflush(stdout);
		char * command = malloc(sizeof(char) * MAXLINE); /* create and alloc pointer to string command to hold user input */
		fgets(command, MAXLINE, stdin); /* take user input with max length of MAXLINE */
		if (strncmp(command, "exit", 4) == 0) /* exit condition checck */
			exit(0);

		strtok(command, "\n"); /* sanitize user input and avoid excess data after input */
		char * token = strtok(command, " "); /* initialize strtok() with user input string */
		int count = 0;
		while (token != NULL) { /* repeat strtok() with command until no more tokens */
			args[count] = token;
			token = strtok(NULL, " ");
			++count;
		}
		--count; /* decrement count to be used as last token index in args later */
		int background = 0;
		if (*args[count] == '&') { /* check for &, if exists remove from args and set background to 1 */
			args[count] = NULL;
			background = 1;
		}

		int pid = fork();
		if (pid == -1) { /* Fork failure */
			printf("Fork attempt failed\n");
		} else if (pid == 0) { /* Child */
			if (execvp(args[0], args) < 0) { /* execvp failure */
				printf("Invalid command\n");
				exit(-1);
			}
		} else { /* Parent */
			if (!background)
				wait(NULL);
		}
		free(command);
	}
	return 0;
}
