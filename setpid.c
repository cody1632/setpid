#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <getopt.h>

static const char *setpid_version_string = "0.0.9";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"command", required_argument, NULL, 'C'},
	{"count", required_argument, NULL, 'c'},
	{"pid", required_argument, NULL, 'p'},
	{"verbose", no_argument, NULL, 'v'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVC:c:p:v";

unsigned int count = 100;
unsigned int verbose;
char *command;
pid_t pid;

void ShowHelp(void) {
	printf("setpid options:\n"
		"\t-h, --help\n"
		"\t-V, --version\n"
		"\t-C, --command \"COMMAND\"\n"
		"\t-c, --count NUM\n"
		"\t-p, --pid\n"
		"\t-v, --verbose\n");
}

int main(int argc, char **argv) {
	int c;
	while (1) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1) break;
		switch (c) {
		case 'h':
			ShowHelp();
			exit(0);
		case 'V':
			printf("setpid %s\n", setpid_version_string);
			exit(0);
		case 'C':
			command = (char *)malloc(strlen(optarg)+1+2);
			sprintf(command, "%s &", optarg);
			break;
		case 'c':
			count = atoi(optarg);
			break;
		case 'p':
			pid = (pid_t)atoi(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			fprintf(stderr, "setpid error: Unknown option: %d<%c>\n", c, (char)c);
			break;
		}
	}

	if (pid) {
		unsigned int cnt = 0;
		pid_t current_pid = getpid();
		if (current_pid > pid) {
			while (1) {
				current_pid = fork();
				if (current_pid == -1) {
					if (verbose)
						printf("#%u pid == -1\n", cnt++);
					exit(1);
				}
				else if (current_pid != 0)
					kill(current_pid, SIGTERM);

				if (verbose)
					printf("#%u pid: %d\n", cnt++, current_pid);

				if (current_pid < pid)
					break;
			}
		}
		while (1) {
			current_pid = fork();
			if (current_pid == -1) {
				if (verbose)
					printf("#%u pid2 == -1\n", cnt++);
				exit(1);
			}
			else if (current_pid != 0)
				kill(current_pid, SIGKILL);

			if (verbose)
				printf("#%u pid2: %d\n", cnt++, current_pid);

			if (current_pid >= pid-1) {
				if (command)
					system(command);
				break;
			}
		}
		exit(0);
	}
	else {
		pid_t pid = getpid();
		if (verbose)
			printf("#1: %d\n", pid);
		char cmdstr[64];
		unsigned int cnt;
		for (cnt = 1; cnt <= count; cnt++) {
			if (verbose) {
				if (command)
					system(command);
				else {
					sprintf(cmdstr, "echo \"#%u: $$\"", cnt);
					system(cmdstr);
				}
			}
			else
				if (command)
					system(command);
				else
					system("true");
		}
	}

	return 0;
}

