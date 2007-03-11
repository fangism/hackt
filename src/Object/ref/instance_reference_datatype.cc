/**
	\file "Object/ref/instance_reference_datatype.cc"
	Method definitions for datatype instance reference classes.
	This file was reincarnated from "Object/art_object_inst_ref_data.cc".
	$Id: instance_reference_datatype.cc,v 1.15 2007/03/11 16:34:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__
#define	__HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__

#define	ENABLE_STACKTRACE			0

#include "Object/ref/meta_instance_reference_subtypes.tcc"
#include "Object/ref/simple_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_instance_reference.tcc"
#include "Object/ref/member_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_value_reference.tcc"
#include "Object/ref/aggregate_meta_instance_reference.tcc"
#include "Object/inst/alias_empty.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/datatype_instance_placeholder.h"
#include "Object/type/canonical_generic_datatype.h"

#include "Object/inst/value_collection.h"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/pint_instance.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/pint_const.h"
// #include "Object/expr/enum_const.h"		// much later...
// #include "Object/expr/struct_const.h"	// much later...

#include "Object/unroll/alias_connection.h"
#include "Object/unroll/data_alias_connection_base.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/enum_expr.h"
#include "Object/expr/real_expr.h"		// why? (nonmeta_expr_visitor)
#include "Object/expr/struct_expr.h"
#include "Object/expr/const_range.h"

#include "Object/devel_switches.h"

// introduced by canonical_type
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"

#include "Object/persistent_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/traits/class_traits.h"
#include "Object/traits/preal_traits.h"	// why? (nonmeta_expr_visitor)
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.tcc"
#include "Object/unroll/port_connection_base.h"

namespace util {
using HAC::entity::int_tag;
using HAC::entity::bool_tag;

SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_int_meta_instance_reference, "int-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_bool_meta_instance_reference, "bool-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_enum_meta_instance_reference, "enum-inst-ref")
#if ENABLE_DATASTRUCTS
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datastruct_meta_instance_reference,
							"struct-inst-ref")
#endif

#if 0
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datatype_nonmeta_instance_reference,
		"data-nonmeta-inst-ref")
#endif
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_int_nonmeta_instance_reference,
		"int-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_bool_nonmeta_instance_reference,
		"bool-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_enum_nonmeta_instance_reference,
		"enum-nonmeta-inst-ref")
#if ENABLE_DATASTRUCTS
SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_datastruct_nonmeta_instance_reference,
		"struct-nonmeta-inst-ref")
#endif

SPECIALIZE_UTIL_WHAT(
	HAC::entity::int_member_meta_instance_reference,
		"int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::bool_member_meta_instance_reference,
		"bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::enum_member_meta_instance_reference,
		"enum-member-inst-ref")
#if ENABLE_DATASTRUCTS
SPECIALIZE_UTIL_WHAT(
	HAC::entity::datastruct_member_meta_instance_reference,
		"struct-member-inst-ref")
#endif

SPECIALIZE_UTIL_WHAT(
	HAC::entity::aggregate_int_meta_instance_reference,
		"int-agg.-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::aggregate_bool_meta_instance_reference,
		"bool-agg.-inst-ref")
SPECIALIZE_UTIL_WHAT(
	HAC::entity::aggregate_enum_meta_instance_reference,
		"enum-agg.-inst-ref")
#if ENABLE_DATASTRUCTS
SPECIALIZE_UTIL_WHAT(
	HAC::entity::aggregate_datastruct_meta_instance_reference,
		"struct-agg.-inst-ref")
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_int_meta_instance_reference, 
		SIMPLE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_bool_meta_instance_reference, 
		SIMPLE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_enum_meta_instance_reference, 
		SIMPLE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
#if ENABLE_DATASTRUCTS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datastruct_meta_instance_reference, 
		SIMPLE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif

#if 0
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datatype_nonmeta_instance_reference, 
		SIMPLE_DATATYPE_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_int_nonmeta_instance_reference, 
		SIMPLE_DINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_bool_nonmeta_instance_reference, 
		SIMPLE_DBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_enum_nonmeta_instance_reference, 
		SIMPLE_ENUM_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#if ENABLE_DATASTRUCTS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_datastruct_nonmeta_instance_reference, 
		SIMPLE_STRUCT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::int_member_meta_instance_reference, 
		MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::bool_member_meta_instance_reference, 
		MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_member_meta_instance_reference, 
		MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY, 0)
#if ENABLE_DATASTRUCTS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::datastruct_member_meta_instance_reference, 
		MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_int_meta_instance_reference, 
		AGGREGATE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_bool_meta_instance_reference, 
		AGGREGATE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_enum_meta_instance_reference, 
		AGGREGATE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
#if ENABLE_DATASTRUCTS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_datastruct_meta_instance_reference, 
		AGGREGATE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif

namespace memory {
// explicit template instantiations
using HAC::entity::simple_bool_meta_instance_reference;
template class count_ptr<const simple_bool_meta_instance_reference>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// policy specializations
// for simple_nonmeta_value_reference

template <>
struct data_type_resolver<bool_tag> {
	typedef	class_traits<bool_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const {
		// easy, no parameters!
		return bool_traits::built_in_type_ptr;
	}

	canonical_generic_datatype
	operator () (const data_value_reference_type&, 
			const unroll_context&) const {
		return canonical_generic_datatype(
			bool_traits::built_in_type_ptr->get_base_datatype_def());
	}

};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<int_tag> {
	typedef	class_traits<int_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	typedef	class_traits<int_tag>::instance_collection_generic_type
					instance_collection_type;

	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d) const {
		return d.get_inst_base_subtype()->get_unresolved_type_ref_subtype();
	}

	// traits_type::resolved_type_ref
	canonical_generic_datatype
	operator () (const data_value_reference_type& d, 
			const unroll_context& c) const {
		// need to do some real work... 
		// extract parameter from collection
		// which needs to be translated from the placeholder
		const never_ptr<const physical_instance_collection>
			pc(c.lookup_instance_collection(
				*d.get_inst_base_subtype()));
		NEVER_NULL(pc);		// for now
		const never_ptr<const instance_collection_type>
			dc(pc.is_a<const instance_collection_type>());
		NEVER_NULL(dc);		// for now
		return data_type_reference::make_canonical_int_type_ref(
			dc->__get_raw_type());
	}
};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct data_type_resolver<enum_tag> {
	typedef	class_traits<enum_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	typedef	class_traits<enum_tag>::instance_collection_generic_type
					instance_collection_type;
	/**
		Technically, enum types are not context dependent... yet.
		If they ever are, then properly lookup the 
		unrolled collection to deduce the type.  
	 */
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d) const {
		// leverange enum_instance_collection?
		return d.get_inst_base_subtype()->get_unresolved_type_ref_subtype();
	}

	/**
		This is a bit overkill... until enum types are actually
		template dependent (possible in future).  
	 */
	canonical_generic_datatype
	operator () (const data_value_reference_type& d, 
			const unroll_context& c) const {
		const never_ptr<const physical_instance_collection>
			pc(c.lookup_instance_collection(
				*d.get_inst_base_subtype()));
		NEVER_NULL(pc);		// for now
		const never_ptr<const instance_collection_type>
			dc(pc.is_a<const instance_collection_type>());
		NEVER_NULL(dc);		// for now
		return dc->get_resolved_canonical_type();
	}

};	// end class data_type_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if ENABLE_DATASTRUCTS
template <>
struct data_type_resolver<datastruct_tag> {
	typedef	class_traits<datastruct_tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	typedef	class_traits<datastruct_tag>::instance_collection_generic_type
					instance_collection_type;
	/**
		User defined data-types may be context-dependent, 
		depending on template parameters.  
	 */
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type& d
			) const {
		// leverange struct_instance_collection?
		return d.get_inst_base_subtype()->get_unresolved_type_ref_subtype();
	}

	canonical_generic_datatype
	operator () (const data_value_reference_type& d, 
			const unroll_context& c) const {
		const never_ptr<const physical_instance_collection>
			pc(c.lookup_instance_collection(
				*d.get_inst_base_subtype()));
		NEVER_NULL(pc);		// for now
		const never_ptr<const instance_collection_type>
			dc(pc.is_a<const instance_collection_type>());
		NEVER_NULL(dc);		// for now
		return dc->get_resolved_canonical_type();
	}

};	// end class data_type_resolver
#endif

