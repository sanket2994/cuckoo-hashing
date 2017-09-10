#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include "hash.h"

int main(int argc, char **argv)
{
	int cfd, sfd, ret, portno;
	struct sockaddr_in caddr,saddr;
	socklen_t clen,slen;
	char buff[100], *ip;

	ip=argv[1];
	sscanf(argv[2], "%d", &portno);

	cfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sfd<0)
	{
		perror("socket: ");
		return -1;
	}

	caddr.sin_family=AF_INET;
	caddr.sin_addr.s_addr=inet_addr(ip);
	caddr.sin_port=htons(portno);
	clen=sizeof(caddr);
	slen=sizeof(saddr);

	

	ret=connect(cfd, (struct sockaddr *)&caddr, clen);
	if(ret==-1)
	{
		perror("connect: ");
		return -1;
	}
	printf("Enter the message to be sent: ");
	scanf("%[^\n]s",buff);

	//check if user wants to perform deletion or lookup of a packet
	if(strcmp(buff, "delete")==0 || strcmp(buff, "lookup")==0)
	{
		
		write(cfd, buff, sizeof(buff));
		struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));
		printf("Enter the packet details: \n");
		printf("1. src address: ");
		scanf("%s", buff);
		packet->saddr=inet_addr(buff);
		printf("2. dest address: ");
		scanf("%s", buff);
		packet->daddr=inet_addr(buff);
		printf("3. src port: ");
		scanf("%d", &portno);
		packet->sport=portno;
		printf("4. dest port: ");
		scanf("%d", &portno);
		packet->dport=portno;
		printf("5. protocol (6(TCP)/17(UDP)): ");
		scanf("%d", &portno);
		packet->protocol=portno;
		
		write(cfd, (char*)packet, sizeof(struct packet));
		return 0;
	}
	write(cfd, buff, sizeof(buff));
}
