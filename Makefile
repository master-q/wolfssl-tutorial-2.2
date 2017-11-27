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

clean:
	rm -f $(PROGS)
