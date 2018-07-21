#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H
//	message queue organization
//
//	[  head node	]->[data1]->[data2]->[data3]
//	[other parameter]
//

// structure for priority queue which will contain all user data with priority
// 
#include "config.h"

typedef struct PQ{
	// priority of the message
	int priority;
	// length of the message	
	int msg_length;
	// buffer which will contain data
	char msg[MSG_SIZE];
	
	struct PQ * next;

}priority_queue;

//structure for message queue which will contain a priority queue and event handler function pointers
typedef struct MQ{

	priority_queue * head;

	// available msg in priority queue (msg = available node in linklist)
	int available_msg;
	
	// total available bytes in priority queue
	int used_bytes;

	// to check status of queue
	int flag;
	
	void (*high)(struct MQ * p);// for high water mark event
		 
	void (*low)(struct MQ * p);//for low water mark event

}message_queue;


/**
	utility functions
*/

extern priority_queue * create_node();

extern void print(priority_queue *p);

extern message_queue * create_msg_queue(int n_msg_q);

extern int check_list_availability(message_queue * msg_q);

extern int check_msg_availability(message_queue * msg_q);

extern void insert_node(priority_queue ** head_p, priority_queue * newNode);
//extern void insert_node(message_queue * msg_q, priority_queue * newNode);

extern void update_node(priority_queue *newNode, char *buf, int length, int priority);

extern void msg_queue_send_update(message_queue * msg_q, int length);

// main funtion to send data in queue
extern int send_msg(message_queue * msg_q, char *buf, int length, int priority);

extern int check_priority(message_queue * mq_p, int * priority);

extern void get_data(message_queue * mq_p, int priority_index, char *buf);

extern void delete_node(priority_queue ** head, int index);
//extern void delete_node(message_queue * msg_p, int index);

extern void msg_queue_recv_update(message_queue * msg_q, int length);

// main funtion to get data from queue
extern int recv_msg(message_queue * msg_q, char *buf, int length, int * priority);

// gives the status of message queue
extern void print_status(message_queue * msg_q);

/**	event handlers
*/
extern void high_level(message_queue * msg_q);

extern void low_level(message_queue * msg_q);

#endif
