/**
	\file "art_object_inst_stmt_data.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_data.h,v 1.1.2.2 2005/03/11 04:08:58 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_DATA_H__
#define	__ART_OBJECT_INST_STMT_DATA_H__

#include "art_object_classification_details.h"
#include "art_object_type_ref_base.h"

namespace ART {
namespace entity {
//=============================================================================
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

#if 0
	/**
		Only need to check for collectible equivalence, 
		not connectible equivalence.  
	 */
	good_bool
	unroll_type_check(instance_collection_generic_type& v, 
			const unroll_context& c) const {
		const type_ref_ptr_type
			final_type_ref(type->unroll_resolve(c));
		if (!final_type_ref) {
			cerr << "ERROR resolving data type reference during unroll."
				<< endl;
			return good_bool(false);
		}
		const bad_bool err(inst_base->commit_type(final_type_ref));
		if (err.bad) {
			cerr << "ERROR during data_instantiation_statement::unroll()"
				<< endl;
			return good_bool(false);
		}
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
#endif

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_DATA_H__

