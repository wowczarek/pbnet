all:
	$(MAKE) -C pb2000
	$(MAKE) -C host
clean:
	$(MAKE) -C pb2000 clean
	$(MAKE) -C host clean

.PHONY: clean




