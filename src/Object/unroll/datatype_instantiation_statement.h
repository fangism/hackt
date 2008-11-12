/**
	\file "Object/unroll/datatype_instantiation_statement.h"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_data.h"
		in a previous life.  
	$Id: datatype_instantiation_statement.h,v 1.17 2008/11/12 03:00:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_DATATYPE_INSTANTIATION_STATEMENT_H__
#define	__HAC_OBJECT_UNROLL_DATATYPE_INSTANTIATION_STATEMENT_H__

#include <iostream>
#include "Object/traits/data_traits.h"
#include "Object/def/datatype_definition_base.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/canonical_type.h"
#include "Object/inst/datatype_instance_placeholder.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/inst/alias_empty.h"
#include "util/persistent_object_manager.h"

namespace HAC {
namespace entity {
class unroll_context;
class footprint;
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
	typedef class_traits<datatype_tag>		traits_type;
	typedef instantiation_statement_type_ref_base	this_type;
public:
	typedef	traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
protected:
	type_ref_ptr_type				type;

protected:
	instantiation_statement_type_ref_base() : type(NULL) { }

	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type& t) :
			type(t) { }

	~instantiation_statement_type_ref_base() { }

	type_ref_ptr_type
	get_type(void) const { return type; }

	instance_collection_parameter_type
	get_canonical_type(const unroll_context& c) const {
		const type_ref_ptr_type t(type->unroll_resolve(c));
		if (!t) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
			return instance_collection_parameter_type();
		}
		return t->make_canonical_type();
	}

	/**
		2005-07-09: NOT fusing
		Only need to check for collectible equivalence, 
		not connectible equivalence.  
	 */
	type_ref_ptr_type
	unroll_type_reference(const unroll_context& c) const {
#if 0
		if (relaxed_args) {
			const type_ref_ptr_type
			merged_type(type->merge_relaxed_actuals(relaxed_args));
			return merged_type->unroll_resolve();
		} else	return type->unroll_resolve();
#else
		return type->unroll_resolve(c);
#endif
	}

	static
	good_bool
	commit_type_first_time(instance_collection_generic_type& v,
			const instance_collection_parameter_type& t, 
			const footprint& top) {
		if (t.is_strict() && !t.unroll_definition_footprint(top).good) {
			return good_bool(false);
		}
		return v.establish_collection_type(t);
	}

	static
	good_bool
	commit_type_check(instance_collection_generic_type& v,
			const instance_collection_parameter_type& t, 
			const footprint& /* top */) {
		return v.check_established_type(t);
	}

	static
	good_bool
	instantiate_indices_with_actuals(instance_collection_generic_type& v, 
			const const_range_list& crl, 
			const unroll_context& c) {
		return v.instantiate_indices(crl, c);
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_DATATYPE_INSTANTIATION_STATEMENT_H__

