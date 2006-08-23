/**
	\file "common/sizes.h"
	How big is the struct?
	$Id: sizes.h,v 1.1 2006/08/23 20:57:23 fang Exp $
 */

#ifndef	__HAC_COMMON_SIZES_H__
#define	__HAC_COMMON_SIZES_H__

#include <iosfwd>
#include "util/what_fwd.h"

namespace HAC {
/**
	Reports mangled name of class and it's size.  
	Doesn't actually require any symbol information, 
	as far as linkage and dependency goes.
 */
template <class T>
static
inline
std::ostream&
__dump_class_size(std::ostream& o) {
	o << "sizeof(" << util::what<T>::name() << ") = "
		<< sizeof(T) << std::endl;
	return o;
}

// basic type sizes
std::ostream&
dump_class_sizes(std::ostream&);

}	// end namespace HAC

#endif	// __HAC_COMMON_SIZES_H__

