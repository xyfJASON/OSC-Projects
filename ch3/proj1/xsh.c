#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LENGTH 80
#define MAX_ARGS 11

char *trim(char *s) {
	char *st = s;
	char *ed = s + strlen(s) - 1;
	while (st <= ed && *st == ' ')	st++;
	while(ed >= st && (*ed == ' ' || *ed == '\n'))	ed--;
	int len = (ed - st) < 0 ? 0 : ed - st + 1;
	st[len] = '\0';
	return st;
}

int update_history(char *command, char **history, int *hisc) {
	strcpy(command, trim(command));
	if (strcmp(command, "!!") == 0) {
		if (*hisc == 0) {
			printf("No commands in history.\n");
			return -1;
		}
		strcpy(command, history[*hisc-1]);
		return 0;
	}
	else if (command[0] == '!') {
		int idx = atoi(command+1);
		if (idx < 0 || idx >= *hisc) {
			printf("No such command in history.\n");
			return -1;
		}
		strcpy(command, history[idx]);
		return 0;
	}
	history[*hisc] = (char *)malloc(MAX_LENGTH);
	strcpy(history[*hisc], command);
	*hisc = *hisc + 1;
	return 0;
}

int parseline(char *command, char **args, int *bg) {
	// parse command line and store into args
	// if runs in background (with &), bg = 1
	char *tmp = (char*)malloc(MAX_LENGTH);
	strcpy(tmp, command);
	int len = strlen(tmp);

	int cnt = 0, argc = 0;
	for (int i = 0; i <= len && argc < MAX_ARGS; i++) {
		if (tmp[i] == ' '|| tmp[i] == '\0') {
			if (i && tmp[i-1] == ' ')	continue;
			tmp[i] = '\0';
			if (strcmp(tmp + i - cnt, "\0") == 0)	continue;
			args[argc] = (char *)malloc(cnt);
			strcpy(args[argc++], tmp+i-cnt);
			cnt = 0;
		}
		else	cnt++;
	}
	free(tmp);
	if (argc == MAX_ARGS) {
		printf("Too many arguments!\n");
		return -1;
	}
	args[argc] = NULL;

	if (argc && strcmp(args[argc-1], "&") == 0) {
		*bg = 1;
		args[--argc] = NULL;
	}
	else	*bg = 0;

	return 0;
}

int builtin_command(char **args, char **history, int *hisc) {
	// if is a builtin command, run it and return 1
	// else return 0
	if (args[0] == NULL) // no command
		return 1;
	else if (strcmp(args[0], "exit") == 0) // exit
		exit(0);
	else if (strcmp(args[0], "history") == 0) { // history
		int tmp = *hisc - 1;
		int lim = args[1] == NULL ? 10 : atoi(args[1]);
		while (tmp >= 0 && *hisc - tmp <= lim) {
			printf("%4d %s\n", tmp, history[tmp]);
			tmp--;
		}
		return 1;
	}
	return 0;
}

void eval(char *command, char **history, int *hisc) {
	// evaluate command
	
	char *args[MAX_ARGS];
	int bg;
	if (update_history(command, history, hisc) < 0)
		return;

	if (parseline(command, args, &bg) < 0)
		return;

	if (builtin_command(args, history, hisc))
		return;

	pid_t pid;
	if ((pid = fork()) == 0) {
		// child process
		if (execvp(args[0], args) < 0)
			printf("%s: command not found.\n", command);
		exit(0);
	}
	else {
		// parent process
		if (!bg) {
			int status;
			if (waitpid(pid, &status, 0) < 0)
				printf("Wait error.\n");
		}
	}
	for(int i = 0; args[i] != NULL; i++)
		free(args[i]);
}

void handler(int signum) {
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0);
}

int main() {
	char command[MAX_LENGTH];
	char *history[1000];
	int hisc = 0;
	signal(SIGCHLD, handler);
	while(1) {
		printf("\033[35mxsh> \033[0m");
		fflush(stdout);

		fgets(command, MAX_LENGTH, stdin);
		if (feof(stdin))	break;
		eval(command, history, &hisc);
	}
	for(int i = 0; history[i] != NULL; i++)
		free(history[i]);
	return 0;
}
