#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <getopt.h>

static const char *setpid_version_string = "0.0.5";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"command", required_argument, NULL, 'C'},
	{"count", required_argument, NULL, 'c'},
	{"debug", no_argument, NULL, 'D'},
	{"pid", required_argument, NULL, 'p'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVC:c:Dp:";

unsigned int count = 10;
unsigned int debug;
char *command;
pid_t pid;

void ShowHelp(void) {
	printf("setpid options:\n"
		"\t-h, --help\n"
		"\t-V, --version\n"
		"\t-C, --command\n"
		"\t-c, --count NUM\n"
		"\t-D, --debug\n"
		"\t-p, --pid\n");
}

int main(int argc, char **argv) {
	if (argc == 1) return 0;
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
		case 'D':
			debug = 1;
			break;
		case 'p':
			pid = (pid_t)atoi(optarg);
			break;
		default:
			fprintf(stderr, "setpid error: Unknown option: %d<%c>\n", c, (char)c);
			break;
		}
	}

	if (command) {
		if (!pid) {
			fprintf(stderr, "setpid error: pid missing, use -p/--pid\n");
			exit(0);
		}
		unsigned int cnt = 0;
		pid_t current_pid = getpid();
		if (current_pid > pid) {
			while (1) {
				current_pid = fork();
				if (current_pid == -1) {
					if (debug)
						printf("#%u pid == -1\n", cnt++);
					break;
				}
				else if (current_pid != 0)
					kill(current_pid, SIGTERM);

				if (debug)
					printf("#%u pid: %d\n", cnt++, current_pid);

				if (current_pid < pid)
					break;
			}
		}
		while (1) {
			current_pid = fork();
			if (current_pid == -1) {
				if (debug)
					printf("#%u pid2 == -1\n", cnt++);
				break;
			}
			else if (current_pid != 0)
				kill(current_pid, SIGKILL);

			if (debug)
				printf("#%u pid2: %d\n", cnt++, current_pid);

			if (current_pid >= pid-1) {
				system(command);
				break;
			}
		}
		exit(0);
	}

	if (debug)
		printf("#0: %d\n", getpid());
	unsigned int cnt;
	for (cnt = 1; cnt < count; cnt++) {
		pid_t pid = fork();
//		usleep(50000);
		if (pid == -1) {
			fprintf(stderr, "#%u setpid error: Cannot fork(): %s\n", cnt, strerror(errno));
		}
		else if (pid > 1) {
			if (debug)
				printf("#%u %d\n", cnt, pid);
			kill(pid, SIGTERM);
		}
		else {
			if (debug)
				fprintf(stderr, "#%u pid == %d!\n", cnt, pid);
		}
	}

	return 0;
}

