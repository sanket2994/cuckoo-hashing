
#include "hash.h"

/*caluate the index based on IP tuple parameters*/
void calc_index(struct packet *pack,int *key)
{
	memset(key, 0, ROWS);
  	key[0]=((((pack->saddr)^(pack->daddr)^(pack->sport)^(pack->sport<<16)^(pack->protocol))*13)%(MAXLEN-1));
	
	key[1]=((((pack->saddr)^(pack->daddr)^(pack->sport)^(pack->sport<<16)^(pack->protocol))*17)%(MAXLEN-1));

	key[2]=((((pack->saddr)^(pack->daddr)^(pack->sport)^(pack->sport<<16)^(pack->protocol))*19)%(MAXLEN-1));

	key[3]=((((pack->saddr)^(pack->daddr)^(pack->sport)^(pack->sport<<16)^(pack->protocol))*23)%(MAXLEN-1));
	
}

/*hash an entry onto the table*/
void insert_packet(void *data)
{
	struct packet *pack=(struct packet*)data;
	struct table **hashtable=pack->hashtable;
	int key[ROWS];
	struct packet *packet;
	calc_index(pack, key);
	/*check if position is available*/
	if(hashtable[pack->row][key[pack->row]].pptr==NULL) 
	{
		hashtable[pack->row][key[pack->row]].pptr=pack;
		update_packet(pack);
		print_packet(pack);
	}
	/*if not then check if the packet should be updated*/
	else
	{
		packet=hashtable[pack->row][key[pack->row]].pptr;
		/*if packet is to be updated*/
		if(calc_param(pack)==calc_param(packet))
		{
			update_packet(packet);
			print_packet(packet);
			printf("stored in table: %d at index: %d \n", pack->row, key[pack->row]);
			memset(pack, 0, sizeof(struct packet));
			free(pack);
			return;
		}
		/*or is a new entry then remove old entry and push to the next table*/
		else
		{
			printf("\n\nreallocating packet:tcp\n\n\n");
			struct packet *rmpack=hashtable[pack->row][key[pack->row]].pptr;
			if(pack->row<(ROWS-1))
			{
				hashtable[pack->row][key[pack->row]].pptr=NULL;
				insert_packet((void*)pack);
				rmpack->row=rmpack->row+1;	
				insert_packet((void*)rmpack);
			}
			else
			{
				hashtable[pack->row][key[pack->row]].pptr=NULL;
				insert_packet((void*)pack);
				rmpack->row=0;
				insert_packet((void*)rmpack);
			}
		}

	}
}


/*check if packetis old entry or new*/
unsigned long calc_param(struct packet *packet)
{
	return ((packet->saddr^packet->daddr)^((packet->sport<<16)^packet->dport)&(packet->protocol));
}

/*update the timestamp and count of an already existing entry in the table*/
void update_packet(struct packet *packet)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	packet->timestamp=asctime(timeinfo);
	packet->count++;
}

/* delete an entry from the table*/
void delete_packet(void *data)
{
	struct packet *packet=(void*)data;
	struct table **hashtable=packet->hashtable;
	int i;
	int key[ROWS];
	calc_index(packet, key);
	for(i=0;i<ROWS;i++)
	{
		/*if packet not present*/
		if(hashtable[i][key[i]].pptr==NULL)
			continue;
		/*if packet is present remove the entry*/
		if(calc_param(hashtable[i][key[i]].pptr)==calc_param(packet))
		{
			/*clear the feilds*/
			memset(hashtable[i][key[i]].pptr, 0, sizeof(struct packet));
			/*freethe memory*/
			free(hashtable[i][key[i]].pptr);	
			hashtable[i][key[i]].pptr=NULL;	
			return;
		}
	}
}

/*lookup for an entry from the table*/
void lookup_packet(void *data)
{
	struct packet *packet=(void*)data;
	struct table **hashtable=packet->hashtable;	
	int i;
	int key[ROWS];
	calc_index(packet, key);
	
	for(i=0;i<ROWS;i++)
	{
		/*if entry not present*/
		if(hashtable[i][key[i]].pptr==NULL)
			continue;
		/*if entry present*/
		if(calc_param(hashtable[i][key[i]].pptr)==calc_param(packet))
		{
			printf("Packet found at table : %d index : %d\n", i, key[i]);
			/*print the found entry*/
			print_packet(hashtable[i][key[i]].pptr);
			return;
		}
	}
}


/*print the packet*/ 
void print_packet(struct packet *packet)
{
	int port;
	char ip[INET_ADDRSTRLEN];
	printf("\nThe parameters are: \n");
	struct sockaddr_in addr;
	addr.sin_addr.s_addr=packet->saddr;

	strcpy(ip, inet_ntoa(addr.sin_addr));
	printf("1. Source Address: %s\n", ip);
	addr.sin_addr.s_addr=packet->daddr;
	
	strcpy(ip, inet_ntoa(addr.sin_addr));
	printf("2. Destination Address: %s\n", ip);
	
	port=packet->sport;
	printf("3. Source Port: %d\n", port);
	
	port=packet->dport;
	printf("4. Destination Port: %d\n", port);
	
	if(packet->protocol==TCP_PKT)
		printf("5. Protocol: TCP\n");
	else if(packet->protocol==UDP_PKT)
		printf("5. Protocol: UDP\n");
	
	printf("6. timestamp: %s", packet->timestamp);
	
	printf("7. Count: %d\n", packet->count);
}


