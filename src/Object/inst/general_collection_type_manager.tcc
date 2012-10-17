/**
	\file "Object/inst/general_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: general_collection_type_manager.tcc,v 1.13 2010/04/07 00:12:39 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/general_collection_type_manager.hh"
#include "Object/type/canonical_type.hh"	// tcc later
#include "Object/def/footprint.hh"
#include <iostream>
#include "util/persistent_object_manager.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

#define GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	GENERAL_COLLECTION_TYPE_MANAGER_CLASS				\
general_collection_type_manager<Tag>

//=============================================================================
/**
	Pretty-print functor.  
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
struct GENERAL_COLLECTION_TYPE_MANAGER_CLASS::dumper {
	ostream& os;

	dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		os << traits_type::tag_name << ' ';
		c.type_parameter.dump(os);
		return os;
	}
};	// end struct dumper


GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
good_bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::complete_type_definition_footprint(
		const count_ptr<const const_param_expr_list>& r) const {
	if (this->is_relaxed_type() && r) {
		const instance_collection_parameter_type
			ct(this->type_parameter, r);
		// param r doesn't belong in temporary footprint
		const DECLARE_TEMPORARY_FOOTPRINT(fake_top);
		// don't have top-level footprint handy, need it?
		return ct.create_definition_footprint(fake_top);
	} else {
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (this->type_parameter)
		this->type_parameter.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	this->type_parameter.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	this->type_parameter.load_object_base(m, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
typename GENERAL_COLLECTION_TYPE_MANAGER_CLASS::type_ref_ptr_type
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::get_type(void) const {
	return this->type_parameter;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if the type is complete.
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::is_complete_type(void) const {
	return this->type_parameter;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::is_relaxed_type(void) const {
	return this->type_parameter.is_relaxed();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: called during connection checking.  
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::must_be_collectibly_type_equivalent(
		const this_type& r) const {
	// is this correct, calling connectibly_type_equivalent?
	return this->type_parameter.must_be_connectibly_type_equivalent(
		r.type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
	2005-07-07:
	TODO: given complete type t, can't distinguish between
		establishing strict type for the entire array
		vs. relaxed type with instance-specific 
		relaxed actuals!!!
		We choose to work around this in 
		instantiation_statement<>::unroll().
	NOW just type checks without committing.  
	TODO: RENAME
	TODO: argument should be same type as type_parameter?
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bad_bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::check_type(
		const instance_collection_parameter_type& t) const {
	INVARIANT(this->type_parameter);
	return bad_bool(!this->type_parameter
		.must_be_collectibly_type_equivalent(t));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
	TODO: argument should be same type as type parameter
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
good_bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const instance_collection_parameter_type& t) {
	NEVER_NULL(t);
	INVARIANT(!this->type_parameter);
	// static type canonical_type<> is already canonical and resolved!
	this->type_parameter = t;
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef GENERAL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__

