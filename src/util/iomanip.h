/**
	\file "util/iomanip.h"
	$Id: iomanip.h,v 1.2 2011/05/03 19:21:06 fang Exp $
 */

#ifndef	__UTIL_IOMANIP_H__
#define	__UTIL_IOMANIP_H__

#include <ios>
#include "util/attributes.h"

namespace util {
using std::ios_base;
using std::streamsize;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class iomanip_base {
protected:
	ios_base&		_ios;

	explicit
	iomanip_base(ios_base& x) : _ios(x) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T, T (ios_base::*memfun)(T)>
class iomanip_property_saver : public iomanip_base {
//	T ios_base::*memfun(T);
	const T			saved;

public:
	iomanip_property_saver(ios_base& x, 
//			T ios_base::*mf(T),
			const T p) :
		iomanip_base(x),
//		memfun(mf),
		saved((x.*memfun)(p)) { }

	~iomanip_property_saver() {
		(_ios.*memfun)(saved);
	}
} __ATTRIBUTE_UNUSED__ ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	iomanip_property_saver<streamsize, &ios_base::precision>
						save_precision;
typedef	iomanip_property_saver<streamsize, &ios_base::width>
						save_width;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace util

#endif	// __UTIL_IOMANIP_H__

