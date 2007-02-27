/**
	\file "Object/sizes-entity.h"
	$Id: sizes-entity.h,v 1.1 2007/02/27 02:27:55 fang Exp $
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

