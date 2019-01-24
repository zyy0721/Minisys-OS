#include <inc/printf.h>
#include <inc/error.h>
#include <kern/console.h>

#define BUFLEN 1024
static char buf[BUFLEN];

char *
readline(const char *prompt)
{
	int i, c, echoing;

	if (prompt != NULL)
		printf("%s", prompt);

	i = 0;
	echoing = iscons(0);
	while (1) {
		c = getchar();
		if (c < 0) {
			printf("read error: %e\n", c);
			return NULL;
		} else if (c == '\b' || c == '\x7f') {
			if (echoing && i > 0) {
				cputchar('\x7f');
				i--;
			}
		} else if (c >= ' ' && i < BUFLEN-1) {
			if (echoing)
				cputchar(c);
			buf[i++] = c;
		} else if (c == '\n' || c == '\r') {
			if (echoing) {
				cputchar('\n');
			}
			buf[i] = 0;
			return buf;
		}
	}
}
