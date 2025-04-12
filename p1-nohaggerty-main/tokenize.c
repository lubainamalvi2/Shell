#include "tokenize.h"

int main(int argc, char** argv) {
  char str[256];
  fgets(str, 256, stdin); 
  char **tokens = tokenize(str);
  for(int i = 0; tokens[i] != NULL; i++){
    printf("%s\n",tokens[i]);
    free(tokens[i]);
  }
  free(tokens);
  return 0;
}

