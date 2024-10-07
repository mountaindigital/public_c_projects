#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEXLEN	2
#define BITMASK	0x0F

char hexen(unsigned digit) {
	if (digit < 10) {
		return '0' + digit;
	}

	return 'A' + digit - 10;
}

void tohex(char output[HEXLEN + 1], unsigned char input) {
	unsigned digit = 0;
	output[HEXLEN] = '\0';
	digit = input & BITMASK;
	output[1] = hexen(digit);
	input >>= 4;
	digit = input & BITMASK;
	output[0] = hexen(digit);
}

int main(void) {
	// printf("%s ", (char[]){'a', 'b', '\0'});
	int ch = {0};
	char hexcode[HEXLEN + 1] = {0};
	while ((ch = fgetc(stdin)) != EOF) {
		if (ch == '\n')
			continue;
		tohex(hexcode, ch);
		printf("%s ", hexcode);
	}

	putchar('\n');

	return EXIT_SUCCESS;
}
