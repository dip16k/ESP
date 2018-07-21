#ifndef CONFIG_H
#define CONFIG_H

/**	this file is used to make compile time configurations 
*/


// to config msg buffers size
#define MSG_SIZE 20

// max and min message limit in message queue
#define MSG_LIMIT 10

// define event levels
#define LOW_LEVEL 1
#define HIGH_LEVEL 9

// for debugging printing messages
#define DEBUG

// to indicate status of message queue
#define AVAILABLE 0
#define FULL 2

// action return by the funtion execution
#define FAIL 0
#define SUCCESS 1

// flag status
#define BUSY 2
#define READY 3


#endif

