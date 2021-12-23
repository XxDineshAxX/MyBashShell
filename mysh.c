#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#define MAXLEN 1000

char command[1000], pathname[1000];
int count = 0;
int tochild[2], toparent[2];
int fds[100][2];
int fdsc = 0;
int linecount = 0;
int opCnt = 0;
char ch;
char *arg[MAXLEN];
int argC = 0;


int quote(){
	execl("./quoteserver", "quoteserver", "-v", NULL);
        fputs("I hope you do not see me!", stderr);
        exit(1);
}

void runCommand(char *input){
	 char *arguments[MAXLEN], line[MAXLEN];

        for(int i=0; input[i]!='\0'; i++)
    {
        line[i] = input[i];
    }

        char *command = strtok(line, " \n");
        arguments[0] = command;

        int i=1;
        //keep getting tokens (individual words)
        while ((arguments[i] = strtok(NULL, " \n")) != NULL)
                i++;

        //printf("Invoking: %s\n", command);
        //printf("Arguments: ");
        for(int j=0; j<i; j++)
        //        printf("%s ", arguments[j]);
        //puts("\nExecuting the command now.\n");

        execvp(command, arguments);
        fputs("I hope you don't see me!\n", stderr);



}

char *commands[100];

int numPipes=1;

int child(int i){
	
	if (i > 0){
	close(0);
	dup(fds[i-1][0]);

	}

	if (i < numPipes){
		close(1);
		dup(fds[i][1]);

	}
	
	if (i > 0){
	close(fds[i-1][0]);
	close(fds[i-1][1]);
	}
	if (i < numPipes){
	close(fds[i][0]);
	close(fds[i][1]);
	}
	runCommand(commands[i]);

}

void processLine(char *line){
	char *pipePtr = strchr(line, '|');
        char *equalPtr = strchr(line, '=');
        if (pipePtr){ //not NULL
                //line has one or more | character

                // cmd1 | cmd2 | cmd3
                //split input line to multiple commands and count n
                commands[0] = strtok(line, "|\n");
                numPipes = 1;
                while (commands[numPipes] = strtok(NULL, "|\n")){
                        numPipes++;
		}
                numPipes--;

                for(int i=0; i<numPipes; i++)
		{
                        pipe(fds[i]);
		}
		
                for(int i=1; i<=numPipes; i++)
         	 {     
			 if (fork() == 0){
                                child(i);
			}
		}
		child(0);
		}
        else if (equalPtr){
                //Example: "computer = user"
                //line has one = character
                char *cmd1 = line;
                *equalPtr = NULL;
                char *cmd2 = equalPtr+1;

                int tocmd1[2], tocmd2[2];
                pipe(tocmd1);
                pipe(tocmd2);
                if (fork() == 0){
                        //replace fd 1 with tocmd1[1]
                        close(1);
                        dup(tocmd1[1]);
                        //replace fd 0 with tocmd2[0]
                        close(0);
                        dup(tocmd2[0]);
                        //take care of command2
                        runCommand(cmd2);
		}
            else
		{
                        //replace fd 0 with tocmd1[0]
			close(0);
			dup(tocmd1[0]);
                        //replace fd 1 with tocmd2[1]
			close(1);
			dup(tocmd2[1]);
               		 //take care of command1
			runCommand(cmd1);
		}
	}   
 else {
                //it is a simple command
                runCommand(line);
}


}


int main(int argc, char *argv[]) {
	while(1){
	
		fdsc = 0;
		
		count = 0;
		if (fork() == 0){
			quote();
		}
		int waiting = 0;
		wait(&waiting);
		printf("#");
		char str[MAXLEN];

   
   		 
   		fgets(str, MAXLEN, stdin);
		

		if (fork() == 0)
		{
			processLine(str);
		}
		int stat = 0;
		wait(&stat);

		}
}
