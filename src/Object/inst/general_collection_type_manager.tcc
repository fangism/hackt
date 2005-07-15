/**
	\file "Object/inst/general_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: general_collection_type_manager.tcc,v 1.1.2.2 2005/07/15 03:49:21 fang Exp $
 */

#ifndef	__OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__
#define	__OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/general_collection_type_manager.h"
#include <iostream>
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

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
		os << class_traits<Tag>::tag_name << ' ';
		c.type_parameter->dump(os) << '^' << c.get_dimensions();
		return os;
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (this->type_parameter)
		this->type_parameter->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, this->type_parameter);
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
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::is_relaxed_type(void) const {
	return this->type_parameter->is_relaxed();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: called during connection checking.  
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::must_match_type(
		const this_type& r) const {
	// must be const-resolved!
	return this->type_parameter->must_be_connectibly_type_equivalent(
		*r.type_parameter);
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
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bad_bool
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::commit_type(
		const type_ref_ptr_type& t) const {
	INVARIANT(this->type_parameter);
	return bad_bool(!this->type_parameter->
		must_be_collectibly_type_equivalent(*t));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
 */
GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
GENERAL_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const type_ref_ptr_type& t) {
	INVARIANT(!this->type_parameter);
	INVARIANT(t->is_resolved());
	INVARIANT(t->is_canonical());
	this->type_parameter = t;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef GENERAL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef GENERAL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_TCC__

