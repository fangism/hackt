/**
	\file "Object/common/util_types.h"
	Collective typedefs for utility types.  
	This file was "Object/common/util_types.h" in a former life.  
	$Id: util_types.h,v 1.4.2.1 2006/01/18 06:24:52 fang Exp $
 */

#ifndef	__OBJECT_COMMON_UTIL_TYPES_H__
#define	__OBJECT_COMMON_UTIL_TYPES_H__

#include "util/STL/vector_fwd.h"
#include "util/STL/deque_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/memory/pointer_classes_fwd.h"
#include <deque>		// to complete type for deque

namespace HAC {
namespace entity {
	class name_space;
	class meta_instance_reference_base;
	class meta_range_list;
	class instantiation_statement_base;
	using std::deque;
	using std::default_deque;
	using std::default_vector;
	using std::list;
	using std::default_list;
	using util::memory::never_ptr;
	using util::memory::count_ptr;

	typedef	default_list<never_ptr<name_space> >::type
						namespace_collection_type;
	typedef	default_list<never_ptr<const name_space> >::type
					const_namespace_collection_type;

	/**
		Should be synchronized with
		parser::expr_list::checked_refs_type.
	 */
	typedef default_vector<count_ptr<meta_instance_reference_base> >::type
						checked_refs_type;


	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	// try to convert this to excl_ptr or sticky_ptr...
	typedef count_ptr<const meta_range_list>
			index_collection_item_ptr_type;

	/**
		Helper struct to predicate instantiation statements.  
		Defined in "Object/common/predicated_inst_stmt_ptr.h".
	 */
	class predicated_inst_stmt_ptr;

	/**
		UPDATE: now contains reference to instantiation_statements, 
		which *contain* the index/range expressions.  

		We keep track of the state of instance collections at
		various program points with this container.

		Eventually work with sub-types only?
		TODO: subtype into specific instantiation statement types.  
	 */
//	typedef DEFAULT_DEQUE(never_ptr<const instantiation_statement_base>)
	typedef default_deque<predicated_inst_stmt_ptr>::type
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
}	// end namespace HAC

#endif	// __OBJECT_COMMON_UTIL_TYPES_H__

