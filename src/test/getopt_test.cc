/**
	\file "main/getopt_test.cc"
	Testing portability of libc's getopt.  
	All this does is character-for-character cat. 
	Copied (slightly modified) from:
	http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
	$Id: getopt_test.cc,v 1.2.10.4 2005/08/04 20:52:06 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "util/getopt_portable.h"
#include "util/using_ostream.h"

using std::cin;
using std::ifstream;

int
main (int argc, char *argv[]) {
	const char optstring[] = "+abc:";
	int aflag = 0;
	int bflag = 0;
	char *cvalue = NULL;
	int index;
	int c;

	{
		cout << "--------------------------------------------" << endl;
		int i = 0;
		if (i < argc) {
			cout << "entire command:";
			while (i < argc) {
				cout << ' ' << argv[i++];
			}
			cout << endl;
		}
	}

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
	case ':':
		cerr << "Expected but missing non-option argument." << endl;
		return 1;
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

	{
		int i = optind;
		if (i < argc) {
			cout << "non-option argv-elements:";
			while (i < argc) {
				cout << ' ' << argv[i++];
			}
			cout << endl;
		}
	}

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


