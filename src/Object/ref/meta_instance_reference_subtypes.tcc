/**
	\file "Object/ref/meta_instance_reference_subtypes.tcc"
	$Id: meta_instance_reference_subtypes.tcc,v 1.2 2006/02/21 04:48:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__

#include <iostream>
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/unroll/port_connection_base.h"
#include "Object/unroll/alias_connection.h"
#include "Object/type/fundamental_type_reference.h"
#include "util/macros.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class meta_instance_reference method definitions

/**
	Implementation copied-modified from 
	simple_meta_instance_reference_base::may_be_type_equivalent().
	Conservatively returns true.
	NOTE: does not attempt to reason about static dimensions of arrays.  
	TODO: be conservative w.r.t. aggregates?
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bool
META_INSTANCE_REFERENCE_CLASS::may_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	const this_type* rr(IS_A(const this_type*, &r));
	if (!rr) {
		// could be more descriptive...
		cerr << "Meta-class type does not match!  got: ";
		this->what(cerr) << " and: ";
		r.what(cerr) << endl;
		return false;
	}
#if 0
	const never_ptr<const instance_collection_base>
		lib(get_inst_base());
	const this_type* const smir = IS_A(const this_type*, &i);
	if (!smir) {
		cerr << "Unhandled case in simple_meta_instance_reference_base"
			"::may_be_type_equivalent(): "
			"comparing to non-simple_meta_instance_reference_base, "
			"probably intended for complex-aggregate instance "
			"references *grin*... returning false." << endl;
		return false;
	}
	const never_ptr<const instance_collection_base>
		rib(smir->get_inst_base());
#endif
	const count_ptr<const fundamental_type_reference>
		ltr(this->get_type_ref());
	const count_ptr<const fundamental_type_reference>
		rtr(rr->get_type_ref());
	const bool type_eq = ltr->may_be_connectibly_type_equivalent(*rtr);
	// if base types differ, then cannot be equivalent
	if (!type_eq) {
		ltr->dump(cerr << "Types do not match! got: ") << " and: ";
		rtr->dump(cerr) << "." << endl;
		return false;
	}
	// else they match, continue to check dimensionality and size.  

	// TO DO: factor this section code out to a method for re-use.  
	// note: is dimensions of the *reference* not the instantiation!
	const size_t lid = this->dimensions();
	const size_t rid = rr->dimensions();
	if (lid != rid) {
		cerr << "Dimensions do not match! got: " << lid
			<< " and: " << rid << "." << endl;
		return false;
	}
	// catch cases where one of them is scalar (zero-dimensional)
	if (!lid) {
		INVARIANT(!rid);
		return true;
	}
	// else fall-through handle multidimensional case
	return true;            // conservatively
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	<sarcasm>
	Shamelessly copied from 
	simple_meta_instance_reference_base::must_be_type_equivalent().
	</sarcasm>
	This is probably never called.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bool
META_INSTANCE_REFERENCE_CLASS::must_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<aliases_connection_base>
META_INSTANCE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<aggregate_meta_instance_reference_base>
META_INSTANCE_REFERENCE_CLASS::make_aggregate_meta_instance_reference_private(
		void) const {
	typedef	count_ptr<aggregate_meta_instance_reference_base>
							return_type;
	typedef	aggregate_meta_instance_reference<Tag>	aggregate_type;
	return return_type(new aggregate_type);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__

