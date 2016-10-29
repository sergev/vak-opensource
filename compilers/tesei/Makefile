SUBDIRS = as lib

all:
	@for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir all; done

install:
	@for subdir in $(SUBDIRS); $(MAKE) -C $$subdir do install; done

clean:
	for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir clean; done
	rm -f *~
