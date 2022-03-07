/*---------------------------------------------------------------------------
  This prints out argv and env variables 
  student copy
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *env[]) {
	printf("args:\n ");
	for (int i = 0; i < argc; i++)
	{
		printf("%s",argv[i]);
	}
	printf("enviornment vars: ");
	int i;
	while (env[i]!=0)
	{
		printf("%s\n",env[i]);
		i++;
	}
	
	
}
