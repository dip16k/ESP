#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#include "thread.h"

pthread_t tid1, tid2;
pthread_mutex_t lock;


void * f1(void *p)
{
	printf("%s\n",(char*)p);
	printf("%ld\n",tid1);

	return NULL;
}

void * f2(void *p)
{
	printf("%s\n",(char*)p);
	printf("%ld\n",tid2);
	
	return NULL;
}

int thread_test1()
{
	pthread_create(&tid1,NULL,f1,(void*)"thread1");
	pthread_create(&tid2,NULL,f2,(void*)"thread2");

	pthread_join(tid1,0);
	pthread_join(tid2,0);

	return 0;
}
