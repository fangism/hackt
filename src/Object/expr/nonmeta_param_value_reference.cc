/**
	\file "Object/expr/nonmeta_param_value_reference.cc"
	Nonmeta parameter value reference expressions.  
	Like references to arrays of constants with run-time index values.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: nonmeta_param_value_reference.cc,v 1.16 2011/04/02 01:45:59 fang Exp $
 */

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/extern_templates.hh"
#include "Object/ref/simple_nonmeta_value_reference.tcc"
#include "Object/traits/classification_tags.hh"
#include "Object/traits/value_traits.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/enum_traits.hh"
#include "Object/traits/struct_traits.hh"	// why? (nonmeta_expr_visitor)
#include "Object/traits/chan_traits.hh"		// why? (nonmeta_expr_visitor)
#include "Object/traits/proc_traits.hh"		// why? (nonmeta_expr_visitor)
#include "Object/persistent_type_hash.hh"
#include "Object/expr/struct_expr.hh"	// why? (nonmeta_expr_visitor)
#include "Object/expr/enum_expr.hh"	// why? (nonmeta_expr_visitor)
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/inst/pint_instance.hh"
#include "Object/inst/pbool_instance.hh"
#include "Object/inst/preal_instance.hh"
#include "Object/inst/pstring_instance.hh"
#include "Object/type/canonical_generic_datatype.hh"
#include "common/TODO.hh"
#include "util/stacktrace.hh"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
using HAC::entity::simple_pbool_nonmeta_instance_reference;
using HAC::entity::simple_pint_nonmeta_instance_reference;
using HAC::entity::simple_preal_nonmeta_instance_reference;
using HAC::entity::simple_pstring_nonmeta_instance_reference;

SPECIALIZE_UTIL_WHAT(simple_pbool_nonmeta_instance_reference,
		"nonmeta-pbool-inst-ref")
SPECIALIZE_UTIL_WHAT(simple_pint_nonmeta_instance_reference,
		"nonmeta-pint-inst-ref")
SPECIALIZE_UTIL_WHAT(simple_preal_nonmeta_instance_reference,
		"nonmeta-preal-inst-ref")
SPECIALIZE_UTIL_WHAT(simple_pstring_nonmeta_instance_reference,
		"nonmeta-pstring-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pbool_nonmeta_instance_reference, 
		SIMPLE_PBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pint_nonmeta_instance_reference, 
		SIMPLE_PINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_preal_nonmeta_instance_reference, 
		SIMPLE_PREAL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	simple_pstring_nonmeta_instance_reference, 
		SIMPLE_PSTRING_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
// specializations
// TODO: build specializations into proper class_traits

/**
        Specialized data type reference resolved for parameter ints, 
        which are promoted to int<32> in data context.  
	TODO: this is wrong, fix me!
	constant parameters can have arbitrary width.  
 */
template <>
struct data_type_resolver<pint_tag> {
	typedef	class_traits<pint_tag>		traits_type;
	typedef	traits_type::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
#if 0
		return int_traits::magic_int_type_ptr;
#else
		// equivalent, but consistent and programmatic
		return class_traits<traits_type::nonmeta_tag_type
			>::nonmeta_data_type_ptr;
#endif
	}

	/**
		Uses magic width of zero, 
		from "Object/traits/class_traits_type.cc".
	 */
	canonical_generic_datatype
	operator () (const data_value_reference_type&, 
			const unroll_context&) const {
		return data_type_reference::make_canonical_int_type_ref(0);
	}

};      // end struct data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<pbool_tag> {
	typedef	class_traits<pbool_tag>		traits_type;
	typedef	traits_type::simple_nonmeta_instance_reference_type
						data_value_reference_type;

	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
#if 0
		return bool_traits::built_in_type_ptr;
#else
		// equivalent, but consistent and programmatic
		return class_traits<traits_type::nonmeta_tag_type
			>::nonmeta_data_type_ptr;
#endif
	}

	canonical_generic_datatype
	operator () (const data_value_reference_type&, 
			const unroll_context&) const {
		return canonical_generic_datatype(
			bool_traits::built_in_type_ptr->get_base_datatype_def());
	}
};      // end struct data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<preal_tag> {
	typedef	class_traits<preal_tag>		traits_type;
	typedef	traits_type::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
#if 0
		return real_traits::built_in_type_ptr;
#else
#if 1
		FINISH_ME(Fang);
		return count_ptr<const data_type_reference>(NULL);
#else
		// equivalent, but consistent and programmatic
		return class_traits<traits_type::nonmeta_tag_type
			>::nonmeta_data_type_ptr;
#endif
#endif
	}

	canonical_generic_datatype
	operator () (const data_value_reference_type&, 
			const unroll_context&) const {
		FINISH_ME(Fang);
		return canonical_generic_datatype();
	}

};      // end struct data_type_resolver


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<pstring_tag> {
	typedef	class_traits<pstring_tag>		traits_type;
	typedef	traits_type::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
#if 0
		return string_traits::built_in_type_ptr;
#else
#if 1
		FINISH_ME(Fang);
		return count_ptr<const data_type_reference>(NULL);
#else
		// equivalent, but consistent and programmatic
		return class_traits<traits_type::nonmeta_tag_type
			>::nonmeta_data_type_ptr;
#endif
#endif
	}

	canonical_generic_datatype
	operator () (const data_value_reference_type&, 
			const unroll_context&) const {
		FINISH_ME(Fang);
		return canonical_generic_datatype();
	}

};      // end struct data_type_resolver


//=============================================================================
// explicit template instantiations

// maybe this belongs in "Object/art_object_data_expr.cc"?
template class simple_nonmeta_value_reference<pint_tag>;
template class simple_nonmeta_value_reference<pbool_tag>;
template class simple_nonmeta_value_reference<preal_tag>;
template class simple_nonmeta_value_reference<pstring_tag>;
// used to be simple_nonmeta_instance_reference<...>

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

