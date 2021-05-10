#include <stdio.h>
#include <unistd.h>
#include <limits.h> // for PATH_MAX
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
	char *stdin_redirect;
	char *stdout_redirect;
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
		printf("\targv[%d] = %s\n", i, argv[i]);
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


//************HOMEWORK 3***********************************************
//************************HOMEWORK 4***********************************

void ReadRedirectsAndBackground(struct Command *command)
{
	command->stdin_redirect = "\0";
	command->stdout_redirect = "\0"; // initialize stdin_redidrect and stdout_redirect to NULL
	command->background = 0; // initialize background to anything other than 1 (i.e. 0)
	int i, j = 0; // initialize counters
	for(i = command->num_sub_commands - 1; i >= 0; i--)
	{
		for(j = 0; command->sub_commands[i].argv[j] != NULL; j++)
		{
			if(strcmp(command->sub_commands[i].argv[j], "<") == 0)
			{
				command->stdin_redirect = command->sub_commands[i].argv[j + 1]; // expected: file name
				//printf("input redirect file = %s\n", command->stdin_redirect);
				command->sub_commands[i].argv[j] = NULL; // remove symbol from args
			}
			else if(strcmp(command->sub_commands[i].argv[j], ">") == 0)
			{
				command->stdout_redirect = command->sub_commands[i].argv[j + 1]; // expected: file name
				//printf("output redirect file = %s\n", command->stdout_redirect);
				command->sub_commands[i].argv[j] = NULL; // remove symbol from args
			}
			else if(strcmp(command->sub_commands[i].argv[j], "&") == 0)
			{
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
	while(i < command->num_sub_commands)
	{
		printf("Subcommand %d:\n", i + 1);
		PrintArgs(command->sub_commands[i].argv);
		i++;
	}

	printf("\n");

	if(strcmp(command->stdin_redirect, "\0") != 0) // stdin_redirect handler
	{
		printf("Redirect stdin: %s\n", command->stdin_redirect);
	}
	if(strcmp(command->stdout_redirect, "\0") != 0) // stdout_redirect handler
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

void ExecutePiped(struct Command *command)
{
	int fds[2];
	pid_t child1, child2;

	int ret = pipe(fds);
	if (ret < 0)
	{
		perror("pipe");
		exit(1);
	}
	child1 = fork();
	if(child1 < 0)
	{
		perror("child1 fork");
		exit(1);
	}
	else if(child1 == 0) //child 1
	{
		close(fds[0]);
		close(1);
		dup(fds[1]);
		close(fds[1]);

		execvp(command->sub_commands[0].argv[0], command->sub_commands[0].argv);
	}
	else{
		child2 = fork();
		if(child2 < 0)
		{
			perror("child2 fork");
			exit(1);
		}
		else if(child2 == 0) //child2
		{
			close(fds[1]);
			close(0);
			dup(fds[0]);
			close(fds[0]);

			execvp(command->sub_commands[1].argv[0], command->sub_commands[1].argv);
		}
		else
		{
			close(fds[0]);
			close(fds[1]);
			if(command->background == 1)
			{
				printf("[%d]\n", child2);
				return;
			}
			else
			{
				int w1 = wait(NULL);
				int w2 = wait(NULL);
				if(w1 && w2) return;
			}
		}
	}
}

void ExecuteNotPiped(struct Command *command)
{
	pid_t pid;

	pid = fork();
	if(pid < 0)
	{
		perror("fork");
	}
	else if(pid == 0)
	{
		execvp(command->sub_commands[0].argv[0], command->sub_commands[0].argv);
	}
	else
	{
		if(command->background == 1)
		{
			printf("[%d]\n", pid);
			return;
		}
		else
		{
			wait(pid);
			return;
		}
	}
}

void CommandHandler(struct Command *command)
{
	int num_pipes = command->num_sub_commands - 1;
	if (num_pipes > 0)
	{
		//printf("NOTE: execute piped\n");
		ExecutePiped(command);
	}
	else
	{
		//printf("NOTE: execute not piped\n");
		ExecuteNotPiped(command);
	}
	return;
}

void ResetCommand(struct Command *command)
{
	 
}

int main(){	
	
	struct Command command;
	char s[200];
	char *argv[10];
	while(1)
	{
		char cwd[PATH_MAX];
		printf("%s$ ", getcwd(cwd, sizeof(cwd)));
		fgets(s, sizeof s, stdin);
		ReadCommand(s, &command);
		ReadRedirectsAndBackground(&command);
		//PrintCommand(&command);
		CommandHandler(&command);
	}
	return 0;
}




