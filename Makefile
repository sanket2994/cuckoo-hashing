#Makefile that invokes multiple makefiles in the test directory
#then it copies the executable files created by those makefiles into pwd



POLL_PATH= test/poll

MULTI_PATH= test/multithread

RAW_PATH= test/rawSocket

CLI_PATH= test/clients

default: all pollserver multithreadServer rawServer client


#invoke the Makefiles present in the test directory
all:
	$(MAKE) -C $(POLL_PATH)
	$(MAKE) -C $(MULTI_PATH)
	$(MAKE) -C $(RAW_PATH)
	$(MAKE) -C $(CLI_PATH)

#move the executables created by these Makefilesintp pwd 
pollserver: $(POLL_PATH)/pollserver
	mv $(POLL_PATH)/pollserver .

multithreadServer: $(MULTI_PATH)/multithreadServer
	mv $(MULTI_PATH)/multithreadServer .

rawServer: $(RAW_PATH)/rawServer
	mv $(RAW_PATH)/rawServer .

client: $(CLI_PATH)/tcpclient $(CLI_PATH)/udpclient
	mv $(CLI_PATH)/tcpclient $(CLI_PATH)/udpclient .


clean:
	rm -rf pollserver multithreadServer rawServer tcpclient udpclient






