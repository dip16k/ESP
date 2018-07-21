#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "config.h"
#include "MsgQueue.h"


/**	this utility function will used to create
	a dynamic allocated memory block 

	@param		void
	@return 	address of the newly created memory block
*/
priority_queue * create_node()
{
	priority_queue * newNode;

	newNode = calloc(1,sizeof(priority_queue));
	
	if(newNode == NULL){
		perror("calloc");
		return NULL;
	}
	return newNode;
}

/**	this function will print available msg in priority list
	it is for debugging purpose

	@param		pointer to the head node
*/
void print(priority_queue *p)
{
	while(p)
	{
		printf("%s\n",p->msg);
		p = p->next;
	}
}

/**	this utility function will create message queue
	@param	n_msg_q 	number of msg queue user wants to create

	@return			pointer of created msg_queue
*/
message_queue * create_msg_queue(int n_msg_q)
{
	message_queue * msg_q;

	msg_q = calloc(n_msg_q,sizeof(message_queue));
	
	if(msg_q == NULL){
		perror("calloc");
		return NULL;
	}
	return msg_q;		
}

/**	this utility function check the availability of space in queue
	and check the flag if it is marked as high level or low level
	FULL indicate hogh level else it is at LOW LEVEL

	@param	msg_q 	pointer to msg queue

	return		SUCCESS on space available else FAIL
*/
int check_list_availability(message_queue * msg_q)
{
	// check list has space to add node and flag is not FULL		
	if((msg_q->available_msg < MSG_LIMIT) && (msg_q->flag != FULL))
		return SUCCESS;
	else
		return FAIL;// 
}

/**	this utilty function check the availability of message 

	@param	msg_q 	pointer to msg queue

	return		SUCCESS on message availability else FAIL
*/
int check_msg_availability(message_queue * msg_q)
{
	// check if list contain msg or not
	if(msg_q->available_msg !=0)
		return SUCCESS;
	else
		return FAIL;
}

/**	this utility funtion will insert the node in queue

	@param1		msg_q		pointer to the message queue 
	@param2		newNode		pointer of the newly created node
	
	@return		void	
*/
void insert_node(priority_queue ** head_p, priority_queue * newNode)
{
	priority_queue * temp = *head_p;

	if((*head_p) == NULL)// first node
	{
		(*head_p) = newNode;
		newNode->next = NULL;
	}
	else{
		while(temp->next)
			temp = temp->next;
		
		// add newNode at the end of list
		temp->next = newNode;
		newNode->next = NULL;
	}
}

/**	this utility funtion will update the newly created node with
	the data which we want to send in queue

	@param1		NewNode		pointer to new node in which we want to fill data
	@param2		buf		pointer to the buffer, from we want to get data
	@param3		length		length of the data in buffer
	@param4		priority	priority of msg

	@return 	void
*/
void update_node(priority_queue *newNode, char *buf, int length, int priority)
{
	//copy buf data to queue
	memcpy(newNode->msg, buf, length);
	newNode->priority = priority;
	newNode->msg_length = length;
}

/**	this utility function will update the queue
	and generate the High level Event
*/
void msg_queue_send_update(message_queue * msg_q, int length)
{
	msg_q->available_msg += 1;

	msg_q->used_bytes += length;

	if(msg_q->available_msg == HIGH_LEVEL)
	{
		msg_q->high(msg_q);// make flag FULL, generate event
	}
}

/**	this utility funtion is used to send data in priority queue
	It is the main funtion to intrect with the message queue

	@param1		mag_q		pointer to message queue
	@param2		buf		pointer to buffer, which data we want to send in the queue
	@param3		length		length of the data which we want to send in the queue
	@param4		periority	priority of the message

	@return		int		FAIL in case failure, SUCCESS in case of successfully delivered msg 
					in the queue
*/
int send_msg(message_queue * msg_q, char *buf, int length, int priority)
{
	priority_queue * newNode = NULL;

	//check if list has space to add new element in msg queue
	if( check_list_availability(msg_q) == FAIL )
		return FAIL;
	
	// create new node to add in msg queue
	newNode = create_node();
	
	// fill newNode with data and its priority
	update_node(newNode, buf, length, priority);
	
	// insert new node into msg queue
	insert_node(&(msg_q->head), newNode);

	// update number of msg and available bytes
	msg_queue_send_update(msg_q, length);

	return SUCCESS;
}

