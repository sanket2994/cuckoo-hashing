#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/un.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include"hash.h"
#define MAX_CLI 65536
#define IPLEN 16
struct threadParamTCP{
	struct sockaddr_in ctaddr;
	struct sockaddr_in staddr;
	int socketfd;
	int ctlen;
	int stlen;
	struct table  **hashtable;
};
struct threadParamUDP{
	struct sockaddr_in caddr;
	struct sockaddr_in saddr;
	int socketfd;
	int clen;
	int slen;
	struct table **hashtable;
};


/*thread for udp type connection */
void* udpHandler(void *data)
{
	
	char buff[100];
	char *buff1=(char*)calloc(1,sizeof(struct packet));
	struct threadParamUDP *args=(struct threadParamUDP*)data;
	/*receive data from user */
	int ret= recvfrom(args->socketfd, buff, sizeof(buff), 0, (struct sockaddr*)&args->caddr, &args->clen);
	if(ret<0)
	{
		perror("recvfrom");
		return NULL;
	}
	
	/*check if user wants to delete entry */
	if(strcmp(buff, "delete")==0)
	{
		struct packet *delpacket;
		recvfrom(args->socketfd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&args->caddr, &args->clen);
		delpacket=(struct packet*)buff1;
		delpacket->hashtable=args->hashtable;
		delete_packet((void*)delpacket);
		memset(delpacket, 0, sizeof(struct packet));
		free(buff1);
		return NULL;
	}

	/*check if user wants to lookup for an entry */
	if(strcmp(buff, "lookup")==0)
	{
		struct packet *lookpacket;
		recvfrom(args->socketfd, buff1, sizeof(struct packet), 0, (struct sockaddr*)&args->caddr, &args->clen);
		lookpacket=(struct packet*)buff1;
		lookpacket->hashtable=args->hashtable;
		lookup_packet((void*)lookpacket);
		memset(lookpacket, 0, sizeof(struct packet));
		free(buff1);
		return NULL;
	}
	
	/*or else insert the entry */
	struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));
	packet->saddr=args->caddr.sin_addr.s_addr;
	packet->daddr=args->saddr.sin_addr.s_addr;
	packet->protocol=UDP_PKT;
	packet->sport=ntohs(args->caddr.sin_port);
	packet->dport=ntohs(args->saddr.sin_port);
	packet->row=0;
	packet->hashtable=args->hashtable;
	printf("\n\npacket received from: \nIP Address: %s\n", inet_ntoa(args->caddr.sin_addr));
	printf("The received packet is: %s\n\n",buff);

	insert_packet((void*)packet);			
	

}



/*thread for tcp connection */
void* tcpHandler(void *data)
{
	
	char buff[100];
	char *buff1=(char*)calloc(1, sizeof(struct packet));
	struct threadParamTCP *args;
	args=(struct threadParamTCP*)data;
	int cnt, ret;
	
	while((cnt=read(args->socketfd, buff, sizeof(buff)))>0)
	{
		/* check if user wants to delete an entry */
		if(strcmp(buff, "delete")==0)
		{
			struct packet *delpacket;
			recv(args->socketfd, buff1, sizeof(struct packet), 0);
			delpacket=(struct packet*)buff1;
			delpacket->hashtable=args->hashtable;
			delete_packet((void*)delpacket);
			memset(delpacket, 0, sizeof(struct packet));
			free(delpacket);
			return NULL;
		}
		/*check if user wants to look up an entry */
		if(strcmp(buff, "lookup")==0)
		{
			struct packet *lookpacket;
			recv(args->socketfd, buff1, sizeof(struct packet), 0);
			lookpacket=(struct packet*)buff1;
			lookpacket->hashtable=args->hashtable;
			lookup_packet((void*)lookpacket);
			memset(lookpacket, 0, sizeof(struct packet));
			free(lookpacket);
			return NULL;
		}
		/* or else simply hash the entry onto table */
		struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));
		packet->saddr=args->ctaddr.sin_addr.s_addr;
		packet->daddr=args->staddr.sin_addr.s_addr;
		packet->protocol=TCP_PKT;
		ret=getsockname(args->socketfd, (struct sockaddr*)&args->ctaddr, &args->ctlen);
		char addr[100];
		strcpy(addr, inet_ntoa(args->ctaddr.sin_addr));
		printf("\n\nreceiving packet from : %s\n", addr);
		printf("The received message is: %s\n", buff);
		packet->sport=ntohs(args->ctaddr.sin_port);
		packet->dport=ntohs(args->staddr.sin_port);
		packet->row=0;
		packet->hashtable=args->hashtable;
		insert_packet((void*)packet);
		free(buff1);
	}



	if(cnt==0)
	{
		close(args->socketfd);
	}


	pthread_exit(NULL);
	return NULL;

}



