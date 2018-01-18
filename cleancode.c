#include "cleancode.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


/*reads code from file named filename and writes it to a file named filename.clean. */
int cleancode(char* filename){
	ssize_t numread = 0;
	ssize_t numwrite = 0;

	/*Create filename for destination file by adding .clean to sourcefiles name*/	
	char* destfilename = malloc(strlen(filename) + strlen(".clean") +1); 
	destfilename = strcpy(destfilename, filename);
	strcat(destfilename, ".clean");
	
	int comment = 0; //1 if we are inside a comment starting with /* and ending with */. Else 0.

	
	//Open source file stream
	FILE* f = fopen(filename, "r");
	if(f == NULL){
		perror("fopen");
		return -1;
	}
	size_t len = 0;


	/*Allocate buffer with size of sourcefile*/
	char* buffer = NULL;

  	//Open destination file
	int fddest = open(destfilename, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRWXU);
	if(fddest == -1){
		perror("open");
		return -1;
	}
	/*Read source one line at time*/
	while((numread = getline(&buffer, &len, f)) != -1){
	
		
		char* currentchar = buffer;	

		int emptychars = 0; /*number of tabs and spaces in the beginning of the line*/
		int emptyline = 0; /*1 if current line only has space, tab and newline chars and comments*/

		/*Count tabs and spaces in the beginning of the line*/
		while(*(currentchar + emptychars) == ' ' || *(currentchar + emptychars) == '\t'){
			emptychars++;
		}
			
		/*Check if line only has space, tab and newline chars and comments*/
		if((*(currentchar + emptychars) == '/' && *(currentchar + emptychars + 1) == '/') 
		||(*(currentchar + emptychars) == '/' && *(currentchar + emptychars + 1) == '*')
		||(*(currentchar + emptychars) == '\n') || comment == 1){
			currentchar += emptychars;
			emptyline = 1;
		}
		
		/*Check if current line is empty (just spaces, tabs and newlines)*/
		if(*currentchar != '\n'){
		

			/*Start writing one char at time*/
			for(int j = 0; j <(numread); j++){
				/* remove '\0'-chars*/
				if(*currentchar != '\0'){
					
					//Check if comment beginning with '/*' begins here
					if(*currentchar == '/' && *(currentchar + 1) == '*'){
						comment = 1;
					}
					//Check if commen beginning with // begins here	
					if(*currentchar == '/' && *(currentchar + 1) == '/'){
						if(emptyline == 0 && comment == 0){
							numwrite = write(fddest, "\n", 1);
							if(numwrite == -1) return -1;
 						}
						break;
					}
					//Write char
					if(comment == 0){
						numwrite = write(fddest, currentchar, 1);
						if(numwrite == -1) return -1;
					}

					//Check if comment ending witg */ ends here
					if(*currentchar == '*' && *(currentchar + 1) == '/'){
						comment = 0;
						currentchar++;
						/*Check if current line only has comments and tab, 
						space and newline chars and if so, skip next newline char*/
						if(emptyline == 1) currentchar++;
					}
					//go to next char
					currentchar++;
				}

			}
		}
	}
	//free memoryallocations
	free(buffer);
	free(destfilename);
	return 1;

}
