/**
	\file "util/timer.cc"
	$Id: $
 */

#include <iostream>
#include "util/timer.h"
#include "util/iomanip.h"

namespace util {
using std::time;
using std::endl;
using std::difftime;

//=============================================================================
timer::timer(const bool e, ostream& o, const char* m) :
		enable(e), os(o), id(m) {
	if (enable) {
		time(&start);
		if (id) {
			os << "# " << id << endl;
		}
		os << "# [timer-start]: ";
		{
		char buf[80];
		ctime_r(&start, buf);
		os << buf;	// already has newline
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
timer::~timer() {
	if (enable) {
		time_t finish;
		time(&finish);
		os << "# [timer-end  ]: ";
		{
		char buf[80];
		ctime_r(&finish, buf);
		os << buf;
		}
		os << "# elapsed: ";
		const double diff = std::difftime(finish, start);
		const long idiff = long(diff);
		ldiv_t s = ldiv(idiff, 60);
		ldiv_t m = ldiv(s.quot, 60);
		ldiv_t h = ldiv(m.quot, 24);
		if (h.quot) {
			os << h.quot << "d ";
		}
		const save_width w(os, 2);
		os << h.rem << ':' << m.rem << ':' << s.rem << endl;
		if (id) {
			os << "# " << id << endl;
		}
	}
}

//=============================================================================
}	// end namespace util
