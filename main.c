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

void ExecuteCommands(struct Command *command)
{
/*
 * Will likely try to make this a recursive solution
 */
	if(command->num_sub_commands > 1) // i.e. if there is a pipe
	{
		int pipes[(command->num_sub_commands - 1)];
		int fds[2];
	}

	int i = command->num_sub_commands;
	int j = 0;

	for (i = command->num_sub_commands; i >= 0; i--)
	{
		execvp(command->sub_commands[i].argv[0], command->sub_commands[i].argv);
	}
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
		PrintCommand(&command);
		ExecuteCommands(&command);
	}
}




