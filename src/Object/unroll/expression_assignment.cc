/**
	\file "Object/unroll/expression_assignment.cc"
	Method definitions pertaining to connections and assignments.  
	This file was moved from "Object/art_object_assign.cc".
 	$Id: expression_assignment.cc,v 1.5.2.1 2006/02/19 03:53:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_CC__
#define	__HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_CC__

#define	ENABLE_STACKTRACE		0
#define	DEBUG_LIST_VECTOR_POOL		0

#include <iostream>
#include <numeric>

#include "Object/unroll/param_expression_assignment.h"
#include "Object/unroll/expression_assignment.tcc"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_collection.h"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/ref/meta_value_reference_base.h"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::pint_expression_assignment, 
		"pint-expression-assignment")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_expression_assignment, 
		"pbool-expression-assignment")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_expression_assignment, 
		"preal-expression-assignment")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pbool_expression_assignment,
		PBOOL_EXPR_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_expression_assignment,
		PINT_EXPR_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_expression_assignment,
		PREAL_EXPR_ASSIGNMENT_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class param_expression_assignment method definitions
	
param_expression_assignment::param_expression_assignment() :
		instance_management_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expression_assignment::~param_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o is already a pointer to meta_instance_reference_base.  
	\return TRUE if there is an error condition in this iteration
		or earlier.
 */
bad_bool
param_expression_assignment::meta_instance_reference_appender::operator () (
		const bad_bool b, const arg_type& o) {
	index++;
	if (!o) {
		cerr << "ERROR: in creating item " << index <<
			" of assign-list." << endl;
		return bad_bool(true);
	}
	const dest_ptr_type i(o.is_a<meta_value_reference_base>());
	if (!i) {
		cerr << "ERROR: unhandled case for item " << index <<
			" of assign-list." << endl;
		return bad_bool(true);
	}
	return ex_ass.append_simple_param_meta_value_reference(i) || b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints error to stderr if dimensions don't match.  
	\todo later redirect output to some error stream context.  
	\param s pointer to the destination of assignment.  
	\return true if dimensions of the source matches d.  
 */
good_bool
param_expression_assignment::validate_dimensions_match(
		const dest_const_ptr_type& s, const size_t d) const {
	const size_t s_dim = s->dimensions();
	if (s_dim != d) {
		cerr << "ERROR: dimensions of expression " << size() +1 <<
			" (" << s_dim << ") doesn't match that of the rhs ("
			<< d << ")." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks that the referenced instance is not already initialized.
	Only checks for scalars and not multidimensional collections.  
 */
good_bool
param_expression_assignment::validate_reference_is_uninitialized(
		const dest_const_ptr_type& s) const {
	if (s->must_be_initialized()) {
		// definitely initialized or formal
		cerr << "ERROR: expression " << size() +1 <<
			"is already initialized!" << endl;
		// don't care if it's same value... still an error
		return good_bool(false);
	} else	return good_bool(true);
}

//=============================================================================
// explicit template instantiation

template class expression_assignment<pbool_tag>;
template class expression_assignment<pint_tag>;
template class expression_assignment<preal_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_CC__

