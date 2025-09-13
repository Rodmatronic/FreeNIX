#include "../include/errno.h"
#include "../include/stdio.h"
#include "../include/stat.h"
#include "../include/fcntl.h"

int
main()
{
  for (int i = 0; i < 8; i++){
	printf("\033[3%dm#", i);
  }
  printf("\n");
  for (int i = 0; i < 8; i++){
	printf("\033[4%dm#", i);
  }
  printf("\033[0m\n");
  for (int i = 0; i < 8; i++){
	printf("\033[9%dm#", i);
  }
  printf("\n");
  for (int i = 0; i < 8; i++){
	printf("\033[10%dm#", i);
  }
  printf("\033[0m\n");
  return 0;
}
