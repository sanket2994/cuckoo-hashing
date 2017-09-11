#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<poll.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include"hash.h"
#define MAX_CLI 65535
#define IPLEN 16

int main(int argc, char **argv)
{
	int i, ret, stfd, sufd, slen, clen, ctfd, portno;
	struct sockaddr_in saddr, caddr;
	char buff[100], *ip;
	ip=(char*)calloc(1,IPLEN);
	
	/*if command line arguments passed */
	if(argc==3)
	{
		ip=argv[1];
		sscanf(argv[2],"%d", &portno);
	}
	/*if not passed */
	else
	{
		printf("Enter the ip : ");
		scanf("%s", ip);
		printf("Enter the prot number: ");
		scanf("%d", &portno);
	}

	char *buff1=(char*)calloc(1, sizeof(struct packet));
	struct table **hashtable=(struct table**)calloc(ROWS, sizeof(struct table*));	
	for(i=0;i<4;i++)
	{
		*(hashtable+i)=(struct table*)calloc(MAX_CLI, sizeof(struct table));
	}

	struct pollfd clifd[MAX_CLI];
	memset(clifd, 0, sizeof(clifd));
	int nfds=0;
	/*create socket for tcp*/
	stfd=socket(AF_INET, SOCK_STREAM, 0);
	if(stfd==-1)
	{
		perror("socket: tcp");
		return -1;
	}
	
	
	saddr.sin_addr.s_addr=inet_addr(ip);
	saddr.sin_port=htons(portno);
	saddr.sin_family=AF_INET;
	slen=sizeof(saddr);
	clen=sizeof(caddr);
	/*bind tcp socket*/
	ret=bind(stfd, (struct sockaddr*)&saddr, slen);
	if(ret)
	{
		perror("bind: tcp");
		return -1;
	}
	
	ret=listen(stfd, MAX_CLI);
	if(ret)
	{
		perror("listen");
		return -1;
	}
	
	/*Create udp socket*/
	sufd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sufd==-1)
	{
		perror("socket: udp");
		return -1;
	}

	/*bind udp socket*/
	ret=bind(sufd, (struct sockaddr*)&saddr, slen);
	if(ret)
	{
		perror("bind : udp");
		return -1;
	}
	free(ip);
	clifd[0].fd=stfd;
	clifd[0].events=POLLIN;
	nfds++;
	clifd[1].fd=sufd;
	clifd[1].events=POLLIN;
	nfds++;
	
	

	while(1)
	{
		ret=poll(clifd, nfds, -1);
		if(ret==-1)
		{
			perror("poll");
			return -1;
		}
		

		for(i=0; i<nfds; i++)
		{
			/*check for activity on sockets*/
			if(clifd[i].revents & POLLIN)
			{
				/*If activity is on udp socket*/
				if(clifd[i].fd==sufd)
				{
					printf("\n\nNew udp connection established at : ");
					ret=recvfrom(sufd, buff, sizeof(buff), 0, (struct sockaddr*)&caddr, &clen);
					if(ret==-1)
					{
						perror("udp recv: ");
					}
					printf("%s\n", inet_ntoa(caddr.sin_addr));
					printf("The received packet is: %s\n", buff);
						
					
					/*If user sent a delete command*/
					if(strcmp(buff, "delete")==0)
					{
						recvfrom(sufd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&caddr, &clen);
						struct packet *delpacket;
						delpacket=(struct packet*)buff1;
						delpacket->hashtable=hashtable;
						delete_packet((void*)delpacket);
						memset(delpacket, 0, sizeof(struct packet));
						break;
					}	

					/*if user wants to look up an entry*/
					if(strcmp(buff, "lookup")==0)
					{
						recvfrom(sufd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&caddr, &clen);
						struct packet *lookPacket;
						lookPacket=(struct packet*)buff1;
						lookPacket->hashtable=hashtable;
						lookup_packet((void*)lookPacket);
						memset(lookPacket, 0, sizeof(struct packet));				
						break;
					}
					struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));	
					packet->protocol=17;
					packet->saddr=caddr.sin_addr.s_addr;
					packet->daddr=saddr.sin_addr.s_addr;
					packet->row=0;
					packet->sport=htons(caddr.sin_port);
					packet->dport=htons(saddr.sin_port);
					packet->hashtable=hashtable;
					insert_packet((void*)packet);			
				
					break;
				}

				/*if activity is on tcp socket*/
				if(clifd[i].fd==stfd)
				{
					/*accept a new tcp connection*/
					ctfd=accept(clifd[i].fd, (struct sockaddr*)&caddr, &clen);
					if(ctfd==-1)
					{
						perror("accept");
						return -1;
					}
					
					clifd[nfds].fd=ctfd;
					clifd[nfds].events=POLLIN;	
				
					printf("\n\nNew connection established at ip : %s\n",inet_ntoa(caddr.sin_addr));
					read(clifd[nfds].fd, buff, sizeof(buff));
					printf("The read data is: %s\n", buff);
			
										
					
						
					/* if user want to delete packet*/
					if(strcmp(buff, "delete")==0)
					{
						read(clifd[nfds].fd, buff1,sizeof(struct packet));
						struct packet *delpacket;
						delpacket=(struct packet*)buff1;
						delpacket->hashtable=hashtable;
						delete_packet((void*)delpacket);
						memset(delpacket, 0, sizeof(struct packet));
						break;
					}	

					/*if user wants to lookup for an entry*/
					if(strcmp(buff, "lookup")==0)
					{
						read(clifd[nfds].fd, buff1, sizeof(struct packet));
						struct packet *lookPacket;
						lookPacket=(struct packet*)buff1;
						lookPacket->hashtable=hashtable;
						lookup_packet((void*)lookPacket);
						memset(lookPacket, 0, sizeof(struct packet));				
						break;
					}
					struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));				
					packet->protocol=6;
					packet->saddr=caddr.sin_addr.s_addr;
					packet->daddr=saddr.sin_addr.s_addr;
					getsockname(clifd[nfds].fd, (struct sockaddr*)&caddr, &clen);				
					packet->sport=htons(caddr.sin_port);
					packet->dport=htons(caddr.sin_port);
					packet->row=0;		
					packet->hashtable=hashtable;		
					insert_packet((void*)packet);
					nfds++;
					
	
				}
				/*exixting tcp connection*/
				else
				{	
					/*check if connection is closing*/
					if(read(clifd[i].fd, buff, 100)==0)
					{
						close(clifd[i].fd);
						clifd[i].events=0;
						int j;
						for(j=i; j<nfds; j++)
						{
							clifd[j]=clifd[j+1];
						}
						nfds--;
					}
					/* read data from a already connected client*/
					else
					{
						ret=recv(clifd[i].fd, buff, sizeof(buff), 0);
						if(ret==-1)
						{
							perror("read");
						}
						printf("\nThe received data is: %s\n", buff);
											
					
						/*If user sent a delete command*/
						if(strcmp(buff, "delete")==0)
						{
							recvfrom(sufd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&caddr, &clen);
							struct packet *delpacket;
							delpacket=(struct packet*)buff1;
							delpacket->hashtable=hashtable;
							delete_packet((void*)delpacket);
							memset(delpacket, 0, sizeof(struct packet));
							break;
						}	
	
						/*if user wants to look up an entry*/
						if(strcmp(buff, "lookup")==0)
						{
							recvfrom(sufd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&caddr, &clen);
							struct packet *lookPacket;
							lookPacket=(struct packet*)buff1;
							lookPacket->hashtable=hashtable;
							lookup_packet((void*)lookPacket);
							memset(lookPacket, 0, sizeof(struct packet));				
							break;
						}
						struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));	
						packet->protocol=6;
						packet->saddr=caddr.sin_addr.s_addr;
						packet->daddr=saddr.sin_addr.s_addr;
						getsockname(clifd[i].fd, (struct sockaddr*)&caddr, &clen);
						packet->sport=htons(caddr.sin_port);
						packet->dport=htons(caddr.sin_port);
						packet->row=0;
						packet->hashtable=hashtable;
						insert_packet((void*)packet);
					}
				}
			}
		}
	}	
}

