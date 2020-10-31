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
	if (args[0] == NULL) {   
		printf("something was null\n");
		return 1; 
	} // An empty command was entered.
	else
	{
		int i = 0;
		//printf("%c\n", **(args+i));
		while(args[i] != NULL)
		{	
			
			
			if(**(args + i) == '<')
			{
				args[i] = NULL;
				close(0);
				int f = open((args[i+1]), O_RDONLY);
				int n = dup(f);
				close(n);
			}
			else if(**(args+i) == '>')
			{
				args[i] = NULL;

				close(1);
				int fd = open((args[i+1]), O_WRONLY);
				int newd = dup(fd);
				close(newd);
			}				
			else if(**(args+i) == '|')
			{
				//*(args + i) = NULL;
				int bsize = 1;
				while(args[i + bsize] != NULL)
				{
					bsize++;
				}

				char** A = malloc((i+1) * sizeof(char*));
				char** atemp = A;
				while(atemp < A + i + 1) {
    				*atemp = malloc(sizeof(char)*(i+1));
    				atemp++;
				}
				int j = 0;
				while(j < i)
				{
					*(A + j) = *(args + j);
					//printf("%sA\n", *(A+j));
					j++;
				}
				A[i] = NULL;

				char** B = malloc(bsize * sizeof(char *));
				char** temp = B;
				while(temp < B + bsize) {
    				*temp = malloc(sizeof(char)*(bsize));
    				temp++;
				}
				int k = i+1;
				int bIter = 0;
				while(bIter < bsize - 1)
				{
					*(B + bIter) = *(args + k);
					//printf("%sB\n", *(B + bIter));
					k++;
					bIter++;
				}
				B[bsize - 1] = NULL;

				//printf("%d\n", bsize);
				//printf("%d\n", i+1);

				int p[2];
				pipe(p);
				if(fork() == 0)  // LHS
				{
					close(1);
					dup(p[1]);
					close(p[0]);
					close(p[1]);
					execvp(A[0], A);
					free(A);
				}
				if(fork() == 0) //RHS
				{
					close(0);
					dup(p[0]);
					close(p[0]);
					close(p[1]);
					sh_execute(B);
					free(B);
				}

				close(p[0]);
				close(p[1]);
				wait(NULL);
				wait(NULL);
				//printf("Black");
				exit(EXIT_SUCCESS);

				
			}
			i++;
		}
		return sh_launch(args);
	}
}

int sh_launch(char **args)
{
	int pid = fork();
	if(pid > 0){
		pid = wait(NULL);
	}
	else if(pid == 0){

		execvp(args[0], args);

		return 1;
	} 
	else {
		//printf("fork error\n");
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
