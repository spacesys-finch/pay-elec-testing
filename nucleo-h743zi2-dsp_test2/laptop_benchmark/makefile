CC = gcc
CFLAGS = -g

laptop_benchmark.out: laptop_benchmark.c
	$(CC) $(CFLAGS) $^ -o laptop_benchmark.out

# use the `time` bash command to measure time of script
# "user time" is the correct one to look at
# https://unix.stackexchange.com/questions/52313/how-to-get-execution-time-of-a-script-effectively
# 
# note that you may have to run `time ./laptop_benchmark.out` directly in CLI to get proper newlines
# 	running in makefile messes up the newlines (at least for Yong Da)
time: laptop_benchmark.out
	make
	time ./laptop_benchmark.out

clean:
	rm -f laptop_benchmark.out