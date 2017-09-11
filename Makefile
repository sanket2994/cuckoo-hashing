#Makefile that invokes multiple makefiles in the test directory
#then it creates the executable files into pwd



POLL_PATH= test/poll
MULTI_PATH= test/multithread
RAW_PATH= test/rawSocket
CLI_PATH= test/clients
LIB_PATH= src/
LIBNAME= libhash.a

#invoke the Makefiles present in the test directory
all:
	$(MAKE) -C $(LIB_PATH)
	$(MAKE) -C $(POLL_PATH)
	$(MAKE) -C $(MULTI_PATH)
	$(MAKE) -C $(RAW_PATH)
	$(MAKE) -C $(CLI_PATH)


clean:
	rm -rf pollserver multithreadServer rawServer tcpclient udpclient $(LIB_PATH)$(LIBNAME)

