#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>


char *sh_read_line(void)
{
  char *line = NULL;
  size_t bufsize = 0;  // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin))  // EOF
    {
      fprintf(stderr, "EOF\n");
      exit(EXIT_SUCCESS);
    } else {
      fprintf(stderr, "Value of errno: %d\n", errno);
      exit(EXIT_FAILURE);
    }
  }
  return line;
}


#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"

char **sh_split_line(char *line)
{
  int bufsize = SH_TOK_BUFSIZE;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "sh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        free(tokens_backup);
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int sh_execute(char **args)
{
	if (args[0] == NULL) {   return 1; } // An empty command was entered.
	else
	{
		int i = 0;
		
		while(args[i] != NULL)
		{	
			if(**(args + i) == '<')
			{
				args[i] = NULL;
				close(0);
				int f = open((args[i+1]), O_RDONLY);
				int n = dup(f);
				//close(n);
			}
			else if(**(args+i) == '>')
			{
				args[i] = NULL;

				close(1);
				int fd = open((args[i+1]), O_WRONLY);
				int newd = dup(fd);
				//close(newd);
				break;
			}				
			i++;
		}
		return sh_launch(args);
	}

 // bool symbol = false;
 // bool symbolLt = false;
  //int i = 0;

/*
  if(symbol)
  {
	
		char** new = (char**)malloc(i * sizeof(char*));
		for(int j = 0; j < i; j++)
		{
			new[j] = (char*)malloc(50 * sizeof(char));
			new[j] = args[j];
		}
		close(1);
		int fd = open((args[i+1]), O_WRONLY);
		int newf = dup(fd);
		close(newf);
		return sh_execute(new);
  }
	i = 0;
	while(args[i] != NULL)
	{
		if(**(args + i) == '<')
		{
			symbolLt = true;
			break;
		}
		i++;
	}

  if(symbolLt)
  {
		char** temp = (char**)malloc(i * sizeof(char*));
		for(int j =0; j < i; j++)
		{
			temp[j] = (char*)malloc(50 * sizeof(char));
			temp[j] = args[j];
		}
		close(0);
		int f = open((args[i+1]), O_RDONLY);
		int n = dup(f);
		close(n);
		return sh_execute(temp);
  }*/
 // else
//  	return sh_launch(args);   // launch

}

int sh_launch(char **args)
{
	int pid = fork();
	if(pid > 0){
		pid = wait(NULL);
	}
	else if(pid == 0){

		execvp(args[0], args);

		return 0;
	} 
	else {
		printf("fork error\n");
	}	
}
void sh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("143A$ ");
    line = sh_read_line();
    args = sh_split_line(line);
    status = sh_execute(args);

    free(line);
    free(args);
  } while (status);
}


int main(int argc, char **argv)
{
  sh_loop();
  return EXIT_SUCCESS;
}
