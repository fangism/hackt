/**
	\file "util/numformat.tcc"
 */

#ifndef	__UTIL_NUMFORMAT_TCC__
#define	__UTIL_NUMFORMAT_TCC__

#include <ostream>
#include "util/numformat.hh"
#include "util/iomanip.hh"

namespace util {

template <class T>
inline
ostream&
operator << (const format_ostream_ref& fos, const T& t) {
	ostream& o(fos.os);
	const save_flags	_sf(o, fos.nf.fmt);
	const save_width	_sw(o, fos.nf.width);
	const save_precision	_sp(o, fos.nf.precision);
	return o << t;
	// automatically restore previous format
}

}	// end namespace util

#endif	// __UTIL_NUMFORMAT_TCC__
