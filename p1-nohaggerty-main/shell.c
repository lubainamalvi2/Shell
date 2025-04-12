#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "tokenize.h"

int is_cmd(char **args, char *cmd);
void exec_ext(char **args);
void exec_cd(char **args);
void exec_source(char **args);
void exec_help(char **args);
void exec(char **args);
void semicolon_handler(char **args);
void pipe_handler(char **args);
void carrot_handler(char **args);

char **get_args(char **args, int start, int stop){
  char **new_args = malloc(sizeof(char*) * (stop-start + 1));
   
  for(int i = start; i < stop; i++){
    char *temp_str = malloc(sizeof(char) * (strlen(args[i]) + 1));
    strcpy(temp_str, args[i]);
    new_args[i-start] = temp_str;
  }
  new_args[stop-start] = NULL;  
  return new_args;
}

// seperates list of arguments by semicolon and executes each independently
void semicolon_handler(char **args){
  int start = 0;
  int stop = 0;
  while(args[stop] != NULL){
    if(strcmp(args[stop], ";") == 0){
      
      // save current input and output files, let piping and redirection change them as needed
      int saved_in = dup(0);
      int saved_out = dup(1);

      char** temp_args = get_args(args, start, stop);
      pipe_handler(temp_args);
      free_args(temp_args);
      
      // reset standard input and output
      dup2(saved_in, 0);
      dup2(saved_out, 1);
      // clear possible EOF errors lurking in the file descriptor
      clearerr(stdin);
      clearerr(stdout);

      start = stop + 1;
    }
    stop++;
  }
  // repeat process for the last segment
  int saved_in = dup(0);
  int saved_out = dup(1);

  char** temp_args = get_args(args, start, stop);
  pipe_handler(temp_args);
  free_args(temp_args);

  dup2(saved_in, 0);
  dup2(saved_out, 1);
  clearerr(stdin);
  clearerr(stdout);
}

// handles all piping in a list of arguments, assumes no semicolons
void pipe_handler(char **args){
  int start = 0;
  int stop = 0;
  while(args[stop] != NULL){
    if(strcmp(args[stop], "|") == 0){
      pid_t pid;
      
      // create pipe
      int pipe_fd[2];
      pipe(pipe_fd);

      pid = fork();

      if (pid == 0){ // if child, close the read side of pipe
        close(1);
	dup(pipe_fd[1]);
	close(pipe_fd[0]);
        
	char** temp_args = get_args(args, start, stop);
	carrot_handler(temp_args);
	free_args(temp_args);
	exit(0);
      } else { // if parent, close the write side of pipe
        close(0);
	dup(pipe_fd[0]);
	close(pipe_fd[1]);
	start = stop + 1;
      }
    }
    stop++;
  }
  char** temp_args = get_args(args, start, stop);
  carrot_handler(temp_args);
  free_args(temp_args);
}

// handles redirections with < and >, assumes no semicolons or piping
void carrot_handler(char **args){
  int i = 0;
  int stop = 0;
  while(args[i] != NULL){ 
    if(strcmp(args[i], "<") == 0){ 
      close(0); //close stdin
      open(args[i + 1], O_RDONLY);
      i = i + 2;
    } else if (strcmp(args[i], ">") == 0){
      close(1);
      open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666); //0666 allows users to access file after created
      i = i + 2;
    } else {
      i++;
      stop++;
    }
  }
  char** temp_args = get_args(args, 0, stop);
  exec(temp_args);
  free_args(temp_args);
}	

int main(int argc, char **argv) {
  printf("%s\n", "Welcome to mini-shell.");
  char prev[256];
  char user_text[256];
  strcpy(user_text, "");
  while(1){
    char *res = "";  
   
    printf("%s", "shell $ ");
    // saving last command 
    strcpy(prev, user_text);
    // reading command
    res = fgets(user_text, 256, stdin);    

    //checking EOF
    if (res == NULL) {
      printf("%s", "\nBye bye.\n");
      break;
    }

    char **args = tokenize(user_text);
    
    if(is_cmd(args, "exit")){
      printf("%s", "Bye bye.\n");
      free_args(args);
      break;
    }
    else if(is_cmd(args, "prev")){
      strcpy(user_text, prev);
      args = tokenize(user_text);
    }
    semicolon_handler(args); // handle command --- semicolons first, then piping, then redirection
    free_args(args);
  }
  return 0;
}

// checks if command is builtin, then checks system
void exec(char **args){
  if(is_cmd(args, "cd")){
    exec_cd(args);
  } else if(is_cmd(args, "source")){
    exec_source(args);
  } else if(is_cmd(args, "help")){
    exec_help(args);	    
  } else {
    exec_ext(args);
  }
}

// convenience because strcmp is confusing
int is_cmd(char **args, char *cmd){
 return !strcmp(args[0], cmd);
}

void exec_cd(char **args){
  chdir(args[1]);
}

void exec_source(char **args){
    
  int stdin_copy = dup(0);
  close(0); //close stdin
  open(args[1], O_RDONLY); //open read file in descriptor 0

  char text[256];
  while(fgets(text, 256, stdin) != NULL){
    char **args = tokenize(text);
    exec(args);
    free_args(args);
  }
  
  //restore stdin file descriptor
  close(0);
  dup2(stdin_copy, 0);
  close(stdin_copy);
  clearerr(stdin); // remove and EOF errors lurking in file descriptor
}

void exec_help(char **args){
  printf("cd:\nChange the current working directory of the shell to the path specified as the argument\n");
  printf("source:\nTakes a filename as an argument and processes each line of the file as a command, including built-ins\n");
  printf("prev:\nPrints the previous command line and executes it again, without becoming the new command line\n");
  printf("help:\nExplains all the built-in commands available in the shell\n");
}

//execute a system command
void exec_ext(char **args){ 
  char* myargv[256];
  char temp_str[256];
  strcpy(temp_str, "/bin/");
  strcat(temp_str, args[0]);
  myargv[0] = temp_str;
 
  int i = 1;
  while(args[i] != NULL){
    myargv[i] = args[i];
    i++;
  }
  myargv[i] = NULL;
 
  //create child
  pid_t pid = fork();

  if(pid == 0){
    int ret = execve(myargv[0], myargv, NULL);
    if (ret == -1){
      printf("%s: command not found\n", args[0]);
      exit(0);
    }
    exit(1);
  } else {
    waitpid(pid, NULL, 0); //wait for child to exit
  }
}
