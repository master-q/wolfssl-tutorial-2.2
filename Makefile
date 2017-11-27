PROGS = \
	original_src/echoserver/echoserver \
	original_src/echoclient/echoclient \
	finished_src/echoserver/echoserver \
	finished_src/echoclient/echoclient

all: $(PROGS)

finished_src/echoserver/echoserver:
	(cd `dirname $@` && $(MAKE))

finished_src/echoclient/echoclient:
	(cd `dirname $@` && $(MAKE))

original_src/echoserver/echoserver:
	(cd `dirname $@` && $(MAKE))

original_src/echoclient/echoclient:
	(cd `dirname $@` && $(MAKE))

run_echoserver: finished_src/echoserver/echoserver
	(cd `dirname $<` && ./echoserver)

run_echoclient: finished_src/echoclient/echoclient
	(cd `dirname $<` && ./echoclient 127.0.0.1)

clean:
	rm -f $(PROGS)
