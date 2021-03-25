TARGET=mytar
CC=gcc

.PHONY: cleanall cleanobj

all: $(TARGET)

mytar: inodemap.o mytar_function.o extract_tar.o


cleanobj:
	$(RM)

cleanall: cleanobj
	$(RM) $(TARGET)