/**	this utility funtion will check the maximum priority 
	in the priority queue and retrun the node index in 
	the preiority queue

	@param1		mq_p		pointer to the msg queue
	@param2		priority	pointer to the variable in which the method will update the 
					priority value (return max priority)

	@return 	index		node index which has maximum priority
*/
int check_priority(message_queue * mq_p, int * priority)
{
	priority_queue * pq_p = mq_p[0].head;

	int max_priority = 0; // default priority
	int msg_index = 0;// default start index
	int priority_index = 0;	// default priority index
	
	if(mq_p->available_msg)
		max_priority = pq_p->priority;
	else
		return -1;
 
	while(msg_index < mq_p->available_msg)
	{
		if(max_priority < pq_p->priority)
		{
			max_priority = pq_p->priority;
			priority_index = msg_index;//update priority index
		}
		pq_p = pq_p->next;
		msg_index++;
	}

	*priority = max_priority;// update the priority level with max
	return priority_index;
}

/**	this utility funtion will retrive data from the priority queue

	@param1		mq_p		pointer to the msg queue
	@param2		priority_index	maximum priority blok's index number
					it will help to find out the node which has 
					max priority
	@param3		buf		pointer to buffer where we want to get data

	@return 	void
*/
void get_data(message_queue * mq_p, int priority_index, char *buf)
{
	priority_queue * pq_p = mq_p->head;
	int index;

	for(index = 0; index < priority_index; index++)
	{
		pq_p = pq_p->next;
	}
	
	memcpy(buf, pq_p->msg, pq_p->msg_length);
}

/**	this utility funtion will remove the node from the priority queue linklist

	@param1		msg_p		pointer to the msg queue
	@param2		index		node index which we want to delete

	@return 	void
*/
void delete_node(priority_queue ** head, int index)
{
	priority_queue * temp = *head;
	priority_queue * pre;
	int count = 0;
	//first_node
	if(index == 0)
	{
		(*head) = (*head)->next;
		free(temp); return;//free temp
	}
	else
	{
		for(count = 0; count < index ; count++)
		{
			pre = temp;
			temp = temp->next;
		}
		if(temp->next){
			pre->next = temp->next;
			free(temp);
		}
		else	//if it is last node
			free(temp);
	}
}

/** 	this utility will update the message queue information list
	and generate Low level event

	@param1		msg_q		pointer to the msg queue
	@param2		length		length of the msg
	
	@return		void
*/
void msg_queue_recv_update(message_queue * msg_q, int length)
{
	msg_q->available_msg -= 1;
	
	msg_q->used_bytes -= length;

	if(msg_q->available_msg == LOW_LEVEL)
	{
		msg_q->low(msg_q);// make flag AVAILABLE to indicate writer availability
	}
}

/**	this utility funtion is used to receve data from the priority queue
	It is the main funtion which receve data from the queue

	@param1		msg_q		pointer to the msg queue
	@param2		buf		pointer to the buffer in which we want to 
					get data 
	@param3		length		length of the data to get from the queue
	@param4		priority	pointer to variable in which receve method
					will update the retrived msg's priority

	@return		int 		FAIL on failure, SUCCESS on successfully recevied msg
*/
int recv_msg(message_queue * msg_q, char *buf, int length, int * priority)
{
	int priority_index;

	// check if list has msg or not
	if( check_msg_availability(msg_q) == FAIL)
		return FAIL;

	// check max priority get priority and priority index
	priority_index = check_priority(msg_q , priority);

	// copy data from msg queue to buf
	get_data(msg_q, priority_index, buf);

	//delete node 'msg is read now delete the node'
	delete_node(&(msg_q->head), priority_index);
	
	// update list 
	msg_queue_recv_update(msg_q, length);
	
	return SUCCESS;
}

/**	this utility funtion will print the current status of the message queue
*/
void print_status(message_queue * msg_q)
{
	puts("\nMessage Queue Status:");
	printf("Available_msg:%d	Available bytes:%d\n",msg_q->available_msg, msg_q->used_bytes);
}
	

//event handlers
/**	these utility funtion is used to set msg queue flag
 *	according to the msg queue event FULL or AVAILABLE
*/
void high_level(message_queue * msg_q)
{
	msg_q->flag = FULL;
	puts("Event: ** Message queue is FULL **");
	//broadcat to writer thread to stop writing
}

void low_level(message_queue * msg_q)
{
	msg_q->flag = AVAILABLE;
	puts("Event: ** Message queue is AVAILABLE to get data **");	
	//broadcast to writer thread to enable writing
}

void delete_msg_queue(message_queue **head_p)
{
	priority_queue *p = NULL;
	priority_queue *temp = (*head_p)->head;
	while(temp)
	{
		p = temp;
		temp = temp->next;
		free(p);
	}
}

