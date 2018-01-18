#include "logger.h"
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//writes string str into log file with a timestamp
int writelog(char* str){

/* Open a log file*/
        int fdlog;
        if((fdlog = open("./log", O_WRONLY | O_CREAT | O_APPEND, S_IRWXU)) < 0){
                perror("opening log-file failed\n");
                return -1;
        }

/*Set up file lock*/
        int status;
        struct flock fl;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        fl.l_type = F_WRLCK;

/*Lock log file */
        status = fcntl(fdlog, F_SETLKW, &fl);
	if(status == -1){
		perror("fcntl");
		exit(-1);
	}

/*star writing */
        const time_t caltime = time(NULL);
        char* timestamp = ctime(&caltime);
	timestamp[strlen(timestamp) -1] = ' ';
        if((write(fdlog, timestamp , strlen(timestamp)))!=strlen(timestamp)) //Writes the timestamp
        {
                perror("writing timestamp failed");
                return -1;
        }
        if((write(fdlog, str, strlen(str))) != strlen(str)) //Writes the string
        {
                perror("writing log failed");
                return -1;
        }
       if(write(fdlog, "\n", 1) != 1) //Writes newline char
        {
                perror("writing newline failed");
                return -1;
        }

/*Removes file lock and closes the file*/
        fl.l_type = F_UNLCK;
        status = fcntl(fdlog, F_SETLKW, &fl);
	if(status == -1){
		perror("fcntl");
		exit(-1);
	}
	 close(fdlog);
        return 0;
}

