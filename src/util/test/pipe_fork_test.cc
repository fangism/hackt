/**
	\file "util/test/pipe_fork_test.cc"
	$Id: pipe_fork_test.cc,v 1.1 2006/11/15 00:09:18 fang Exp $
	Ripped from: libc.info.gz (info2html.sourceforge.net)

	Here is an example of a simple program that creates a pipe.  This
	program uses the `fork' function ( Creating a Process) to create
	a child process.  The parent process writes data to the pipe, which is
	read by the child process.
 */

#include "config.h"
#ifdef	HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
// assuming these are present
#include <stdio.h>
#include <stdlib.h>
#include "util/memory/excl_ptr.h"
#include "util/memory/deallocation_policy.h"

using util::memory::excl_ptr;
using util::memory::FILE_tag;
typedef	excl_ptr<FILE, FILE_tag>	FILE_ptr;

/**
	Read characters from the pipe and echo them to `stdout'.
 */
static
void
read_from_pipe (const int file) {
	const FILE_ptr stream(fdopen (file, "r"));
	int c;
	while ((c = fgetc (&*stream)) != EOF)
		putchar (c);
	// fclose (stream);	// pointer will auto-close
}

/* Write some random text to the pipe. */
static
void
write_to_pipe (const int file) {
	const FILE_ptr stream(fdopen (file, "w"));
	fprintf (&*stream, "hello, world!\n");
	fprintf (&*stream, "goodbye, world!\n");
	// fclose (stream);	// pointer will auto-close
}

int
main (int argc, char* argv[]) {
	int mypipe[2];
	/* Create the pipe. */
	if (pipe (mypipe)) {
		fprintf (stderr, "Pipe failed.\n");
		return EXIT_FAILURE;
	}

	/* Create the child process. */
	const pid_t pid = fork();
	if (pid == pid_t(0)) {
		/* This is the child process.  Close other end first. */
		close (mypipe[1]);
		read_from_pipe (mypipe[0]);
		return EXIT_SUCCESS;
	} else if (pid < pid_t(0)) {
		/* The fork failed. */
		fprintf (stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	} else {
		/* This is the parent process.  Close other end first. */
		close (mypipe[0]);
		write_to_pipe (mypipe[1]);
		return EXIT_SUCCESS;
	}
}

