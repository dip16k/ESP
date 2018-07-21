config.h
-this file is used to make, compile time configurations like High/Low level event. Msg queue SIZE
 and other required flags values

-void msg_queue_send_update(message_queue * msg_q, int length);
 update send funtion is used to trigger high level event 'high_level(message_queue * msg_q)'

-after sending data it will update the message queue and check, if it found level == high it will
 generate an event 

-void msg_queue_recv_update(message_queue * msg_q, int length);
 update recv funtion is used to trigger low level event 'low_level(message_queue * msg_q)'
 
-after receving data it will update the message count and if it found msg level == low
 than it will generate an event

-int recv_msg(message_queue * msg_q, char *buf, int length, int * priority);
 is the main function to get data from the queue

-int send_msg(message_queue * msg_q, char *buf, int length, int priority);
 is the main funtion to send data in msg queue

-pthread_mutex_t read_lock; is used to lock reader process to thread safe reading
-pthread_mutex_t write_lock; is used to lock writer process to thread safe writing

-MainApp.c is the main entry point to test all funtionality of the message queue
