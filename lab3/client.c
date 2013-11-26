#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define BufferLength 100
#define SERVER "The_server_name_or_IP"
#define SERVPORT 3111
 
int main(int argc, char *argv[])
{
	int sd, rc, length = sizeof(int);
	struct sockaddr_in serveraddr;
	char buffer[BufferLength];
	char server[255];
	char temp;
	int totalcnt = 0;
	struct hostent *hostp;
	char *data = (char*)malloc(100*sizeof(char));
 
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Client-socket() error");
		exit(-1);
	}
	else
		printf("Client-socket() OK\n");
	if(argc > 1)
	{
		strcpy(server, argv[1]);
		printf("Connecting to the %s, port %d ...\n", server, SERVPORT);
	}
	else
	strcpy(server, SERVER);
 
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
 
	if((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)
	{
 
		hostp = gethostbyname(server);
		if(hostp == (struct hostent *)NULL)
		{
			printf("HOST NOT FOUND --> ");
			printf("h_errno = %d\n",h_errno);
			printf("---This is a client program---\n");
			printf("Command usage: %s <server name or IP>\n", argv[0]);
			close(sd);
			exit(-1);
		}
		memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
	}
	if((rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		perror("Client-connect() error");
		close(sd);
		exit(-1);
	}
	else
		printf("Connection established...\n");
 
	printf("Посылка имени файла на сервер  %s...\n", server);
	printf("Введите имя файла\n");
	scanf("%s", data);
	rc = write(sd, data, 100);//посыл имени файла на сервер
 
	if(rc < 0)
	{
		perror("Client-write() error");
		rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, &temp, &length);
		if(rc == 0)
		{
			errno = temp;
			perror("SO_ERROR was");
		}
		close(sd);
		exit(-1);
	}
	else
	{
		printf("Client-write() is OK\n");
		printf("String successfully sent lol!\n");
		printf("Waiting the %s to echo back...\n", server);
	}
 
	totalcnt = 0;




	while(totalcnt < BufferLength)
	{
		rc = read(sd, &buffer[totalcnt], BufferLength-totalcnt);//чтение имени файла переданного сервером
		if(rc < 0)
		{
			perror("Client-read() error");
			close(sd);
			exit(-1);
		}
		else if (rc == 0)
			{
				printf("Server program has issued a close()\n");
				close(sd);
				exit(-1);
			}
			else
				totalcnt += rc;
	}
	printf("Client-read() is OK\n");
	printf("Полученное имя файла: %s\n", buffer);

	FILE *f;
	if(f=fopen(buffer, "wb"))
	{
		char buffer2[512];

		do
		{

			rc=read(sd, buffer2, 512);//чтение файла с сервера
			fwrite(buffer2, 512, 1, f);//и запись его в файл
		}while(rc);
		fclose(f);
	}

	close(sd);
	exit(0);
	return 0;
}
