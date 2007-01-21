/**
	\file "Object/inst/channel_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: channel_collection_type_manager.tcc,v 1.2 2007/01/21 05:59:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/channel_collection_type_manager.h"
// #include "Object/type/data_type_reference.h"
// #include "Object/def/enum_datatype_def.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include <iostream>
#include "util/persistent_object_manager.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

#define CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE	\
template <class Tag>

#define	CHANNEL_COLLECTION_TYPE_MANAGER_CLASS			\
channel_collection_type_manager<Tag>

//=============================================================================
/**
	Pretty-print functor.  
 */
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
struct CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::dumper {
	ostream& os;

	dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
#if BUILTIN_CHANNEL_FOOTPRINTS
		os << traits_type::tag_name << ' ';
		return c.type_parameter.dump(os);
#else
		return os << traits_type::tag_name << ' ' <<
			c.type_parameter->get_qualified_name();
#endif
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
#if BUILTIN_CHANNEL_FOOTPRINTS
	this->type_parameter.collect_transient_info_base(m);
#else
	if (this->type_parameter)
		this->type_parameter->collect_transient_info(m);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
#if BUILTIN_CHANNEL_FOOTPRINTS
	this->type_parameter.write_object_base(m, o);
#else
	m.write_pointer(o, this->type_parameter);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
#if BUILTIN_CHANNEL_FOOTPRINTS
	this->type_parameter.load_object_base(m, i);
#else
	m.read_pointer(i, this->type_parameter);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
typename CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::resolved_type_ref_type
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::get_resolved_canonical_type(void) const {
	return resolved_type_ref_type(this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: called during connection checking.  
 */
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::
		must_be_collectibly_type_equivalent(const this_type& r) const {
	// must be typedef-resolved!
	return this->type_parameter.must_be_collectibly_type_equivalent(
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
 */
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bad_bool
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::check_type(
		const instance_collection_parameter_type& t) const {
	INVARIANT(this->type_parameter);
	return bad_bool(this->type_parameter != t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
	TODO: handle typedef'd enums!
 */
CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
good_bool
CHANNEL_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const instance_collection_parameter_type& t) {
	NEVER_NULL(t);
	INVARIANT(!this->type_parameter);
	this->type_parameter = t;
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef CHANNEL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef CHANNEL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_TCC__

