#ifndef __HASH_H
#define __HASH_H


#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<string.h>

#ifndef MAXLEN
#define MAXLEN 65536
#endif

#define TCP_PKT 6
#define UDP_PKT 17
#define ROWS 4 
//packet structure consisting of ip tuple parameters
struct packet{
	unsigned long saddr;   		/*source ip address*/
	unsigned long daddr;		/*destination ip address*/
	unsigned int sport;		/*source port*/
	unsigned int dport;		/*destination port*/
	int protocol;			/*protocol (TCP/UDP etc.)*/
	char *timestamp;		/*timestamp of the last packet received*/
	int count;			/*number of packets received of the same flow*/
	int row;			/*the row number or table number in which the packet is hashed*/
	struct table **hashtable;	/*The pointer to the hashtable*/
};	

//structure table consisting of pointer to a packet
struct table{
	struct packet *pptr;
};


//function to hash the function onto the table
void insert_packet(void *data);
/*
* in order to use this function first you need to fill the packet structure alsoyou need to create a hashtable of type struct table .
* since using cuckoo hashing the table should be of type struct table**.
* typecast the packet using a void pointer and pass it as an argument to this function
*/

void delete_packet(void *data);
/*
*This is the function to remove a previously hashed entry from the table.
*similar to the above function you need to fill the feilds of the struct packet.
*but only the first 5 feilds i.e saddr, daddr, sport, dport and protocol are necessary.
*then typecast the structure using void pointer and pass it as an argument.
*/

void lookup_packet(void *data);
/*
*This is function to lookup for an entry into the hashtable
*similar to above function we need to fill the 5 feilds of the struct packet
*i.e. saddr, daddr, sport, dport and protocol
*then typecast the struct using void pointer and pass it as an argument to the function
*/

void update_packet(struct packet *pack);
/*
*This function is called internally by insert_packet
*This udates the timestamp and the count of the packet
*/

void calc_index(struct packet *pack, int *key);
/*
*used for calcuating the key of the hashtable
*since we are using cuckoo hashing we have 4 keys for 4 levels hence pass an array to get the key
*Also the key is calculated using IP tuple parameters present in struct packet . 
*hence pointer to struct packet and an int array is passed as arguments
*/

void print_packet(struct packet *pack);
/*
*function prints all the contents of the packet present at a particular index of hash table
* it prints out src address, dst address, src port, dest port, protocol, timestamp and count
*/
unsigned long calc_param(struct packet *pack);
/*
*This is used to compare the packets which caluates a unique value and check if the collided entry is for updation or for insertion.
*/
#endif






















