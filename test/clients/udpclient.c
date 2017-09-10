#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/un.h>
#include<sys/types.h>
#include<string.h>
#include "hash.h"

int main(int argc, char *argv[])
{
	int sfd, ret, slen, portno;
	struct sockaddr_in saddr;
	char buff[100];
	
	char *ip=argv[1];
	sscanf(argv[2], "%d", &portno);

	
	sfd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd==-1)
	{
		perror("socket");
		return -1;
	}
	
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr(ip);
	saddr.sin_port=htons(portno);
	slen=sizeof(saddr);
	printf("Enter packet to be sent: ");
	scanf("%[^\n]s",buff);
	sendto(sfd, buff, sizeof(buff), 0, (struct sockaddr*)&saddr, slen);

	
	//check if user wants to lookup or delete a packet
	if(strcmp(buff, "delete")==0 || strcmp(buff, "lookup")==0)
        {
	                
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

		sendto(sfd, (char*)packet, sizeof(struct packet), 0, (struct sockaddr*)&saddr, slen);
                return 0;
        }

		
}
