/**
	\file "Object/art_object_base.cc"
	Method definitions for base classes for semantic objects.  
 	$Id: art_object_base.cc,v 1.32.2.3 2005/05/13 06:44:37 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_BASE_CC__
#define	__OBJECT_ART_OBJECT_BASE_CC__

#include <iostream>
#include <numeric>

#include "util/STL/list.tcc"

#include "Object/art_object_base.h"
#include "Object/art_object_expr.h"		// for dynamic_param_expr_list
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_assign.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_base.h"

#include "util/boolean_types.h"

//=============================================================================

// whether or not unimplemented objects should be written/loaded to/from file. 
// #define	USE_UNDEFINED_OBJECTS		1

//=============================================================================
namespace ART {
namespace entity {
using std::accumulate;
#include "util/using_ostream.h"
using namespace util::memory;

// for function compositions
// using namespace ADS;

//=============================================================================
// general non-member function definitions

//=============================================================================
// class object method definitions

void
object::bogus(void) const { }

//=============================================================================
// class object_handle method definitions

/**
	Constructs an object handle, wrapping around a non-owned
	pointer to the canonical object.  
	\param o pointer to object to be wrapped, 
		which may not be another handle.  
 */
object_handle::object_handle(never_ptr<const object> o) :
		object(), obj(*o) {
	INVARIANT(!o.is_a<const object_handle>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_handle::what(ostream& o) const {
	return obj.what(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_handle::dump(ostream& o) const {
	return obj.dump(o << "(handle) ");
}

//=============================================================================
// class object_list method definitions

object_list::object_list() : object(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object_list::~object_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_list::what(ostream& o) const {
	return o << "object-list";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_list::dump(ostream& o) const {
	what(o) << ":" << endl;
	const_iterator i = begin();
	for ( ; i!=end(); i++)
		dump(o) << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	First half should be similar to make_sparse_range_list.  
	At the end build an index_list instead.  
	\return newly constructed index list.  
 */
excl_ptr<index_list>
object_list::make_index_list(void) const {
	typedef	excl_ptr<index_list>		return_type;
	// initialize some bools to true
	// and set them false approriately in iterations
	bad_bool err(false);
	bool is_valid_index = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	int k = 1;
	for ( ; i!=end(); i++, k++) {
		if (!*i) {
			cerr << "Error in dimension " << k <<
				" of array indices.  " << endl;
			continue;
		}
		const count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		const count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err.bad = true;
			}
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err.bad = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			// can be loop-independent, do we need to track?
			// can be conditional, do we need to track?
		} else if (r) {
			// same thing... copy
			if (r->dimensions() != 0) {
				cerr << "range expression must be 0-dimensional, "
					"but is actually " <<
					r->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err.bad = true;
			}
			if (r->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (r->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "range expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err.bad = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			INVARIANT(!i->is_a<pint_const>());
			INVARIANT(!i->is_a<const_range>());
			is_valid_index = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err.bad = true;
		}
	}
	if (err.bad || !is_valid_index) {
		cerr << "Failed to construct an index list!  "
			<< endl;
		return return_type(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		excl_ptr<const_index_list> ret(new const_index_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			const count_ptr<pint_expr> p(j->is_a<pint_expr>());
			const count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_value();
				if (n < 0) {
					cerr << "Integer for an index must "
						"be non-negative, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return return_type(NULL);
				}
				ret->push_back(count_ptr<pint_const>(
					new pint_const(n)));
			} else {
				NEVER_NULL(r);
				if (!r->is_sane()) {
					cerr << "Index-range is not valid.  "
						"ERROR!  " << endl;
					return return_type(NULL);
				}
				const count_ptr<const_range>
					cr(r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(
						count_ptr<const_index>(
							new const_range(*cr)));
				} else {
					const count_ptr<pint_range>
						pr(r.is_a<pint_range>());
					NEVER_NULL(pr);
					INVARIANT(pr->is_static_constant());
					ret->push_back(count_ptr<const_index>(
						new const_range(
						pr->static_constant_range())));
				}
			}
		}
		// transfers ownership
		return return_type(ret);
	} else if (is_initialized) {
		const_iterator j = begin();
		excl_ptr<dynamic_index_list> ret(new dynamic_index_list);
		for ( ; j!=end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				// check dimensionality!
				ret->push_back(j->is_a<pint_expr>());
			} else {
				// check dimensionality!
				INVARIANT(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return return_type(ret);
	} else {
		cerr << "Failed to construct a index list!  " << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Doesn't do any checking, just re-wraps the list.  
	Now does checking for static constants.  
	Objects may be null, but all others must be param_expr.  
	For now always return a dynamic list.
	\return newly allocated dynamic expression list.  
 */
excl_ptr<dynamic_param_expr_list>
object_list::make_param_expr_list(void) const {
	typedef	excl_ptr<dynamic_param_expr_list>	return_type;
	// first walk to determine if it qualified as a const_param_expr_list
	const_iterator i = begin();
	return_type ret(new dynamic_param_expr_list);
	for ( ; i!=end(); i++) {
		const count_ptr<const object> o(*i);
		const count_ptr<const param_expr>
			pe(o.is_a<const param_expr>());
		if (o)	INVARIANT(pe);
		ret->push_back(pe);	// NULL is ok.  
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !USE_NEW_CERTIFY_PORTS
/**
	Creates a port connection object, given an invoking instance
	reference and a list of port actuals (instance references).
	\param ir the invoking instance to which port should connect.  
 */
excl_ptr<const port_connection>
object_list::make_port_connection(
		const count_ptr<const simple_instance_reference>& ir) const {
	typedef	excl_ptr<const port_connection>		return_type;
	excl_ptr<port_connection>
		ret(new port_connection(ir));
	never_ptr<const definition_base>
		base_def(ir->get_base_def());

	const size_t ir_dim = ir->dimensions();
	if (ir_dim) {
		cerr << "Instance reference port connection must be scalar, "
			"but got a " << ir_dim << "-dim reference!" << endl;
		return return_type(NULL);
	} else if (base_def->certify_port_actuals(*this).good) {
		const_iterator i = begin();
		for ( ; i!=end(); i++) {
			count_ptr<const instance_reference_base>
				ir(i->is_a<const instance_reference_base>());
			ret->append_instance_reference(ir);
		}
		// transfers ownership
		return return_type(ret);
	} else {
		cerr << "At least one error in port connection.  " << endl;
		return return_type(NULL);
	}
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_BASE_CC__