//=============================================================================
#if 0
// template specializations of struct nonmeta_reference_type_check_policy

template <>
struct nonmeta_reference_type_check_policy<bool_tag> {
	typedef	bool_nonmeta_instance_reference		reference_type;
	static
	bool
	may_accept_expr_type(const reference_type&, const data_expr& d) {
		// this should cover bools and pbools (by subclass)
		return IS_A(const bool_expr*, &d);
	}
};	// end struct nonmeta_reference_type_check_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct nonmeta_reference_type_check_policy<int_tag> {
	typedef	int_nonmeta_instance_reference		reference_type;
	static
	bool
	may_accept_expr_type(const reference_type& r, const data_expr& d) {
		// this should cover int and pint
		const int_expr* i = IS_A(const int_expr*, &d);
		if (!i)	return false;
		// see if it references another int, if so, check width
#if 0
		const reference_type* rv = IS_A(const reference_type*, i);
		if (rv) {
		} else {
			// is some other 
			return true;
		}
#endif
	}
};	// end struct nonmeta_reference_type_check_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif

//=============================================================================
// class data_nonmeta_instance_reference method definitions

/**
	Using cross-casting: cross-over and cross-back.  
	Leveraging lvalue < rvalue fact.
 */
count_ptr<const data_nonmeta_instance_reference>
data_nonmeta_instance_reference::unroll_resolve_copy(const unroll_context& c,
		const count_ptr<const this_type>& p) const {
	INVARIANT(p == this);
	const count_ptr<const data_expr> d(p.is_a<const data_expr>());
	NEVER_NULL(d);
	return d->unroll_resolve_copy(c, d).is_a<const this_type>();
}

