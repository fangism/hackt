/**
	\file "Object/unroll/expression_assignment.cc"
	Method definitions pertaining to connections and assignments.  
	This file was moved from "Object/art_object_assign.cc".
 	$Id: expression_assignment.cc,v 1.11 2011/04/02 01:46:12 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_LIST_VECTOR_POOL		0

#include <iostream>
#include <numeric>

#include "Object/unroll/param_expression_assignment.h"
#include "Object/unroll/expression_assignment.tcc"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/pint_instance.h"
#include "Object/inst/preal_instance.h"
#include "Object/inst/pstring_instance.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_collection.h"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/traits/pstring_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/ref/meta_value_reference_base.h"

//=============================================================================
namespace util {
using HAC::entity::pint_expression_assignment;
using HAC::entity::pbool_expression_assignment;
using HAC::entity::preal_expression_assignment;
using HAC::entity::pstring_expression_assignment;

SPECIALIZE_UTIL_WHAT(pint_expression_assignment, 
		"pint-expression-assignment")
SPECIALIZE_UTIL_WHAT(pbool_expression_assignment, 
		"pbool-expression-assignment")
SPECIALIZE_UTIL_WHAT(preal_expression_assignment, 
		"preal-expression-assignment")
SPECIALIZE_UTIL_WHAT(pstring_expression_assignment, 
		"pstring-expression-assignment")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pbool_expression_assignment, PBOOL_EXPR_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pint_expression_assignment, PINT_EXPR_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	preal_expression_assignment, PREAL_EXPR_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pstring_expression_assignment, PSTRING_EXPR_ASSIGNMENT_TYPE_KEY, 0)
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
	return bad_bool(ex_ass.append_simple_param_meta_value_reference(i).bad
		|| b.bad);
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

//=============================================================================
// explicit template instantiation

template class expression_assignment<pbool_tag>;
template class expression_assignment<pint_tag>;
template class expression_assignment<preal_tag>;
template class expression_assignment<pstring_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

