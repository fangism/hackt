/**
	\file "Object/art_object_inst_stmt_base.h"
	Instance statement base class.
	$Id: art_object_inst_stmt_base.h,v 1.9 2005/05/22 06:24:17 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_BASE_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_BASE_H__

#include <string>
#include "Object/art_object_fwd.h"
#include "Object/art_object_instance_management_base.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
using std::string;
using std::istream;
USING_LIST
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::good_bool;

//=============================================================================
/**
	This node retains the information for an instantiation statement.  
	This is what will be unrolled.  
	No parent, is a globally sequential item.  
	Every sub-class will contain a modifiable
	back-reference to an (sub-type of) instance_collection_base, 
	where the collection will be unrolled.  
	Should this point to an unrolled instance?
	No, it will be looked up.  
 */
class instantiation_statement_base : public instance_management_base {
protected:
	index_collection_item_ptr_type		indices;

protected:
	instantiation_statement_base() : instance_management_base(), 
		indices(NULL) { }
public:
	explicit
	instantiation_statement_base(
		const index_collection_item_ptr_type& i);

virtual	~instantiation_statement_base();

	ostream&
	dump(ostream& o) const;

virtual	void
	attach_collection(const never_ptr<instance_collection_base> i) = 0;

virtual	never_ptr<instance_collection_base>
	get_inst_base(void) = 0;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

	string
	get_name(void) const;

	size_t
	dimensions(void) const;

	index_collection_item_ptr_type
	get_indices(void) const;

	good_bool
	resolve_instantiation_range(const_range_list&, 
		const unroll_context&) const;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

// should be pure virtual eventually
virtual	void
	unroll(unroll_context& ) const;

/***
	case: A top-level instantiation is called.
	case: won't happen?  instantiation inside definition called
***/
	// unroll (context?)

protected:

	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

};	// end class instantiation_statement_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_BASE_H__

