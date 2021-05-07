#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SUB_COMMANDS 5
#define MAX_ARGS 10

//****************************HOMEWORK THREEE***********************************
struct SubCommand{
	char *line;
	char *argv[MAX_ARGS];
};

struct Command{
	struct SubCommand sub_commands[MAX_SUB_COMMANDS];
	int num_sub_commands;
	char* stdin_redirect;
	char* stdout_redirect;
	int background;
};

void ReadArgs(char *in, char **argv, int size){
	char *token;
	char *target;
	char *delim = " \n";
	int i = 0;

	//split
	token = strtok(in, delim);

	while (token != NULL && i < size-1){
		target = strdup(token);
		argv[i] = target;
		token = strtok(NULL, delim);
		i++;
	}
	argv[i] = NULL;
}

void PrintArgs(char **argv){
	int i = 0;
	while (argv[i] != NULL){
		printf("argv[%d] = %s\n", i, argv[i]);
		i++;
	}

}


void ReadCommand(char *line, struct Command *command){
	char *token;
	char *target;
	char *delim = "|";

	//line is split into sub-strings using strtok
	//each sub-string is duplicated and stored into the sub-commands line field
	token = strtok(line, delim);
	target = strdup(token);
	command->sub_commands[0].line = target;
	command->num_sub_commands = 1;

	int n = 1;
	while (n < MAX_SUB_COMMANDS){
		token = strtok(NULL, delim);
		if (token == NULL){
			break;
		}
		target = strdup(token);
		command->sub_commands[n].line = target;
		n++;
	}
	command->num_sub_commands = n;

	//all sub-commands are processed
	//their arg field is populated (call ReadArgs)
	int i;
	for (i = 0; i < command->num_sub_commands; i++){
		ReadArgs(command->sub_commands[i].line, command->sub_commands[i].argv, MAX_ARGS);
	}
}

void LS(){

	

}
//************HOMEWORK 3***********************************************
//************************HOMEWORK 4***********************************

void ReadRedirectsAndBackground(struct Command *command)
{
	command->stdin_redirect, command->stdout_redirect = NULL; // initialize stdin_redidrect and stdout_redirect to NULL
	command->background = 0; // initialize background to anything other than 1 (i.e. 0)
	int i, j = 0; // initialize counters
	for(i = command->num_sub_commands - 1; i >= 0; i--)
	{
		//printf("Iteration #%d of outer loop\n", i+1);
		for(j = 0; command->sub_commands[i].argv[j] != NULL; j++)
		{
			//printf("Iteration #%d of inner loop\n", j+1);
			//printf("command->sub_commands[%d].argv[%d] = '%s'\n", i, j, command->sub_commands[i].argv[j]);
			if(strcmp(command->sub_commands[i].argv[j], "<") == 0)
			{
				//printf("FOUND STDIN_REDIRECT @ command->sub_commands[%d].argv[%d]\tNEXT ELEMENT = %s\n", i, j, command->sub_commands[i].argv[j+1]);
				command->stdin_redirect = command->sub_commands[i].argv[j + 1]; // expected: file name
				command->sub_commands[i].argv[j] = NULL; // remove symbol from args
			}
			else if(strcmp(command->sub_commands[i].argv[j], ">") == 0)
			{
				//printf("FOUND STDOUT_RIDRECT @ command->sub_commands[%d].argv[%d]\tNEXT ELEMENT = %s\n", i, j, command->sub_commands[i].argv[j + 1]);
				command->stdout_redirect = command->sub_commands[i].argv[j + 1]; // expected: file name
				command->sub_commands[i].argv[j] = NULL; // remove symbol from args
			}
			else if(strcmp(command->sub_commands[i].argv[j], "&") == 0)
			{
				//printf("FOUND BACKGROUND SYMBOL @ command->sub_commands[%d].argv[%d]\n", i, j);
				command->background = 1; // set background to the "yes" value (i.e. 1)
				command->sub_commands[i].argv[j] = NULL; // remove symbol from args
			}
		}
	}
	return;
}


