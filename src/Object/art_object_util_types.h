/**
	\file "Object/art_object_util_types.h"
	Collective typedefs for utility types.  
	$Id: art_object_util_types.h,v 1.1.2.2.2.1 2005/06/12 21:28:01 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UTIL_TYPES_H__
#define	__OBJECT_ART_OBJECT_UTIL_TYPES_H__

#include "util/STL/vector_fwd.h"
#include "util/STL/deque_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/memory/pointer_classes_fwd.h"

#include <deque>		// to complete type for deque

namespace ART {
namespace entity {
	class name_space;
	class meta_instance_reference_base;
	class meta_range_list;
	class instantiation_statement_base;
	USING_DEQUE
	USING_LIST
	using util::memory::never_ptr;
	using util::memory::count_ptr;

	typedef	list<never_ptr<name_space> >	namespace_collection_type;
	typedef	list<never_ptr<const name_space> >
						const_namespace_collection_type;

	/**
		Should be synchronized with
		parser::expr_list::checked_refs_type.
	 */
	typedef DEFAULT_VECTOR(count_ptr<meta_instance_reference_base>)
						checked_refs_type;


	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	// try to convert this to excl_ptr or sticky_ptr...
	typedef count_ptr<const meta_range_list>
			index_collection_item_ptr_type;

	/**
		UPDATE: now contains reference to instantiation_statements, 
		which *contain* the index/range expressions.  

		We keep track of the state of instance collections at
		various program points with this container.

		Eventually work with sub-types only?
	 */
	typedef DEFAULT_DEQUE(never_ptr<const instantiation_statement_base>)
			index_collection_type;

	/**
		The state of an instance collection, kept track by each
		instance reference.
		Since the iterators are list-like, they remain valid
		after sequence manipulation operations (like insert, erase).
	 */
	typedef index_collection_type::const_iterator
			instantiation_state;

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UTIL_TYPES_H__

