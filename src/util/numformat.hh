/**
	\file "util/numformat.hh"
	See also util/iomanip.hh
 */

#ifndef	__UTIL_NUMFORMAT_H__
#define	__UTIL_NUMFORMAT_H__

#include <ios>

namespace util {
using std::ostream;
using std::istream;

/**
	based on std::ios_base fmtflags, width, precision, bundled together
 */
struct numformat {
	std::ios_base::fmtflags			fmt;
	std::streamsize				width;
	std::streamsize				precision;

	explicit
	numformat(const std::ios_base& i) :
		fmt(i.flags()), width(i.width()), precision(i.precision()) { }

	// default dtor

	ostream&
	dump(ostream&) const;

	ostream&
	describe(ostream&) const;

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end struct numformat

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A temporary ostream format manipulator.
	This really could just be a std::pair from make_pair.
 */
struct format_ostream_ref {
	ostream&				os;
	const numformat&			nf;

	format_ostream_ref(ostream& o, const numformat& f) : os(o), nf(f) { }
};	// end struct format_ostream_ref

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
ostream&
operator << (const format_ostream_ref&, const T&);

}	// end namespace util

#endif	// __UTIL_NUMFORMAT_H__
