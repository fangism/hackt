/**
	\file "util/numformat.cc"
 */

#include <iostream>
#include <iomanip>
#include "util/numformat.hh"
#include "util/iomanip.hh"
#include "util/IO_utils.tcc"

namespace util {
// class numformat method definitions
using std::ios_base;
using std::endl;

ostream&
numformat::dump(ostream& o) const {
{
	const save_flags s(o << "flags:0x", fmt);
	o << std::hex << fmt;
}
	o << ", w:" << width;
	o << ", p:" << precision;
	return o;
}

ostream&
numformat::describe(ostream& o) const {
	o << "fixed: " << ((fmt & ios_base::fixed) ?
		"yes" : "no") << endl;
	o << "sci  : " << ((fmt & ios_base::scientific) ?
		"yes" : "no") << endl;
	o << "prec : " << precision << endl;
	return o;
}

void
numformat::write_object(ostream& o) const {
	write_value(o, fmt);
	write_value(o, width);
	write_value(o, precision);
}

void
numformat::load_object(istream& i) {
	read_value(i, fmt);
	read_value(i, width);
	read_value(i, precision);
}

}	// end namespace util

