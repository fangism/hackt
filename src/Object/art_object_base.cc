/**
	\file "art_object_base.cc"
	Method definitions for base classes for semantic objects.  
 	$Id: art_object_base.cc,v 1.25 2004/12/07 02:22:06 fang Exp $
 */

#include <iostream>
#include <numeric>

#include "STL/list.tcc"

#include "art_object_base.h"
#include "art_object_expr.h"		// for dynamic_param_expr_list
#include "art_object_inst_ref_base.h"
#include "art_object_assign.h"
#include "art_object_connect.h"
#include "art_object_definition_base.h"

//=============================================================================

// whether or not unimplemented objects should be written/loaded to/from file. 
// #define	USE_UNDEFINED_OBJECTS		1

//=============================================================================
namespace ART {
namespace entity {
using std::accumulate;
#include "using_ostream.h"
using namespace util::memory;

// for function compositions
// using namespace ADS;

//=============================================================================
// general non-member function definitions

//=============================================================================
// class object method definitions

//=============================================================================
// class object_handle method definitions

object_handle::object_handle(never_ptr<const object> o) :
		object(), obj(*o) {
	assert(!o.is_a<const object_handle>());
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

object_list::object_list() : object(), parent() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object_list::~object_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_list::what(ostream& o) const {
	return o << "object-list";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_list::dump(ostream& o) const {
	what(o) << ":" << endl;
	parent::const_iterator i = begin();
	for ( ; i!=end(); i++)
		dump(o) << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an instance_collection_item.  
	The restriction with this version is that each item must
	be a single pint_expr, not a range.  
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_formal_dense_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_pint_expr = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		if (!p) {
			is_pint_expr = false;
			cerr << "non-int expression found where single int "
				"is expected in dense range declaration.  "
				"ERROR!  " << endl;	// where?
			err = true;
		} else {
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->may_be_initialized()) {
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is not initialized.  "
					"ERROR!  " << endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			assert(p->is_loop_independent());
			assert(p->is_unconditional());
		}
	}
	if (err) {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			const int n = j->is_a<const pint_expr>()
				->static_constant_int();
			if (n <= 0) {
				cerr << "Integer size for a dense array "
					"must be positive, but got: "
					<< n << ".  ERROR!  " << endl;
				// where? callers figure out...
				return count_ptr<const_range_list>(NULL);
			}
			ret->push_back(const_range(n));
		}
		return ret;
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			ret->push_back(count_ptr<pint_range>(
				new pint_range(j->is_a<const pint_expr>())));
		}
		return ret;
	} else {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an instance_collection_item.  
	No restriction on this, may be single integer or pint_range.  
	Will automatically convert to ranges.
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_sparse_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_valid_range = true;
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
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
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
				err = true;
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
				err = true;
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
				err = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			assert(!i->is_a<pint_const>());
			assert(!i->is_a<const_range>());
			is_valid_range = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err = true;
		}
	}
	if (err || !is_valid_range) {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			count_ptr<pint_expr> p(j->is_a<pint_expr>());
			count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_int();
				if (n <= 0) {
					cerr << "Integer size for a dense array "
						"must be positive, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return count_ptr<const_range_list>(NULL);
				}
				ret->push_back(const_range(n));
			} else {
				assert(r);
				if (!r->is_sane()) {
					cerr << "Range is not valid. ERROR!  "
						<< endl;
					return count_ptr<const_range_list>(NULL);
				}
				count_ptr<const_range> cr(
					r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(const_range(*cr));
				} else {
					count_ptr<pint_range> pr(
						r.is_a<pint_range>());
					assert(pr);
					assert(pr->is_static_constant());
					ret->push_back(
						pr->static_constant_range());
				}
			}
		}
		return ret;
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				ret->push_back(count_ptr<pint_range>(
					new pint_range(j->is_a<pint_expr>())));
			} else {
				assert(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return ret;
	} else {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	First half should be similar to make_sparse_range_list.  
	At the end build an index_list instead.  
 */
excl_ptr<index_list>
object_list::make_index_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
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
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
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
				err = true;
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
				err = true;
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
				err = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			assert(!i->is_a<pint_const>());
			assert(!i->is_a<const_range>());
			is_valid_index = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err = true;
		}
	}
	if (err || !is_valid_index) {
		cerr << "Failed to construct an index list!  "
			<< endl;
		return excl_ptr<index_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		excl_ptr<const_index_list> ret(new const_index_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			count_ptr<pint_expr> p(j->is_a<pint_expr>());
			count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_int();
				if (n < 0) {
					cerr << "Integer for an index must "
						"be non-negative, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return excl_ptr<index_list>(NULL);
				}
				ret->push_back(count_ptr<pint_const>(
					new pint_const(n)));
			} else {
				assert(r);
				if (!r->is_sane()) {
					cerr << "Index-range is not valid.  "
						"ERROR!  " << endl;
					return excl_ptr<index_list>(NULL);
				}
				count_ptr<const_range> cr(
					r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(
						count_ptr<const_index>(
							new const_range(*cr)));
				} else {
					count_ptr<pint_range> pr(
						r.is_a<pint_range>());
					assert(pr);
					assert(pr->is_static_constant());
					ret->push_back(count_ptr<const_index>(
						new const_range(
						pr->static_constant_range())));
				}
			}
		}
		return excl_ptr<index_list>(ret);
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
				assert(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return excl_ptr<index_list>(ret);
	} else {
		cerr << "Failed to construct a index list!  "
			<< endl;
		return excl_ptr<index_list>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Converts a list into a param_expression_assignment object.  
	This is non-const because assignment requires that we 
	initialize parameter instance references, which requires 
	modification.  
	\return newly constructed param_expresion_assignment 
		if there are no errors, else NULL.  
		(May change with more sophisticated error-handling.)
 */
excl_ptr<param_expression_assignment>
object_list::make_param_assignment(void) {
	typedef	excl_ptr<param_expression_assignment>	return_type;
	bool err = false;
	// right-hand-side source expression
	const parent::value_type& last_obj = back();
	const count_ptr<const param_expr>
		rhse = last_obj.is_a<const param_expr>();
	assert(rhse);

	excl_ptr<param_expression_assignment> ret;

	// later, fold these error messages into static constructor?
	if (!last_obj) {
		cerr << "ERROR: rhs of expression assignment "
			"is malformed (null)" << endl;
		return return_type(NULL);
	} else if (rhse) {
		// last term must be initialized or be dependent on formals
		// if collective, conservative: may-be-initialized
		ret = param_expr::make_param_expression_assignment(rhse);
		assert(ret);
	} else {
		last_obj->what(
			cerr << "ERROR: rhs is unexpected object: ") << endl;
		return return_type(NULL);
	}

	param_expression_assignment::instance_reference_appender
		append_it(*ret);
	const iterator dest_end = --end();
	iterator dest_iter = begin();
	err = accumulate(dest_iter, dest_end, err, append_it);

	// if there are any errors, discard everything?
	// later: track errors in partially constructed objects
	if (err) return return_type(NULL);
	else	return ret;		// is ok
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Doesn't do any checking, just re-wraps the list.  
	Now does checking for static constants.  
	Objects may be null, but all others must be param_expr.  
	For now always return a dynamic list.
 */
excl_ptr<dynamic_param_expr_list>
object_list::make_param_expr_list(void) const {
	// first walk to determine if it qualified as a const_param_expr_list
	const_iterator i = begin();
	excl_ptr<dynamic_param_expr_list>
		ret(new dynamic_param_expr_list);
	for ( ; i!=end(); i++) {
		count_ptr<const object> o(*i);
		count_ptr<const param_expr> pe(o.is_a<const param_expr>());
		if (o)	assert(pe);
		ret->push_back(pe);	// NULL is ok.  
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an alias connection object, given a list of instance
	references.  Performs type-checking.  
 */
excl_ptr<const aliases_connection>
object_list::make_alias_connection(void) const {
	excl_ptr<aliases_connection>
		ret(new aliases_connection);
	const_iterator i = begin();
	assert(size() > 1);		// else what are you connecting?
	count_ptr<const object> fo(*i);
	count_ptr<const instance_reference_base>
		fir(fo.is_a<const instance_reference_base>());
	// keep this around for type-checking comparisons
	ret->append_instance_reference(fir);
	// starting with second instance reference, type-check and alias
	int j = 2;
	for (i++; i!=end(); i++, j++) {
		count_ptr<const object> o(*i);
		assert(o);
		count_ptr<const instance_reference_base>
			ir(o.is_a<const instance_reference_base>());
		assert(ir);
		if (!fir->may_be_type_equivalent(*ir)) {
			cerr << "ERROR: type/size of instance reference " 
				<< j << " of alias list doesn't match the "
				"type/size of the first instance reference!  "
				<< endl;
			return excl_ptr<const aliases_connection>(NULL);
		} else {
			ret->append_instance_reference(ir);
		}
	}
	return excl_ptr<const aliases_connection>(ret);	// const-ify
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a port connection object, given an invoking instance
	reference and a list of port actuals (instance references).
	\param ir the invoking instance to which port should connect.  
 */
excl_ptr<const port_connection>
object_list::make_port_connection(
		count_ptr<const simple_instance_reference> ir) const {
//	cerr << "In object_list::make_port_connection(): FINISH ME!" << endl;
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
	} else if (base_def->certify_port_actuals(*this)) {
		const_iterator i = begin();
		for ( ; i!=end(); i++) {
			count_ptr<const instance_reference_base>
				ir(i->is_a<const instance_reference_base>());
			ret->append_instance_reference(ir);
		}
		return return_type(ret);
	} else {
		cerr << "At least one error in port connection.  " << endl;
		return return_type(NULL);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

