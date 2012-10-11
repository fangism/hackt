/**
	\file "Object/common/util_types.h"
	Collective typedefs for utility types.  
	This file was "Object/common/util_types.h" in a former life.  
	$Id: util_types.h,v 1.7 2006/02/21 04:48:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_UTIL_TYPES_H__
#define	__HAC_OBJECT_COMMON_UTIL_TYPES_H__

#include <vector>
#include <list>
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
	class name_space;
	class meta_instance_reference_base;
	class meta_range_list;
	class instantiation_statement_base;
	using util::memory::never_ptr;
	using util::memory::count_ptr;

	typedef	std::list<never_ptr<name_space> >
						namespace_collection_type;
	typedef	std::list<never_ptr<const name_space> >
					const_namespace_collection_type;

	/**
		Should be synchronized with
		parser::expr_list::checked_refs_type.
	 */
	typedef std::vector<count_ptr<meta_instance_reference_base> >
						checked_refs_type;

	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	// try to convert this to excl_ptr or sticky_ptr...
	typedef count_ptr<const meta_range_list>
			index_collection_item_ptr_type;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_UTIL_TYPES_H__

