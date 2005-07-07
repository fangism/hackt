/**
	\file "Object/art_object_inst_stmt_data.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_data.h,v 1.4.10.4 2005/07/07 23:48:08 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_DATA_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_DATA_H__

#include <iostream>
#include "Object/art_object_data_traits.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance.h"
// #include "Object/art_object_instance_collection.h"
#include "Object/expr/param_expr_list.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
class param_expr_list;
using util::persistent_object_manager;
#include "util/using_ostream.h"

//=============================================================================
/**
	Consider making template out of this for the other classes...
	Modeled after instantiation_statement_type_ref_default, 
	in "Object/art_object_inst_stmt_type_ref_default.h".
	See its documentation for explanations.  
	TODO: sub-type for built-in types, especially ones 
		that do not require relaxed arguments (int, bool, enum).  
	This is really intended for unknown (template-dependent)
		or user-defined data-struct-types.  
 */
class class_traits<datatype_tag>::instantiation_statement_type_ref_base {
	typedef instantiation_statement_type_ref_base	this_type;
public:
	// typedef	count_ptr<param_expr_list>	relaxed_args_type;
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
protected:
	type_ref_ptr_type				type;
	const_relaxed_args_type				relaxed_args;

protected:
	instantiation_statement_type_ref_base() :
		type(NULL), relaxed_args(NULL) { }

	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type& t) :
			type(t), relaxed_args(NULL) { }

	instantiation_statement_type_ref_base(
		const type_ref_ptr_type& t, const const_relaxed_args_type& a) :
			type(t), relaxed_args(a) { }

	type_ref_ptr_type
	get_type(void) const { return type; }

	type_ref_ptr_type
	get_resolved_type(unroll_context& c) const {
		const type_ref_ptr_type ret(type->unroll_resolve(c));
		if (!ret) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
		}
#if 0
		// enable optimization later
		else {
			const_cast<this_type*>(this)->type = ret;
			// cache the equivalent resolved type
		}
#endif
		return ret;
	}

	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return relaxed_args;
	}

	/**
		Only need to check for collectible equivalence, 
		not connectible equivalence.  
	 */
	type_ref_ptr_type
	unroll_type_reference(unroll_context& c) const {
		if (relaxed_args) {
			const type_ref_ptr_type
			merged_type(type->merge_relaxed_actuals(relaxed_args));
			return merged_type->unroll_resolve(c);
		} else	return type->unroll_resolve(c);
	}

	static
	void
	commit_type_first_time(instance_collection_generic_type& v,
			const type_ref_ptr_type& t) {
		v.establish_collection_type(t);
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
		if (relaxed_args)
			relaxed_args->collect_transient_info(m);
	}

	void
	write_object_base(const persistent_object_manager& m, 
			ostream& o) const {
		m.write_pointer(o, type);
		m.write_pointer(o, relaxed_args);
	}

	void
	load_object_base(const persistent_object_manager& m, 
			istream& i) {
		m.read_pointer(i, type);
		m.read_pointer(i, relaxed_args);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_DATA_H__

