/**
	\file "Object/art_object_instance_int.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_int.cc,v 1.21.2.4 2005/07/08 18:15:29 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_INT_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_INT_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

//=============================================================================
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_int.h"
#include "Object/art_object_instance_alias_empty.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_built_ins.h"
#include "Object/art_object_nonmeta_value_reference.h"
#include "Object/art_object_instance_collection.tcc"
#include "Object/art_object_inst_stmt.h"
#include "Object/art_object_inst_stmt_data.h"
	// for class_traits<>::instantiation_statement_type_ref_base

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"


//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::int_instance_collection,
		"int_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_scalar, "int_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_1D, "int_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_2D, "int_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_3D, "int_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_4D, "int_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_scalar, DINT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_1D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_2D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_3D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_4D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util


namespace ART {
namespace entity {

//=============================================================================
// functor specializations
template <>
struct collection_type_manager<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<int_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<int_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			return os << "int<" << c.get_type_parameter() <<
				">^" << c.get_dimensions();
		}
	};	// end struct dumper

	static
	void
	collect(persistent_object_manager& m, 
		const instance_collection_generic_type& c) {
		// c.type_parameter contains no pointers
	}

	static
	void
	write(const persistent_object_manager&, ostream& o,
		const instance_collection_generic_type& c) {
		// parameter is just an int
		write_value(o, c.type_parameter);
	}

	static
	void
	load(const persistent_object_manager&, istream& i,
		instance_collection_generic_type& c) {
		// parameter is just an int
		read_value(i, c.type_parameter);
	}

	/**
		TODO: optimization, cache the result of the first call
		to this function (for this collection), and return the
		cached type, because it's not supposed to change.  
		See also implementation in "Object/art_built_ins.cc", 
			under int32_type_ptr.
		TODO: What if parameter is template-dependent and not
			yet resolved?
			Should probably return the template-dependent
			width expression in the type pointer.  
	 */
	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type& i) {
		if (i.type_parameter) {
			// then type was already committed
			return type_ref_ptr_type(
				data_type_reference::make_quick_int_type_ref(
					i.type_parameter));
		} else {
			// not yet unrolled... need to extract from
			// first instantiation statement.
			// extract as in pulling teeth...
			// TODO: subtype versions of the following calls
			const never_ptr<const data_instantiation_statement>
				first(i.index_collection.front()
				.is_a<const data_instantiation_statement>());
			return first->get_type_ref()
				.is_a<const data_type_reference>();
		}
	}

private:
	/**
		Can't we expect type to be resolved constants by now?
	 */
	static
	pint_value_type
	get_int_width(const type_ref_ptr_type& tp) {
		// resolve type def?
		const type_ref_ptr_type
			t(tp->make_canonical_type_reference()
				.is_a<const data_type_reference>());
		INVARIANT(t->get_base_datatype_def()
			->resolve_canonical_datatype_definition() == &int_def);

		const count_ptr<const param_expr_list>
			params(t->get_template_params().get_strict_args());
		NEVER_NULL(params);
		// extract first and only parameter, the integer width
		const count_ptr<const const_param_expr_list>
			cparams(params.is_a<const const_param_expr_list>());

		NEVER_NULL(cparams);
		INVARIANT(cparams->size() == 1); 
		const count_ptr<const const_param>&
			param1(cparams->front());
		NEVER_NULL(param1);
		const count_ptr<const pint_const>
			pwidth(param1.is_a<const pint_const>());
		NEVER_NULL(pwidth);
		return pwidth->static_constant_value();
	}

public:
	/**
		During unroll phase, this commits the type of the collection.  
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	static
	bad_bool
	commit_type(const instance_collection_generic_type& c,
		const type_ref_ptr_type& tp) {
		// resolve type def?
#if 0
		const type_ref_ptr_type
			t(tp->make_canonical_type_reference()
				.is_a<const data_type_reference>());
		INVARIANT(t->get_base_datatype_def()
			->resolve_canonical_datatype_definition() == &int_def);

		const count_ptr<const param_expr_list>
			params(t->get_template_params().get_strict_args());
		NEVER_NULL(params);
		// extract first and only parameter, the integer width
		const count_ptr<const const_param_expr_list>
			cparams(params.is_a<const const_param_expr_list>());

		NEVER_NULL(cparams);
		INVARIANT(cparams->size() == 1); 
		const count_ptr<const const_param>&
			param1(cparams->front());
		NEVER_NULL(param1);
		const count_ptr<const pint_const>
			pwidth(param1.is_a<const pint_const>());
		NEVER_NULL(pwidth);
		const pint_value_type
			new_width = pwidth->static_constant_value();
		INVARIANT(new_width);
		if (c.is_partially_unrolled()) {
			INVARIANT(c.type_parameter);
			return bad_bool(new_width != c.type_parameter);
		} else { 
			c.type_parameter = new_width;
			return bad_bool(false);
		}
#else
		const pint_value_type
			new_width = get_int_width(tp);
		INVARIANT(c.type_parameter);
		return bad_bool(new_width != c.type_parameter);
#endif
	}

	static
	void
	commit_type_first_time(instance_collection_generic_type& c,
		const type_ref_ptr_type& tp) {
		INVARIANT(!c.type_parameter);
		c.type_parameter = get_int_width(tp);
	}


};      // end struct collection_type_manager

//=============================================================================
// class int_instance method definitions

int_instance::int_instance() : persistent(), back_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance::~int_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::collect_transient_info(persistent_object_manager& m) const {
	// register me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// load me!
}

//=============================================================================
// typedef int_instance_alias_base function definitions

ostream&
operator << (ostream& o, const int_instance_alias_base& i) {
	return o << "int-alias @ " << &i;
}

//=============================================================================
// explicit template instantiations

template class instance_collection<int_tag>;
template class instance_array<int_tag, 0>;
template class instance_array<int_tag, 1>;
template class instance_array<int_tag, 2>;
template class instance_array<int_tag, 3>;
template class instance_array<int_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_ART_OBJECT_INSTANCE_INT_CC__

