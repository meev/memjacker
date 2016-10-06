#include <stdio.h>
#include <stdlib.h>

void listen_on_me(char *username, char*password)
{
	printf("Original function done..\n");
}

int main(int argc, char *argv[], char *envp[])
{
	listen_on_me("nice name", "secret123");
	return 0;
}
