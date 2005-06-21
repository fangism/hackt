/**
	\file "main/getopt_test.cc"
	Testing portability of libc's getopt.  
	Copied (slightly modified) from:
	http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
	$Id: getopt_test.cc,v 1.2 2005/06/21 21:26:36 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "util/getopt_portable.h"
#include "util/using_ostream.h"

using std::cin;
using std::ifstream;

int
main (int argc, char *argv[]) {
	const char* optstring = "abc:";
	int aflag = 0;
	int bflag = 0;
	char *cvalue = NULL;
	int index;
	int c;

	opterr = 0;

	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'a':
		aflag = 1;
		break;
	case 'b':
		bflag = 1;
		break;
	case 'c':
		cvalue = optarg;
		break;
	case '?':
		if (isprint(optopt))
			cerr << "Unknown option `-" << char(optopt) <<
				"'." << endl;
		else
		cerr << "Unknown option character `" <<
			reinterpret_cast<void*>(optopt) << "'." << endl;
		return 1;
	default:
		abort();
	}
	}

	cout << "aflag = " << aflag << ", bflag = " << bflag <<
		", cvalue = ";
	if (cvalue)
		cout << cvalue;
	else	cout << "(null)";
	cout << endl;

	index = optind;
	if (index == argc) {
		cout << "No standard arguments found, expecting input "
			"from stdin:" << endl;
		char ch;
		while (cin.get(ch)) cout.put(ch);
	} else {
		for ( ; index < argc; index++) {
			cout << "Non-option argument " << argv[index] << endl;
			ifstream inf(argv[index]);
			if (inf) {
				char ch;
				while (inf.get(ch)) cout.put(ch);
			} else {
				cout << "Couldn't open \"" << argv[index] <<
					"\" for reading." << endl;
			}
			inf.close();
		}
	}
	return 0;
}


