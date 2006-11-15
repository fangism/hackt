/**
	\file "util/test/popen_test.cc"
	$Id: popen_test.cc,v 1.1 2006/11/15 00:09:18 fang Exp $
	Ripped from: info2html.sourceforge.net (info.libc.gz)
	Here is an example showing how to use `popen' and `pclose' to filter
	output through another program, in this case the paging program `more'.
 */

#include <stdio.h>
#include <stdlib.h>

static
void
write_data (FILE * stream) {
	int i;
	for (i = 0; i < 10; i++)
		fprintf (stream, "%d\n", i);
	if (ferror (stream)) {
		fprintf (stderr, "Output to stream failed.\n");
		exit (EXIT_FAILURE);
	}
}

int
main (int argc, char* argv[]) {
	FILE *output;
	output = popen ("more", "w");
	if (!output) {
		fprintf (stderr, "incorrect parameters or too many files.\n");
		return EXIT_FAILURE;
	}
	write_data (output);
	if (pclose (output) != 0) {
		fprintf (stderr, "Could not run more or other error.\n");
	}
	return EXIT_SUCCESS;
}

