app: MsgQueue.o Test.o MainApp.o
	cc -o app MainApp.o MsgQueue.o Test.o -lpthread

MsgQueue.o: MsgQueue.c MsgQueue.h config.h 
	cc -c -Wall MsgQueue.c

Test.o: Test.c Test.h MsgQueue.h  
	cc -c -Wall Test.c

MainApp.o: MainApp.c config.h MsgQueue.h Test.h
	cc -c -Wall MainApp.c -lpthread
	
clean:
	rm *.o
