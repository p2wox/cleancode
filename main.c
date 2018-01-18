#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "cleancode.h"
#include "logger.h"

static volatile sig_atomic_t mysignal = 0;

/*Signal handler*/
static void sig_usr(int signo){
	mysignal = signo;
} 

/*At exit function
Writes error message to log if errno is set*/
void atexitfunc(){
	if(errno != 0){
		char* errtxt = strerror(errno);
		char* errmsg = malloc(strlen("error: ") + strlen(errtxt) + 1);
		strcpy(errmsg, "error: ");	
		strcat(errmsg, errtxt);
		writelog(errmsg);
		writelog("terminating\n");
		free(errmsg);
	}
}

int main(int argc, char **argv) {
/*Set up signalhandler*/
	struct sigaction sig;
	sigemptyset(&sig.sa_mask);
	sig.sa_handler = sig_usr;
	sig.sa_flags = 0;

	int sigtest = sigaction(SIGINT,&sig,NULL);
	if(sigtest == -1){
		perror("sigaction");
		exit(-1);
	}

/*Define atexit function*/
	atexit(atexitfunc);

/*Write start in log*/
	int num = writelog("started program");
	if(num == -1) exit(-1);
			
	pid_t childpid, waitpid;

/*Argument check*/
	if(argc < 2){
		printf("Give at least one filename as argument\n");
		num = writelog("no filename as argument");
		if(num == -1) exit(-1);
	}
	else{

/*Main part of the program*/

		for(int i = 0; i < (argc - 1); i++){
			/*Check for signals*/
			if(mysignal != 0){
				char* logtext = malloc(35);
				sprintf(logtext, "SIGINT when %d children were created", i);
				num = writelog(logtext);
				if(num == -1) exit(-1);
				printf("received SIGINT\n");
				free(logtext);
				exit(-1);
			}

			char* filename;
			childpid = fork();
			/*Error handling*/
			if(childpid == -1){
				perror("fork() failed");
			}
			/*Child process*/
			else if(childpid == 0){
				filename = argv[i+1];

				if(cleancode(filename) == -1){ 
					perror("code cleaning failed");
					exit(-1);
				}
				
				/*Log*/
				num = writelog(strcat(filename, " cleaned"));
				
				if(num == -1) exit(-1);
				exit(0);/*child exits*/
			}
			
							
		}
		/*Wait for children to exit*/
		while((waitpid = wait(NULL)) >0){
		}
		if(errno != ECHILD) perror("wait");
		num = writelog("all childs have exited");
		if(num == -1) exit(-1);
		errno = 0;


	}
	/*Close program*/
	num = writelog("closing program\n");
	if(num == -1) exit(-1);

	exit(0);

}

