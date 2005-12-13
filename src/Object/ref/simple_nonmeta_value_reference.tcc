/**
	\file "Object/ref/simple_nonmeta_value_reference.cc"
	Class method definitions for semantic expression.  
	This file was reincarnated from 
		"Object/art_object_nonmeta_value_reference.cc"
 	$Id: simple_nonmeta_value_reference.tcc,v 1.5 2005/12/13 04:15:37 fang Exp $
 */

#ifndef	__OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__
#define	__OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__

// flags for controlling conditional compilation, mostly for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <exception>
#include <algorithm>

#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/traits/class_traits.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/type/data_type_reference.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
using namespace util::memory;
#include "util/using_ostream.h"
USING_STACKTRACE
using util::persistent_traits;

//=============================================================================
// class simple_nonmeta_value_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::simple_nonmeta_value_reference() :
		parent_type(), interface_type(), value_collection_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::simple_nonmeta_value_reference(
		const value_collection_ptr_type pi) :
		parent_type(), 
		interface_type(), 
		value_collection_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::~simple_nonmeta_value_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_inst_base(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::value_collection_ptr_type
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_inst_base_subtype(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::dump(ostream& o,
		const expr_dump_context& c) const {
	return grandparent_type::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::dimensions(void) const {
	return grandparent_type::dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const data_type_reference>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_data_type_ref(void) const {
	return data_type_resolver<Tag>()(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: reserve for later, in graph construction.  
	If both this and argument are instance references, 
	we consider them equivalent if they reference the same position
	parameter in the template formals list.  
	This allows us to correctly compare the equivalence of 
	template signatures whose member depend on template parameters.  
	\return true if boolean instance references are equivalent.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::must_be_equivalent(
		const data_expr_base_type& b) const {
	const this_type* const br = IS_A(const this_type*, &b);
	if (br) {
		// compare template formal parameter positions for equivalence!
		// INVARIANT (2005-01-30): if they are both template formals, 
		// then they refer to equivalent owners.  
		// This will not be true if the language allows nested 
		// templates, so beware in the distant future!

		// check owner pointer equivalence? not pointer equality!
		// same qualified name, namespace path...
		const size_t lpos =
			value_collection_ref->is_template_formal();
		const size_t rpos =
			br->value_collection_ref->is_template_formal();
		if (lpos && rpos && (lpos == rpos)) {
			if (this->array_indices && br->array_indices) {
				return this->array_indices->
					must_be_equivalent_indices(
						*br->array_indices);
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else {
		// conservatively
		return false;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {  
	collect_transient_info_base(m);
	value_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void    
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, value_collection_ref);
	write_object_base(m, f);
}
	
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Loads the instance reference from an input stream, translating
	indices to pointers.
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, value_collection_ref);
	NEVER_NULL(value_collection_ref);
	m.load_object_once(
		const_cast<value_collection_type*>(&*value_collection_ref));
	load_object_base(m, f);
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__

