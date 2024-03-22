#define _GNU_SOURCE
#define _POSIX_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


//#define PORT 3402


void sigchild_handler(int s) {
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);
		
	errno = saved_errno;
}

void handle_request(int nfd)
{
   FILE *network = fdopen(nfd, "r+");
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   while ((num = getline(&line, &size, network)) >= 0)
   {
	write(nfd, line, num);
      //printf("%s", line);
   }

   free(line);
   fclose(network);
}
void run_service(int fd)
{
   while (1)
   { 


      int nfd = accept_connection(fd);
      if (nfd != -1)
      {

	pid_t pid = fork();

	if(pid == 0) {
		printf("Connection established\n");
		handle_request(nfd);
		printf("Connection closed\n");
		exit(0);
	} else if (pid > 0){


		close(nfd);
	} else {
		perror("fork failed");
	}
         //printf("Connection established\n");
         //handle_request(nfd);
         //printf("Connection closed\n");
      }
   }
}

int main(int argc, char *argv[])
{

	if (argc != 2) {
		perror("Include Port\n");
		exit(1);
	}

   int port = atoi(argv[1]);


   if (port < 1024 || port > 65535) {
	perror("Port must be between 1024 and 65535\n");
	exit(1);
   }







   int fd = create_service(port);
   struct sigaction sa;


   if (fd == -1)
   {
      perror(0);
      exit(1);
   }


   sa.sa_handler = sigchild_handler;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;

   if(sigaction(SIGCHLD, &sa, NULL) == -1) {
	perror("sigaction");
	close(fd);
	exit(1);
}

   printf("listening on port: %d\n", port);
   run_service(fd);
   close(fd);

   return 0;
}
