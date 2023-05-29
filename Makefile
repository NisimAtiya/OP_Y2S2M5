CC = gcc
CFLAGS = -g -Wall


all: st_pipeline

st_pipeline: st_pipeline.c
	$(CC) $(CFLAGS) -o st_pipeline st_pipeline.c

clean:
	rm -f st_pipeline