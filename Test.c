#include <stdio.h>
#include "MsgQueue.h"
#include "Test.h"

/**	this funtion will test the basic read/write funtionality of the queue
 *	first it will send the data to the queue with its priority than it will
 *	check the status and than start reading the queue. at the end it will
 *	again read the status of the msg queue
*/
void test1()
{
	char snd[][10]={"hello","world","how","are","you"};
	char rcv[10];
	int priority, i;
	
	puts("-------------- Basic funtionality testing --------------");
	message_queue *mq;
	// i want to create 1 msg queue
	mq = create_msg_queue(1);
	
	//register signal handler
	mq->high = high_level;
	mq->low = low_level;

	for(i = 0; i < 5; i++)
	{	
		send_msg(&mq[0], (char*)snd[i], 6, i);
	}
	
	// print current status of msg queue after sending 5 msg
	print_status(mq);

	for(i = 0; i < 5; i++)
	{
		recv_msg(&mq[0], rcv, 6, &priority);
		printf("recv: %s 	priority: %d\n",rcv, priority);
	}

	// print current status of msg queue after receving 5 msg
	print_status(mq);

	puts("---------- EXIT: Basic funtionality testing --------------\n");
}

/**	this funtion will test low level event 
 *	when we try to read at low level this will generate an event 
 *	and set the message queue flag 
*/
void low_event_test()
{
	char msg[10];
	int priority, ret;	
	message_queue *mq;

	puts("-------------- low level event testing --------------");

	// i want to create 1 msg queue
	mq = create_msg_queue(1);
	
	mq->high = high_level; 
	mq->low = low_level;

	//try to recevie msg from empty queue
	ret = recv_msg(&mq[0], msg, 5, &priority);
	if(ret == FAIL)
		puts("recv_msg fail:");
	else
		printf("recevied msg: %s\n",msg);

	puts("----------- EXIT : low level event testing ---------\n");
}

/**	this funtion will test high level event
*	when we try to send at up to max limit this will generate an event 
*	and set the message queue flag
*/
void high_event_test()
{	
	message_queue *mq;
	char p[]="hello";
	int i,ret;
	
	puts("-------------- high level event testing ----------------");

	mq = create_msg_queue(1);
	
	//register signal handler
	mq->high = high_level;
	mq->low = low_level;

	for(i = 0; i <= HIGH_LEVEL; i++)
	{
		ret = send_msg(&mq[0], p, 6, i);
		if(ret == FAIL)
			printf("send_msg_fail:: i: %d\n",i);
	}

	puts("------------ EXIT : high level event testing -----------\n");
}
//end of file
