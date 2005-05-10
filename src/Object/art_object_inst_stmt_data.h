/**
	\file "Object/art_object_inst_stmt_data.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_data.h,v 1.3 2005/05/10 04:51:15 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_DATA_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_DATA_H__

#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_ref_base.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Consider making template out of this for the other classes...
 */
class class_traits<datatype_tag>::instantiation_statement_type_ref_base {
protected:
	type_ref_ptr_type				type;
protected:
	instantiation_statement_type_ref_base() : type(NULL) { }

	explicit
	instantiation_statement_type_ref_base(
		const type_ref_ptr_type& t) : type(t) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return type; }

	/**
		Only need to check for collectible equivalence, 
		not connectible equivalence.  
	 */
	type_ref_ptr_type
	unroll_type_reference(unroll_context& c) const {
		return type->unroll_resolve(c);
	}

	static
	good_bool
	commit_type_check(instance_collection_generic_type& v,
			const type_ref_ptr_type& t) {
		return v.commit_type(t);
	}

	void
	collect_transient_info_base(persistent_object_manager& m) const {
		type->collect_transient_info(m);
	}

	void
	write_object_base(const persistent_object_manager& m, 
			ostream& o) const {
		m.write_pointer(o, type);
	}

	void
	load_object_base(const persistent_object_manager& m, 
			istream& i) {
		m.read_pointer(i, type);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_DATA_H__

