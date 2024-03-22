#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#define PORT 3402

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
         printf("Connection established\n");
         handle_request(nfd);
         printf("Connection closed\n");
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

   if (fd == -1)
   {
      perror("Error creating service\n");
      exit(1);
   }

   printf("listening on port: %d\n", port);
   run_service(fd);
   close(fd);

   return 0;
}