void PrintCommand(struct Command *command)
{
	int i = 0;
	//printf("command->num_sub_commands = %d\n", num);
	//printf("Enter PrintCommand while\n");
	while(i < command->num_sub_commands)
	{
		printf("Command %d:\n", i + 1);
		PrintArgs(command->sub_commands[i].argv);
		i++;
	}

	// Begin additions for HW4:

	printf("\n");

	if(!(strcmp(command->stdin_redirect, "\0") == 0)) // stdin_redirect handler
	{
		printf("Redirect stdin: %s\n", command->stdin_redirect);
	}
	if(!(strcmp(command->stdout_redirect, "\0") == 0)) // stdout_redirect handler
	{
		printf("Redirect stdout: %s\n", command->stdout_redirect);
	}
	if(command->background == 1) // background handler
	{
		printf("Background: yes\n");
	}
	else // if command->background != 1
	{
		printf("Background: no\n");
	}
}

void *LS_LCommand(int argc, char **argv){
	int ret = fork();
	if (ret < 0){
		fprintf(stderr, "fork failed\n");
		exit(1);

	}

	else if (ret == 0){
		//close default standard output 
		close(1);

		//redirect output 
		int fd = open(argv[3], O_WRONLY | O_CREAT, 0660); 
		if (fd < 0){
			fprintf(stderr, "cannot open %s", argv[3]);
			exit(1);
		}

		//Launch command ls -l
		char *myargs[3];
		myargs[0] = "ls";
		myargs[1] = "-l";
		myargs[2] = NULL;
		execvp(myargs[0], myargs);
		printf("This is unreachable code");
	}
	else{
		int w = wait(NULL);
	}

	return 0; 

}

void *WCComand(int argc, char **argv){
	int ret = fork();
	if (ret < 0){
		fprintf(stderr, "forkfailed\n");
		exit(1);

	}
	else if (ret == 0){
		//close default standard output 
		close(0);

		//Redirect output to argv[3]
		int fd = open(argv[3], O_RDONLY);
		if (fd < 0){
			fprintf(stderr, "cannot open %s", argv[3]);
		}

		//launch command "wc"
		char *myargs[2]; 
		myargs[0] = "wc";
		myargs[1] = NULL; 
		execvp(myargs[0], myargs);
		printf("this is unreachable code");
	}
	else{
		int w = wait(NULL);
	}

	return 0; 

}

int LSLWCCommand(int argc, char *argv){
	//create pipe 
	int fds[2]; 
	int err = pipe(fds);

	if (err == -1){
		perror("pipe");
		return 1;
	}

	//spawn child 
	int ret = fork();
	if (ret < 0){
		perror("fork");
		return 1;
	}
	else if (ret == 0){
		//close
		close(fds[1]);

		//Duplicate read end of pipe in standard input 
		close(0);
		dup(fds[0]);

		//child launches command "wc"
		char *argv[2];
		argv[0] = "wc";
		argv[1] = NULL; 
		execvp(argv[0], argv);
	}

	else{

		//close read end of pipe 
		close(fds[0]);

		//duplicate write end of pipe in standard output 
		close(1);
		dup(fds[1]);

		//Parent launches command "ls -l"
		char *argv[3];
		argv[0] = "ls";
		argv[1] = "-l";
		argv[2] = NULL;
		execvp(argv[0], argv);
	}

	return 0; 

}

void* ShellCommandProcessing(int argc, char **argv, struct *Command command){
	int i; 

	//for loop that reads all the arguments 
	for (i=0; i<argc; i++){
		//ls and ls -l commands 
		if (strcmp(argv[0], "ls")==0)
	
	}


}

//what the shell prints
//infinite while loop with one break condition (if the shell command is 'exit')
int main(){
	struct Command command;
	char s[200];
	char *argv[10];

	fgets(s, sizeof s, stdin);
	s[strlen(s) - 1] = '\0';
	ReadCommand(s, &command);
	ReadRedirectsAndBackground(&command);
	PrintCommand(&command);

	return 0;
	//print $ everysingle time
	while (1){
		printf("$ ");
		//support for for everything else

	}

}


