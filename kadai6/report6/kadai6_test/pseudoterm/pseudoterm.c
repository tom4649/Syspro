#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <wait.h>
#include <termios.h>
#include <sys/ioctl.h>

#define handle_error(msg) do { fprintf(stderr, msg); exit(1); } while(0)

int main(int argc, char **argv){
	int master_fd, client_fd, i;
	char *pts;
	pid_t shell, checker, pid;
	int status, cstatus = 0;

	if(argc < 3){
		fprintf(stderr, "Usage: %s (shell program) (checker program)\n",
		        argv[0]);
		exit(1);
	}

	master_fd = getpt();
	if(master_fd < 0)
    handle_error("Failed to open ptmx\n");
	if(unlockpt(master_fd) < 0)
    handle_error("Failed to unlock pts\n");
	if(grantpt(master_fd) < 0)
    handle_error("Failed to grant pts\n");

	pts = ptsname(master_fd);
/*	printf("Assigning new pts : %s\n", pts); */
	if((shell = fork()) == 0){
		char *_argv[] = { argv[1], NULL };
		struct termios term;
		struct winsize winsize;
		pid_t target;

		close(master_fd);

		client_fd = open(pts, O_RDWR);

		if(client_fd < 0)
      handle_error("Failed to open slave\n");

		setsid();

		ioctl(client_fd, TIOCSCTTY, 0);

		if(tcsetpgrp(client_fd, getpgid(0)) < 0)
      handle_error("Failed to tcsetpgrp\n");

		dup2(client_fd, 0);
		dup2(client_fd, 1);
		dup2(client_fd, 2);
		close(client_fd);

		tcgetattr(0, &term);
		term.c_iflag |= IXANY | IUTF8 | IMAXBEL;
		tcsetattr(0, TCSANOW, &term);

		winsize.ws_row = 25;
		winsize.ws_col = 80;

		ioctl(0, TIOCSWINSZ, &winsize);

		if((target = fork()) == 0){
			struct sigaction sa;

			setpgid(0, 0);

			memset(&sa, 0, sizeof(sa));
			sa.sa_handler = SIG_IGN;
			sigaction(SIGTTOU, &sa, NULL);

			tcsetpgrp(0, getpid());

			sa.sa_handler = SIG_DFL;
			sigaction(SIGTTOU, &sa, NULL);

			execvp(argv[1], _argv);
			perror("execvp");
			exit(1);
		}else{
			waitpid(target, &status, WUNTRACED);
			if(WIFSTOPPED(status) || WIFSIGNALED(status)){
				exit(1);
			}
			exit(0);
		}
	}else if(shell == -1){
		printf("Failed to exec %s\n", argv[1]);
		exit(1);
	}else if((checker = fork()) == 0){
		char **_argv;

		_argv = malloc(sizeof(char *) * argc);
		for(i = 0; i < argc - 2; i++){
			_argv[i] = argv[2 + i];
		}
		_argv[i] = pts;
		_argv[i + 1] = NULL;

		dup2(master_fd, 0);
		dup2(master_fd, 1);

		execvp(argv[2], _argv);
	}else if(checker == -1){
		kill(shell, SIGKILL);
		printf("Failed to exec %s\n", argv[2]);
		exit(1);
	}else{
		for(i = 0; i < 2; i++){
			if((pid = waitpid(-1, &status, WUNTRACED)) < 0){
				if(errno == EINTR){
					i--;
					continue;
				}
			}
			if(pid == shell){
				// printf("Shell done with %d\n", WEXITSTATUS(status));
				if(WEXITSTATUS(status) != 0){
					// cstatus = (WEXITSTATUS(status) << 1) | 1;
					cstatus = WEXITSTATUS(status);
				}
			}else if(pid == checker){
      /*
				printf("Checker done with %d\n", status);
				if(WEXITSTATUS(status) != 0){
					cstatus |= 2;
				}
        */
			}
		}

		close(master_fd);
	}
	exit(cstatus);
}
