/**
	\file "Object/traits/class_traits_fwd.h"
	Forward declaration of entity::class_traits.
	This file used to be "Object/art_object_classification_fwd.h".
	$Id: class_traits_fwd.h,v 1.5 2006/04/12 08:53:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__
#define	__HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__

#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {

template <class>
class class_traits;

// These are defined in "Object/traits/*_traits.h" as specializations.  
typedef	class_traits<bool_tag>		bool_traits;
typedef	class_traits<int_tag>		int_traits;
typedef	class_traits<enum_tag>		enum_traits;
typedef	class_traits<process_tag>	process_traits;
typedef	class_traits<channel_tag>	channel_traits;
typedef	class_traits<pbool_tag>		pbool_traits;
typedef	class_traits<pint_tag>		pint_traits;
typedef	class_traits<preal_tag>		preal_traits;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__

