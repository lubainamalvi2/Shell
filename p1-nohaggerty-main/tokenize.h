#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *substring(char *str, int start, int stop);

int is_special(char c){
  return c == '<' || c == '>' || c == '|' || c  == ';' || c == '(' || c == ')';
}

int is_white(char c){
  return c == ' ' || c == '\n' || c == '\t' || c =='\0';
}


// free list of string
void free_args(char **args){
  for(int i = 0; args[i] != NULL; i++){
    free(args[i]);
  }
  free(args);
}

char **tokenize(char *str){
  char **tokens = malloc(sizeof(char*) * strlen(str)); 
  
  int start = 0;
  int stop = 0;
  int num_tokens = 0;
  int in_string = 0; // 0 not in string, 1 in string, 2 just exited string
  while(1){
    if(str[stop] == '\"'){ // if string, move to next in_string state
      in_string = (in_string + 1) % 3;
    } 
    if (in_string == 2){ // if just exited string, create token but exclude quotations
      tokens[num_tokens] = substring(str, start + 1, stop);
      num_tokens++;
      start = stop + 1;
      in_string = 0;
    } else if (in_string == 0){
      if(is_special(str[stop])){
        if (stop > start){//if special character and has non-special characters before it, tokenize non-special characters
	  tokens[num_tokens] = substring(str, start, stop);
	  num_tokens++;
        }
	tokens[num_tokens] = substring(str, stop, stop + 1); //tokenize special character
	num_tokens++;
	start = stop + 1;
      } else if (is_white(str[stop])){
        if(stop > start){// if white space and have non-special characters before it, tokenize the non-special characters
          tokens[num_tokens] = substring(str, start, stop);
	  num_tokens++;
	}
        start = stop + 1;
      }
    }
    if (str[stop] == '\0'){
      break;
    }
    stop++;
  }
  tokens[num_tokens] = NULL;
  tokens = realloc(tokens, sizeof(char*) * (num_tokens + 1));
  return tokens;
}

// creates a new substring from the start index to the stop index
char *substring(char *str, int start, int stop){
  char *new_str = malloc(sizeof(char) * (stop-start + 1));
  
  for(int i = start; i < stop; i++){
    new_str[i-start] = str[i];
  }
  new_str[stop-start] = '\0';
  return new_str;
}
