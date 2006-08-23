/**
	\file "Object/sizes.h"
	$Id: sizes.h,v 1.1 2006/08/23 20:57:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_SIZES_H__
#define	__HAC_OBJECT_SIZES_H__

#include <iosfwd>

namespace HAC {
namespace entity {

// extern
std::ostream&
dump_class_sizes(std::ostream&);

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_SIZES_H__

