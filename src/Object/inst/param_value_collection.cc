/**
	\file "Object/inst/param_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file used to be "Object/art_object_instance_param.cc"
		in a previous life.  
 	$Id: param_value_collection.cc,v 1.17 2011/04/02 01:46:02 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)

#include <iostream>

#include "Object/common/namespace.h"
#include "Object/type/param_type_reference.h"
#include "Object/inst/param_value_collection.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/inst/param_value_placeholder.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"

#include "common/ICE.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;
using util::disable_indent;

//=============================================================================
// class param_value_collection method definitions

/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_value_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	NOTE: this is conservative and need not be precise.  
	\return true if dimensions *may* match.  
 */
good_bool
param_value_collection::may_check_expression_dimensions(
		const param_expr& pe) const {
	STACKTRACE_VERBOSE;
	// MUST_BE_A(const param_value_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  
	// shouldn't we just virtual get_dimensions here?
	const size_t dimensions = get_placeholder_base()->get_dimensions();
	const size_t pdim = pe.dimensions();
	if (dimensions != pdim) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
		/**
			We used to statically check dimensions for 
			early rejection, but the effort gave little benefit.  
		**/
		// be conservative
		return good_bool(true);
	} else {
		// dimensions == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		return good_bool(pdim == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_value_collection derivatives, 
		in the context of checking template formals against actuals
		when unrolling instantiations.  
	May be useful else where for connections.  
	This should really onle be called suring the unroll phase, 
		when instance collections are 'certain'.  
	NOTE: instantiation indices may depend on template parameters
		so they need to be unroll resolved!
	TODO: what if template formal size depends on template parameter!
	\return true if dimensions *may* match.  
 */
good_bool
param_value_collection::must_check_expression_dimensions(
		const const_param& pe, const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// MUST_BE_A(const param_value_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  
	const size_t dimensions = get_dimensions();
	if (dimensions != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
#if 1
		// true for formal parameters
		// number of dimensions doesn't even match!
		// this is true only if parameters that check this
		// are template formals.  
		// not sure if this will be called by non-formals, will see...

		INVARIANT(pe.has_static_constant_dimensions());
		const const_range_list d(pe.static_constant_dimensions());

		// make sure sizes in each dimension
		const index_collection_item_ptr_type
			mrl(get_placeholder_base()->get_initial_instantiation_indices());
		NEVER_NULL(mrl);
		const count_ptr<const const_range_list>
			crl(mrl.is_a<const const_range_list>());
		if (crl) {
			// return good_bool(*crl == d);
			return good_bool(crl->is_size_equivalent(d));
		} else {
			// is dynamic, conservatively return false
			// we're in trouble for template-dependent expressions
			// need unroll parameters!
			const_range_list _r;
			if (!mrl->unroll_resolve_rvalues(_r, c).good) {
				// there was error resolving parameters!
				// should this ever happen???
			ICE(cerr, 
				cerr << "Error resolving instantiation range "
					"for checking expression dimensions!"
					<< endl;
			)
				return good_bool(false);
			}
			return good_bool(_r.is_size_equivalent(d));
		}
#else
		const const_index_list dummy;
		const const_index_list cil(resolve_indices(dummy)); // virtual
		if (cil.empty()) {
			cerr << "Error resolving indices of value collection."
				<< endl;
			// already have error message?
			// was unable to resolve dense collection
			return good_bool(false);
		}
		const const_range_list crl(cil.collapsed_dimension_ranges());
		return good_bool(crl.is_size_equivalent(d));
#endif
	} else {
		return good_bool(pe.dimensions() == 0);
	}
}	// end method param_value_collection::must_check_expression_dimensions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Note: only one flavor needed (hopefully).  
	One should be able to statically determine whether or not
	something is loop-dependent.  
	Wait, does it even make sense for an "instantiation"?
	This should only be applicable to meta_instance_references...
	put this on hold...
 */
bool
param_value_collection::is_loop_independent(void) const {
	
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