int main(int argc, char **argv)
{
	int ret, i, slen, clen, stfd, ctfd, sufd, cufd, client_tcp[MAX_CLI]={0}, maxfd, portno;
	struct sockaddr_in saddr, caddr;
	fd_set readfds;
	char buff[100], ip[IPLEN];
	
	if(argc==3)
	{
		strcpy(ip, argv[1]);
		sscanf(argv[2], "%d", &portno);
	}
	else
	{
		printf("Enter the ip : ");
		scanf("%s", ip);
		printf("Enter the prot number: ");
		scanf("%d", &portno);
	}

	struct table **hashtable=(struct table**)calloc(ROWS, sizeof(struct table*));
	for(i=0;i<ROWS;i++)
		*(hashtable+i)=(struct table*)calloc(MAX_CLI, sizeof(struct table));

	/* create tcp socket */
	stfd= socket(AF_INET, SOCK_STREAM, 0);
	if(stfd==-1)
	{
		perror("socket");
		return -1;
	}

	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr(ip);
	saddr.sin_port=htons(portno);
	slen= sizeof(saddr);
	clen= sizeof(caddr);
	/* bind tcp socket */
	ret= bind(stfd, (struct sockaddr*)&saddr, slen);
	if(ret)
	{
		perror("bind");
		return -1;
	}

	ret= listen(stfd, MAX_CLI);
	if(ret)
	{
		perror("listen");
		return -1;
	}
	/* create an udp socket */
	sufd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sufd==-1)
	{
		perror("socket");
		return -1;
	}
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr(ip);
	saddr.sin_port=htons(portno);
	slen= sizeof(saddr);
	clen= sizeof(caddr);

	/* bind udp socket */
	ret= bind(sufd, (struct sockaddr*)&saddr, slen);
	if(ret)
	{
		perror("bind udp");
		return -1;
	}
	
	struct threadParamTCP args;
	struct threadParamUDP arg1;
	while(1)
	{
		pthread_t tcpThread, udpThread;
		FD_ZERO(&readfds);
		FD_SET(sufd, &readfds);
		FD_SET(stfd, &readfds);
		if(sufd>stfd)
			maxfd=sufd;
		else
			maxfd=stfd;

		/* check for activity on the two sockets i.e tcp and udp */
		ret=select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(ret==-1)
		{
			perror("select");
			return -1;
		}

		/* check if activity is on udp socket */
		if(FD_ISSET(sufd, &readfds))
		{
			/* fill the udp thread structure */
			struct threadParamUDP args;
			arg1.caddr=caddr;
			arg1.saddr=saddr;
			arg1.socketfd=sufd;
			arg1.clen=clen;
			arg1.slen=slen;
			arg1.hashtable=hashtable;
			/*create a new udp thread */
			pthread_create(&udpThread, NULL, udpHandler, (void*)&arg1);
			
		}
		/* check if connection is on tcp socket */
		if(FD_ISSET(stfd, &readfds))
		{
			printf("\nsetting up an tcp client: \n\n");
			ctfd = accept(stfd, (struct sockaddr*)&caddr, &clen);
			if(ctfd==-1)
			{
				perror("accept");
				return -1;
			}
			printf("setup: %s\n", inet_ntoa(caddr.sin_addr));
			/* fill the tcp thread structure */
			args.ctaddr=caddr;
			args.staddr=saddr;
			args.socketfd=ctfd;
			args.ctlen=clen;
			args.stlen=slen;
			args.hashtable=hashtable;
			/*create a new thread for tcp connection*/
			pthread_create(&tcpThread, NULL, tcpHandler, (void*)&args);

		}

	}
}
