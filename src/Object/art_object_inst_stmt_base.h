/**
	\file "art_object_inst_stmt_base.h"
	Instance statement base class.
	$Id: art_object_inst_stmt_base.h,v 1.5 2005/01/28 19:58:42 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_BASE_H__
#define	__ART_OBJECT_INST_STMT_BASE_H__

#include "art_object_instance_management_base.h"
#include "memory/pointer_classes.h"

namespace ART {
namespace entity {

using std::istream;
USING_LIST
using namespace util::memory;	// for experimental pointer classes

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
class instantiation_statement : public instance_management_base {
protected:
	index_collection_item_ptr_type		indices;

protected:
	instantiation_statement() : instance_management_base(), 
		indices(NULL) { }
public:
	explicit
	instantiation_statement(
		const index_collection_item_ptr_type& i);

virtual	~instantiation_statement();

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

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

// should be pure virtual eventually
virtual	void unroll(void) const;

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
	load_object_base(persistent_object_manager& m, istream& );

};	// end class instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_BASE_H__

