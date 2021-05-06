#include <stdio.h>
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
void PrintCommand(struct Command *command){
	//prints all args for subcommands of the command passed
	//internally call print args
	int i = 0;
	int num = command->num_sub_commands;
	for (i = 0; i < num; i++){
		PrintArgs(command->sub_commands[i].argv);
	}

}


//************HOMEWORK 3***********************************************
//************************HOMEWORK 4***********************************