//=============================================================================
// explicit template instantiations

template class meta_instance_reference<bool_tag>;
template class meta_instance_reference<int_tag>;
template class meta_instance_reference<enum_tag>;
#if ENABLE_DATASTRUCTS
template class meta_instance_reference<datastruct_tag>;
#endif

template class simple_meta_instance_reference<bool_tag>;
template class simple_meta_instance_reference<int_tag>;
template class simple_meta_instance_reference<enum_tag>;
#if ENABLE_DATASTRUCTS
template class simple_meta_instance_reference<datastruct_tag>;
#endif

// template class simple_nonmeta_instance_reference<datatype_tag>;
// recently upgraded from simple_nonmeta_instance_reference
template class simple_nonmeta_value_reference<bool_tag>;
template class simple_nonmeta_value_reference<int_tag>;
template class simple_nonmeta_value_reference<enum_tag>;
#if ENABLE_DATASTRUCTS
template class simple_nonmeta_value_reference<datastruct_tag>;
#endif

template class member_meta_instance_reference<bool_tag>;
template class member_meta_instance_reference<int_tag>;
template class member_meta_instance_reference<enum_tag>;
#if ENABLE_DATASTRUCTS
template class member_meta_instance_reference<datastruct_tag>;
#endif

template class aggregate_meta_instance_reference<bool_tag>;
template class aggregate_meta_instance_reference<int_tag>;
template class aggregate_meta_instance_reference<enum_tag>;
#if ENABLE_DATASTRUCTS
template class aggregate_meta_instance_reference<datastruct_tag>;
#endif

// NOTE: nonmeta_value_reference<{pint,pbool,preal}_tag> are instantiated in
// "Object/expr/nonmeta_param_value_reference.cc"

// and my work is done!
//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_INSTANCE_REFERENCE_DATATYPE_CC__

