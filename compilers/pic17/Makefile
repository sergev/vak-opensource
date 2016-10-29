SUBDIRS = as cc lib include

all:
	@for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir all; done

install:
	@for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir install; done

clean:
	for subdir in $(SUBDIRS) hello test; do $(MAKE) -C $$subdir clean; done
	rm -f *~
