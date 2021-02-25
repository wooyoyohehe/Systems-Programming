TARGET=my_malloc-driver
CFLAGS=-std=gnu99 -Wno-deprecated-declarations

.PHONY: cleanall cleanobj

all: $(TARGET)

my_malloc-driver: my_malloc.o my_malloc-driver.o

cleanobj:
	$(RM) my_malloc.o my_malloc-driver.o

cleanall: cleanobj
	$(RM) $(TARGET)
