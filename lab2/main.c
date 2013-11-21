#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

void main(int argc, char *argv[])
{
	if(!argv[1])
	{
		puts("Error! ");
		exit(0);
	}

	int port;
	int cSocket, lSocket;
	struct sockaddr_in sAddr;
	struct sockaddr_in cAddr;
	socklen_t addr_size = sizeof(struct sockaddr);
	char *p;
	char *msg1, *msg2;
	time_t timer;

	const long res=strtol(argv[1], &p, 10);
	if(p!=strchr(argv[1], '\0'))
	{
		puts("Error port!");
		exit(0);
	}
	port=atoi(argv[1]);

	lSocket=socket(AF_INET, SOCK_STREAM, 0);
	if(!lSocket)
	{
		puts("Socket create Error!");
		exit(0);
	}

	sAddr.sin_family=AF_INET;
	sAddr.sin_port=htons(port);
	sAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(lSocket, (struct sockaddr *)&sAddr, sizeof(sAddr))<0)
	{
		puts("bind Error!");
		exit(0);
	}

	listen(lSocket, 10);
	while(1)
	{
		cSocket=accept(lSocket, &cAddr, &addr_size);
		if(!cSocket)
		{
			puts("accept Error!");
			exit(0);
		}

		msg1="msg from server";
		//timer=time(NULL);
 		//*msg2=ctime(&timer);

 		write(cSocket, msg1, strlen(msg1));
 		//write(cSocket, msg2, strlen(msg2));
 		write(cSocket, "\n\n", 2);
		close(cSocket);
	}
	close(lSocket);
	puts("done!");
}
