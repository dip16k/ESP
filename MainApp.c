#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
// include configuration
#include "config.h"
#include "MsgQueue.h"
// for testing
#include "Test.h"

// Global variables for pthread locking
//pthread_mutex_t lock       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_lock  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;

// global message queue pointer
message_queue *mq = NULL;

/**
 *	This utility funtion create one message queue and 
 *	initialize its members
 ********************************************************/
void init()
{
	mq = create_msg_queue(1);	
	mq->high          = high_level; 
	mq->low           = low_level;
	mq->available_msg = 0;	
	mq->used_bytes    = 0;
	mq->flag 	  = READY;
}
void * writer_thread(void *p)
{
	char msg[][10]={"hello","world","how","are","you"};
	int i,ret;
	for(i = 0;i < 5; i++)
	{
		//pthread_mutex_lock(&lock);// lock the mutex
		pthread_mutex_lock(&write_lock);
		printf("+++\n%s\n",(char*)p);
		ret = send_msg(&mq[0], (char*)msg[i], 6, i);
		if (ret == FAIL)
			puts("send_msg call fail:");
		
		//pthread_mutex_unlock(&lock);// unlock the mutex
		pthread_mutex_unlock(&read_lock);// unlock the mutex
	}
	return NULL;
}

void * reader_thread(void *p)
{
	char msg[10];
	int priority, ret, i;
	//sleep(1);
	for(i = 0; i < 5; i++)
	{
		//pthread_mutex_lock(&lock);// lock the mutex
		pthread_mutex_lock(&read_lock);// lock the mutex
		printf("---\n%s\n",(char*)p);
		ret = recv_msg(&mq[0], msg, 6, &priority);
		if(ret == FAIL)
			puts("recv_msg call fail:");
		else
			printf("recv: %s  ::  priority: %d\n",msg, priority);

		//pthread_mutex_unlock(&lock);// unlock the mutex
		pthread_mutex_unlock(&write_lock);// unlock the mutex
	}
	return NULL;
}

int thread_test1()
{
	pthread_t tid1, tid2;
	init();

	pthread_mutex_init(&read_lock, NULL);
	pthread_mutex_init(&write_lock, NULL);
	puts("-------------- 2 threads testing ----------------------");

	pthread_create(&tid1,NULL,writer_thread,(void*)"writer_thread1");
	pthread_create(&tid2,NULL,reader_thread,(void*)"reader_thread2");
	pthread_join(tid1,0);
	pthread_join(tid2,0);

	pthread_mutex_destroy(&read_lock);
	pthread_mutex_destroy(&write_lock);

	puts("-------------- 2 thread testing  finish --------------\n");
	return 0;
}

int thread_test2()
{
	pthread_t tid1, tid2, tid3, tid4;
	init();

	pthread_mutex_init(&read_lock, NULL);
	pthread_mutex_init(&write_lock, NULL);

	puts("-------------- 4 threads testing ----------------------");

	pthread_create(&tid1,NULL,writer_thread,(void*)"writer_thread1");
	pthread_create(&tid2,NULL,writer_thread,(void*)"writer_thread2");

	pthread_create(&tid3,NULL,reader_thread,(void*)"reader_thread1");
	pthread_create(&tid4,NULL,reader_thread,(void*)"reader_thread2");

	pthread_join(tid1,0);
	pthread_join(tid2,0);
	pthread_join(tid3,0);
	pthread_join(tid4,0);

	pthread_mutex_destroy(&read_lock);
	pthread_mutex_destroy(&write_lock);
	puts("-------------- 4 thread testing finish --------------\n");
	return 0;
}
int main()
{
	puts("================= MAIN test starts ==================\n");
		test1();
	sleep(2);
		high_event_test();
	sleep(2);
		low_event_test();
	sleep(2);
		thread_test1();
	sleep(2);
		thread_test2();
	sleep(1);
	puts("================= MAIN EXIT =========================\n");
	return 0;
}
