#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#define BufferLength 100
#define SERVPORT 3111
 
int main()
{
	int sd, sd2, rc, length = sizeof(int);
	int totalcnt = 0, on = 1;
	char temp;
	char buffer[BufferLength];
	struct sockaddr_in serveraddr;
	struct sockaddr_in their_addr;
 
	fd_set read_fd;
	struct timeval timeout;
	timeout.tv_sec = 15;
	timeout.tv_usec = 0;
 
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Server-socket() error");
		exit (-1);
	}
	else
		printf("Server-socket() is OK\n");
 
	if((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
	{
		perror("Server-setsockopt() error");
		close(sd);
		exit (-1);
	}
	else
		printf("Server-setsockopt() is OK\n");
 
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
	printf("Using %s, listening at %d\n", inet_ntoa(serveraddr.sin_addr), SERVPORT);
 
	if((rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		perror("Server-bind() error");
		close(sd);
		exit(-1);
	}
	else
    		printf("Server-bind() is OK\n");
	if((rc = listen(sd, 10)) < 0)
	{
    		perror("Server-listen() error");
    		close(sd);
    		exit (-1);
	}
	else
    		printf("Server-Ready for client connection...\n");
 
	int sin_size = sizeof(struct sockaddr_in);
	if((sd2 = accept(sd, (struct sockaddr *)&their_addr, &sin_size)) < 0)
	{
    		perror("Server-accept() error");
    		close(sd);
    		exit (-1);
	}
	else
    		printf("Server-accept() is OK\n");
 
	printf("Server-new socket, sd2 is OK...\n");
	printf("Got connection from the client: %s\n", inet_ntoa(their_addr.sin_addr));

	FD_ZERO(&read_fd);
	FD_SET(sd2, &read_fd);
	rc = select(sd2+1, &read_fd, NULL, NULL, &timeout);
	if((rc == 1) && (FD_ISSET(sd2, &read_fd)))
	{
		totalcnt = 0;
 
		while(totalcnt < BufferLength)
		{
			rc = read(sd2, &buffer[totalcnt], 100);//чтение имени файла
			if(rc < 0)
			{
    				perror("Server-read() error");
    				close(sd);
    				close(sd2);
    				exit (-1);
			}
			else if (rc == 0)
				{
    					printf("Client program has issued a close()\n");
    					close(sd);
    					close(sd2);
    					exit(-1);
				}
				else
				{
    					totalcnt += rc;
    					printf("Server-read() is OK\n");

					printf("%s\n", buffer);
					FILE *f;
					if (f=fopen(buffer,"rb")) 
					{
						printf("Файл открылся\n");


						rc = write(sd2, buffer, totalcnt);//передаём клиенту имя файла
						if(rc != totalcnt)
						{
							perror("Server-write() error");
							rc = getsockopt(sd2, SOL_SOCKET, SO_ERROR, &temp, &length);
							if(rc == 0)
							{
    								errno = temp;
    								perror("SO_ERROR was: ");
							}
							else
    								printf("Server-write() is OK\n");
 
								close(sd);
						}
						char buffer2[512];//буффер для пересылки информации
						while(rc)
						{

							rc=fread(buffer2, 512, 1, f);//чтение файла
							write(sd2, buffer2, 512);//и засыл клиенту
						}
						fclose(f);
					}
					else printf("Нет такого файла\n");
				}
 
		}
	}
	else if (rc < 0)
		{
    			perror("Server-select() error");
    			close(sd);
    			close(sd2);
    			exit(-1);
		}
		else
		{
    			printf("Server-select() timed out.\n");
    			close(sd);
    			close(sd2);
    			exit(-1);
		}
	close(sd2);
	close(sd);
	exit(0);
	return 0;
}
