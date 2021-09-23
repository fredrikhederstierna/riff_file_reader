
all:
	gcc -o tester -I. tester.c riff_file_reader.c -W -Wall -Wextra -Wno-unused-parameter -O2 -std=c99
