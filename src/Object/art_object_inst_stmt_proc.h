/**
	\file "Object/art_object_inst_stmt_proc.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_proc.h,v 1.3 2005/05/10 04:51:15 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_PROC_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_PROC_H__

#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_ref_base.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
//=============================================================================
class class_traits<process_tag>::instantiation_statement_type_ref_base {
protected:
	type_ref_ptr_type				type;
protected:
	instantiation_statement_type_ref_base() : type(NULL) { }

	explicit
	instantiation_statement_type_ref_base(
		const type_ref_ptr_type& t) : type(t) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return type; }

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

#endif	// __OBJECT_ART_OBJECT_INST_STMT_PROC_H__

