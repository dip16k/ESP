#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
// include configuration
#include "config.h"
#include "MsgQueue.h"
// for testing
#include "Test.h"

// Global variables for pthread locking
pthread_mutex_t lock       ;
pthread_mutex_t read_lock  ;
pthread_mutex_t write_lock ;

// conditional variable for blocking and unblocking of threads
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

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

/**	Writer_thread will write data in the queue
	when it will start wrting to the queue it holds mutex write lock
	so the other thread cannot enter the critical area, 
	after writing it unlock the reader thread so that it can now read the
	msg from the queue
*/
void * writer_thread(void *p)
{
	char msg[][10]={"hello","world","how","are","you"};
	int i,ret;
	for(i = 0;i < 5; i++)
	{
		pthread_mutex_lock(&write_lock);
		printf("\n++++++: %s :++++++\n",(char*)p);
		ret = send_msg(&mq[0], (char*)msg[i], 6, i);
		if (ret == FAIL)
			puts("send_msg call fail:");
	
		pthread_mutex_unlock(&read_lock);// unlock the mutex
	}
	return NULL;
}

/**	reader_thread will read data from the queue
	when it will start reading from the queue it holds the mutex read_lock,
	after reading it unlock the writer_thread lock so that they can start 
	writing to the queue
*/
void * reader_thread(void *p)
{
	char msg[10];
	int priority, ret, i;
	//sleep(1);
	for(i = 0; i < 5; i++)
	{
		pthread_mutex_lock(&read_lock);// lock the mutex
		printf("\n-----: %s :---------\n",(char*)p);
		ret = recv_msg(&mq[0], msg, 6, &priority);
		if(ret == FAIL)
			puts("recv_msg call fail:");
		else
			printf("recv: %s  ::  priority: %d\n",msg, priority);

		pthread_mutex_unlock(&write_lock);// unlock the mutex
	}
	return NULL;
}

/**	writer thread will start writing the data in the queue
	if the queue is full they will suspend writing and
	if the queue is available they will resume writing
	
	when queue is full they will wait for the signal from the
	reader thread when the queue becomes available, to write 
	data in the queue
	when queue is available they will resume writing into the queue
	
	the funtion use pthread_cond_timedwait to avoid dead lock condition
	if no reader is available and queue is full 
	the writer thread will wait for some time for the queue become avialable
	if it found time out, it will return (to make thread unblockable) 	
*/
void * writer_conditional_thread(void *p)
{
	char msg[][10]={"01_A","02_B","03_C","04_D","05_E","06_F","07_G"};
	int i,ret;
	static struct timespec time_to_wait = {0, 0};

	for(i = 0; i < 6; i++)
	{
		time_to_wait.tv_sec = time(NULL) + 5; // initialize waiting time

		pthread_mutex_lock(&lock);
		printf("\n+++++: %s :++++++++\n",(char*)p);
	
		ret = send_msg(&mq[0], (char*)msg[i], 6, i);
		if (ret == FAIL)
			puts("send_msg call fail:");
	
		if(mq[0].flag == FULL){

			// wait for the 'AVAILABLE' signal from the reader thread
			ret = pthread_cond_timedwait(&condition_var, &lock, &time_to_wait);
			if(ret != 0)
				puts("----TIME OUT----");
		}

		pthread_mutex_unlock(&lock);// unlock the mutex
	}
	return NULL;
}

/**	reader thread will start reading from the queue if queue
	become empty they will signal to the writer that queue is
	now availabe they can resume their writing
*/
void * reader_conditioal_thread(void *p)
{
	char msg[10];
	int priority, ret, i;
	// make sleep the reader so that writer can fill the data in queue
	sleep(2);
	for(i = 0; i < 5; i++)
	{
		pthread_mutex_lock(&read_lock);// lock the mutex
		printf("\n------: %s :-----\n",(char*)p);

		ret = recv_msg(&mq[0], msg, 6, &priority);
		if(ret == FAIL)
			puts("recv_msg call fail:");
		else
			printf("recv: %s  ::  priority: %d\n",msg, priority);

		if(mq[0].flag == AVAILABLE)
			pthread_cond_signal(&condition_var);
			//signal the writer thread that queue is available they can resume their writing
	
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
	puts("-----concurently writing and reading of 2 threads-----");

	pthread_create(&tid1,NULL,writer_thread,(void*)"writer_thread1");
	pthread_create(&tid2,NULL,reader_thread,(void*)"reader_thread2");
	pthread_join(tid1,0);
	pthread_join(tid2,0);

	pthread_mutex_destroy(&read_lock);
	pthread_mutex_destroy(&write_lock);

	puts("-------------- 2 thread testing  :finish: --------------\n");
	return 0;
}

/**	this test chase will test concurrently reading or writing of the two 4 threads
	2 are writning and 2 threads are reading the data from the queue
	but it holds a mutex lock so that only one can perform the operation at a time
*/
int thread_test2()
{
	pthread_t tid1, tid2, tid3, tid4;
	init();

	pthread_mutex_init(&read_lock, NULL);
	pthread_mutex_init(&write_lock, NULL);

	puts("\n---------------- thread_test2 ----------------------");
	puts("-----concurently writing and reading of 4 threads-----");

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

	puts("-------------- thread_test2 :finish: --------------\n");
	return 0;
}

/**	this test funtion will test writer thread funtionality when queue is full or available
	when queue is full writer thread will suspend their writing
	when queue is available writer thread will resume their writing
*/
int thread_test3()
{
	pthread_t tid1, tid2, tid3, tid4;
	init();

	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&lock, NULL);

	puts("\n------------------ thread_test3 -----------------------");
	puts("-----concurently writing and reading of 4 threads--------");
	puts("-------------suspend and resume functionality------------");

	pthread_create(&tid1,NULL,writer_conditional_thread,(void*)"writer_conditional_thread1");
	pthread_create(&tid2,NULL,writer_conditional_thread,(void*)"writer_conditional_thread2");

	pthread_create(&tid3,NULL,reader_conditioal_thread,(void*)"reader_conditioal_thread1");
	pthread_create(&tid4,NULL,reader_conditioal_thread,(void*)"reader_conditioal_thread2");

	pthread_join(tid1,0);
	pthread_join(tid2,0);
	pthread_join(tid3,0);
	pthread_join(tid4,0);

	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock);

	puts("----------------- thread_test3 :finish: --------------\n");
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
	sleep(2);
		thread_test3();
	puts("================= MAIN EXIT =========================\n");
	return 0;
}
