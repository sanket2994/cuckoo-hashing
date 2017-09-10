#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/un.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<netinet/if_ether.h>
#include<arpa/inet.h>
#include"hash.h"
#define MAX_CLI 65536


int main()
{
	int i, sfd, slen, clen, ret;
	struct sockaddr_in saddr, caddr;
	char buff[100];
	
	struct table **hashtable=(struct table**)calloc(ROWS, sizeof(struct table*));
	for(i=0;i<ROWS;i++)
		*(hashtable+i)=(struct table*)calloc(MAX_CLI, sizeof(struct table));

	//create a raw socket accepting all connection
	sfd= socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sfd==-1)
	{
		perror("socket");
		return -1;
	}
	clen=sizeof(caddr);
	while(1)
	{
		//receive a packet of any type 
		ret=recvfrom(sfd, buff, sizeof(buff), 0, (struct sockaddr*)&caddr, &clen);
		if(ret==-1)
		{
			perror("recvfrom");
			return -1;
		}
		
		struct packet *packet=(struct packet*)calloc(1, sizeof(struct packet));
		//forextracting ip information +sizeof(struct ethhdr) for excludeing the ethernet header info
		struct iphdr *ip=(struct iphdr*)(buff+sizeof(struct ethhdr));
		//checkif packet is a TCP packet
		if(ip->protocol==TCP_PKT)
		{
			//extracting the tcp packet info
			struct tcphdr *tcp=(struct tcphdr*)(buff+sizeof(struct ethhdr)+sizeof(struct iphdr));
			packet->saddr=ip->saddr;
			packet->daddr=ip->daddr;
			packet->sport=tcp->th_sport;
			packet->dport=tcp->th_dport;
			packet->protocol=ip->protocol;
			packet->hashtable=hashtable;
			packet->row=0;
			// hashing the packet
			insert_packet((void*)packet);
		}
		//check if packet is of UDP type
		else if(ip->protocol==UDP_PKT)
		{
			//extracting the udp packet info
			struct udphdr *udp=(struct udphdr*)(buff+sizeof(struct ethhdr)+sizeof(struct iphdr));
			packet->saddr=ip->saddr;
			packet->daddr=ip->daddr;
			packet->sport=udp->uh_sport;
			packet->dport=udp->uh_dport;
			packet->protocol=ip->protocol;
			packet->hashtable=hashtable;
			packet->row=0;
			//hashing the packet
			insert_packet((void*)packet);
		
		}
			
	}


}
