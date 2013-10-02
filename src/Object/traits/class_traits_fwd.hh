/**
	\file "Object/traits/class_traits_fwd.hh"
	Forward declaration of entity::class_traits.
	This file used to be "Object/art_object_classification_fwd.h".
	$Id: class_traits_fwd.hh,v 1.10 2010/09/21 00:18:26 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__
#define	__HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__

#include "Object/traits/classification_tags_fwd.hh"

namespace HAC {
namespace entity {

template <class>
struct class_traits;

// forward declar specializations
template <> struct class_traits<bool_tag>;
template <> struct class_traits<int_tag>;
template <> struct class_traits<ebool_tag>;
template <> struct class_traits<eint_tag>;
template <> struct class_traits<enum_tag>;
template <> struct class_traits<real_tag>;
template <> struct class_traits<node_tag>;
template <> struct class_traits<process_tag>;
template <> struct class_traits<channel_tag>;
template <> struct class_traits<datastruct_tag>;
template <> struct class_traits<string_tag>;
template <> struct class_traits<pbool_tag>;
template <> struct class_traits<pint_tag>;
template <> struct class_traits<preal_tag>;
template <> struct class_traits<pstring_tag>;

// These are defined in "Object/traits/*_traits.hh" as specializations.  
typedef	class_traits<bool_tag>		bool_traits;
typedef	class_traits<int_tag>		int_traits;
typedef	class_traits<ebool_tag>		ebool_traits;
typedef	class_traits<eint_tag>		eint_traits;
typedef	class_traits<enum_tag>		enum_traits;
typedef	class_traits<real_tag>		real_traits;
typedef	class_traits<node_tag>		node_traits;
typedef	class_traits<string_tag>	string_traits;
typedef	class_traits<process_tag>	process_traits;
typedef	class_traits<channel_tag>	channel_traits;
typedef	class_traits<pbool_tag>		pbool_traits;
typedef	class_traits<pint_tag>		pint_traits;
typedef	class_traits<preal_tag>		preal_traits;
typedef	class_traits<pstring_tag>	pstring_traits;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CLASS_TRAITS_FWD_H__

