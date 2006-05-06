/**
	\file "AST/range.cc"
	Class method definitions for HAC::parser, 
	related to ranges and range lists.  
	$Id: range.cc,v 1.5 2006/05/06 22:08:12 fang Exp $
	This file used to be the following before it was renamed:
	$Id: range.cc,v 1.5 2006/05/06 22:08:12 fang Exp $
 */

#ifndef	__HAC_AST_RANGE_CC__
#define	__HAC_AST_RANGE_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <iterator>
#include <vector>

#include "AST/range_list.h"
#include "AST/token_char.h"
#include "AST/node_list.tcc"

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/expr/const_range.h"
#include "Object/expr/pint_range.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/int_range_expr.h"
#include "Object/expr/nonmeta_index_list.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug flags
#define	DEBUG_ID_EXPR	0

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::range, "(range)")
}

//=============================================================================
namespace HAC {
using namespace entity;
using std::back_inserter;

namespace parser {
#include "util/using_ostream.h"

//=============================================================================
// class range method definitions

CONSTRUCTOR_INLINE
range::range(const expr* l) : lower(l), upper(NULL) {
	NEVER_NULL(lower); 
}

CONSTRUCTOR_INLINE
range::range(const expr* l, const expr* u) : lower(l), upper(u) {
	NEVER_NULL(lower); NEVER_NULL(upper);
}

DESTRUCTOR_INLINE
range::~range() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(range)

line_position
range::leftmost(void) const {
	return lower->leftmost();
}

line_position
range::rightmost(void) const {
        if (upper)      return upper->rightmost();
        else            return lower->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Both expressions of the range should be of type pint.  
	No collective expressions, only single pints.  
	Do we check for constant cases?
	How do we interpret x[i]?
	\param c the context where to start resolving identifiers.  
	\return pointer to checked entity::index_expr.
 */
range::meta_return_type
range::check_meta_index(const context& c) const {
	typedef	meta_return_type	return_type;
	const expr::meta_return_type l(lower->check_meta_expr(c));
	const count_ptr<pint_expr> lp(l.is_a<pint_expr>());
	if (l) {
		if (!lp) {
			cerr << "Expression is not a pint-type, ERROR!  " <<
				where(*lower) << endl;
			THROW_EXIT;
		}
		// check if expression is initialized
	} else {
		cerr << endl;
		cerr << "Error resolving expression " << where(*lower)
			<< endl;
		THROW_EXIT;
	}

	if (upper) {
		const expr::meta_return_type u(upper->check_meta_expr(c));
		// puts param_expr on stack
		// grab the last two expressions, 
		// check that they are both pints, 
		// and make a range object
		const count_ptr<pint_expr> up(u.is_a<pint_expr>());
		if (u) {
			if (!up) {
				cerr << "Expression is not a pint-type, "
					"ERROR!  " << where(*upper) << endl;
				THROW_EXIT;
			}
			// check if expression is initialized
		} else {
			cerr << "Error resolving expression " <<
				where(*upper) << endl;
			THROW_EXIT;
		}
		// at this point, is ok

		// later: resolve constant if possible...
		// modularize this into a method in Object/expr/...
		if (lp->is_static_constant() && up->is_static_constant()) {
			const int lb = lp->static_constant_value();
			const int ub = up->static_constant_value();
			if (lb > ub) {
				// error!  can't construct invalid const_range
				cerr << "Lower bound of range " <<
					where(*lower) << " is greater than "
					"upper bound " << where(*upper) <<
					".  ERROR!" << endl;
				return return_type(NULL);
				// THROW_EXIT;
				// or let error get caught elsewhere?
			} else {
				// make valid constant range
				return return_type(new const_range(lb, ub));
			}
		} else {
			// can't determine validity of bounds statically
			return return_type(new pint_range(lp, up));
		}
	} else {
		return lp;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: write this comment
	Both expressions of the range should be of type pint.  
	No collective expressions, only single pints.  
	Do we check for constant cases?
	How do we interpret x[i]?
	\param c the context where to start resolving identifiers.  
	\return pointer to checked entity::index_expr.
 */
range::nonmeta_return_type
range::check_nonmeta_index(const context& c) const {
	typedef	nonmeta_return_type	return_type;
	const expr::nonmeta_return_type l(lower->check_nonmeta_expr(c));
	const count_ptr<int_expr> lp(l.is_a<int_expr>());
	if (l) {
		if (!lp) {
			cerr << "Expression is not a int-type, ERROR!  " <<
				where(*lower) << endl;
			return return_type(NULL);;
		}
		// check if expression is initialized
	} else {
		cerr << endl;
		cerr << "Error resolving expression " << where(*lower)
			<< endl;
		return return_type(NULL);;
	}

	if (upper) {
		const expr::nonmeta_return_type u(upper->check_nonmeta_expr(c));
		// puts param_expr on stack
		// grab the last two expressions, 
		// check that they are both pints, 
		// and make a range object
		const count_ptr<int_expr> up(u.is_a<int_expr>());
		if (u) {
			if (!up) {
				cerr << "Expression is not a int-type, "
					"ERROR!  " << where(*upper) << endl;
				return return_type(NULL);;
			}
			// check if expression is initialized
		} else {
			cerr << "Error resolving expression " <<
				where(*upper) << endl;
			return return_type(NULL);;
		}
		// at this point, is ok

		return return_type(new int_range_expr(lp, up));
	} else {
		return lp;
	}
}

//=============================================================================
// class range_list method definitions

range_list::range_list(const range* r) : parent_type(r) {
}

range_list::~range_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: limited to 4 dimensions.  

	Should we take this opportunity to const-ify as much as possible?

	This is called by instance_array::check_build and 
	index_expr::check_meta_reference.  

	\return success of type-check of all elements.  
 */
good_bool
range_list::postorder_check_meta(meta_check_type& temp,
		const context& c) const {
	STACKTRACE_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		const range::meta_return_type o((*i)->check_meta_index(c));
		if (!o) {
			cerr << "Problem with dimension " << j <<
				" of sparse_range_list between "
				<< where(*this) << endl;
			THROW_EXIT;		// terminate?
			return good_bool(false);
		}
		// collect first, store temporary, check for constness
		temp.push_back(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: limited to 4 dimensions.  

	Should we take this opportunity to const-ify as much as possible?

	This is called by instance_array::check_build and 
	index_expr::check_meta_reference.  

	\return success of type-check of all elements.  
 */
good_bool
range_list::postorder_check_nonmeta(nonmeta_check_type& temp, 
		const context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		const range::nonmeta_return_type
			o((*i)->check_nonmeta_index(c));
		if (!o) {
			cerr << "Problem with dimension " << j <<
				" of sparse_range_list between "
				<< where(*this) << endl;
			return good_bool(false);
		}
		// collect first, store temporary, check for constness
		temp.push_back(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return index_expr_list.
 */
range_list::checked_meta_indices_type
range_list::check_meta_indices(const context& c) const {
	typedef	checked_meta_indices_type	return_type;
	typedef	meta_check_type			check_type;
	STACKTRACE_VERBOSE;
	check_type temp;
#if 0
	// causes apple-gcc-3.3 ICE :(
	list_checker<check_type, parent_type, context&>
		lc(&range::check_meta_index, c);
	lc(temp, *this);
#endif
	if (!postorder_check_meta(temp, c).good) {
		// THROW_EXIT;
		return return_type(NULL);
	}
	const count_ptr<dynamic_meta_index_list>
		dyn_ret(new dynamic_meta_index_list(size()));
	NEVER_NULL(dyn_ret);
	bool is_const = true;
	check_type::const_iterator i(temp.begin());
	const check_type::const_iterator e(temp.end());
	for ( ; i!=e; i++) {
		dyn_ret->push_back(*i);
		if (!i->is_a<entity::const_index>()) {
			is_const = false;
		}
	}
	if (is_const) {
		const count_ptr<const_index_list>
			const_ret(new const_index_list(size()));
		// size is hint for reserving memory
		NEVER_NULL(const_ret);
		for (i = temp.begin(); i!=e; i++) {
			const_ret->push_back(i->is_a<entity::const_index>());
		}
		return const_ret;
	}
	// don't necessarily want to interpret as sparse_range
	// may want it as an index!
	else {
		return dyn_ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return nonmeta_index_list.
 */
range_list::checked_nonmeta_indices_type
range_list::check_nonmeta_indices(const context& c) const {
	typedef	checked_nonmeta_indices_type	return_type;
	typedef	nonmeta_check_type		check_type;
	check_type temp;
#if 0
	// causes apple-gcc-3.3 ICE :(
	list_checker<check_type, parent_type, context&>
		lc(&range::check_meta_index, c);
	lc(temp, *this);
#endif
	if (!postorder_check_nonmeta(temp, c).good) {
		// THROW_EXIT;
		return return_type(NULL);
	}

	const count_ptr<nonmeta_index_list>
		ret(new nonmeta_index_list(size()));
	NEVER_NULL(ret);
	copy(temp.begin(), temp.end(), back_inserter(*ret));
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a type-checked range-expression-list.
	No restriction on this, may be single integer or pint_range.
	Will automatically convert to ranges.
	Walks list twice, once to see what the best classification is,
	again to copy-create.
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.
	Cannot possibly be loop-dependent or conditional!
	\return newly allocated meta_range_list.
 */
range_list::checked_meta_ranges_type
range_list::check_meta_ranges(const context& c) const {
	typedef	checked_meta_ranges_type	return_type;
	typedef	meta_check_type			check_type;
	check_type temp;
	if (!postorder_check_meta(temp, c).good) {
		// THROW_EXIT;
		return return_type(NULL);
	}

	// copy from object_list::make_sparse_range_list()
	// initialize some bools to true
	// and set them false approriately in iterations
	bad_bool err(false);
	bool is_valid_range = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	check_type::const_iterator i(temp.begin());
	int k = 1;
	for ( ; i!=temp.end(); i++, k++) {
		if (!*i) {
			cerr << "Error in dimension " << k <<
				" of array indices.  " << endl;
			continue;
		}
		const count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		const count_ptr<meta_range_expr> r(i->is_a<meta_range_expr>());
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
			if (p->may_be_initialized()) {  // definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.
				cerr << "int expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;        // where?
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
			if (r->may_be_initialized()) {  // definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.
				cerr << "range expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;        // where?
				err.bad = true;
			}
		} else {
			// is neither pint_expr nor meta_range_expr
			INVARIANT(!i->is_a<pint_const>());
			INVARIANT(!i->is_a<const_range>());
			is_valid_range = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") <<
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;     // where?
			err.bad = true;
		}
	}
	if (err.bad || !is_valid_range) {
		cerr << "Failed to construct a sparse range list!  " << endl;
		return return_type(NULL);
	} else if (is_static_constant) {
		check_type::const_iterator j(temp.begin());
		const count_ptr<const_range_list>
			ret(new const_range_list(size()));
		for ( ; j!=temp.end(); j++) {
			// should be safe to do this, since we checked above
			const count_ptr<pint_expr> p(j->is_a<pint_expr>());
			const count_ptr<meta_range_expr>
				r(j->is_a<meta_range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_value();
				if (n <= 0) {
					cerr << "Integer size for a dense array "
						"must be positive, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return return_type(NULL);
				}
				ret->push_back(const_range(n));
			} else {
				NEVER_NULL(r);
				if (!r->is_sane()) {
					cerr << "Range is not valid. ERROR!  "
						<< endl;
					return return_type(NULL);
				}
				const count_ptr<const_range>
					cr(r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(const_range(*cr));
				} else {
					const count_ptr<pint_range>
						pr(r.is_a<pint_range>());
					NEVER_NULL(pr);
					INVARIANT(pr->is_static_constant());
					ret->push_back(
						pr->static_constant_range());
				}
			}
		}
		return ret;
	} else if (is_initialized) {
		check_type::const_iterator j(temp.begin());
		const count_ptr<dynamic_meta_range_list>
			ret(new dynamic_meta_range_list(size()));
		for ( ; j!=temp.end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				ret->push_back(count_ptr<pint_range>(
					new pint_range(j->is_a<pint_expr>())));
			} else {
				INVARIANT(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return ret;
	} else {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return return_type(NULL);
	}
}

//=============================================================================
// class dense_range_list method definitions

dense_range_list::dense_range_list(const expr* r) : parent_type(r) {
}

dense_range_list::~dense_range_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does basic checking of leaf nodes first.  
	\param temp sequence of intermediate type check results, 
		passed in as empty, populated and returned by reference.  
	\param c the parse context.  
 */
good_bool
dense_range_list::postorder_check_meta(meta_check_type& temp, 
		const context& c) const {
	INVARIANT(temp.empty());
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		const expr::meta_return_type o((*i)->check_meta_expr(c));
		// accumulate multiple errors?
		if (!o) {
			cerr << "Problem with dimension " << j <<
				" of dense_range_list between "
				<< where(*this) << endl;
			THROW_EXIT;		// terminate?
			return good_bool(false);
		} else if (!o.is_a<pint_expr>()) {
			// make sure that each item is an integer expr
			cerr << "Expression in dimension " << j <<
				" of dense_range_list is not integer!  "
				<< where(*this) << endl;
			THROW_EXIT;
			return good_bool(false);
		}
		// HAVE EXPR, WANT RANGE
		temp.push_back(o);
	}
	// collect first, store temporary, check for constness?
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a type-checked meta_range_list.  
	The restriction with this version is that each item must
	be a single pint_expr, not a range.  
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
	\param c expression context.  
	\return pointer to newly allocated range expression list.  
 */
dense_range_list::meta_return_type
dense_range_list::check_formal_dense_ranges(const context& c) const {
	typedef	meta_return_type	return_type;
	typedef	meta_check_type		check_type;
	STACKTRACE("dense_range_list::check_formal_dense_ranges()");
	check_type temp;
	if (!postorder_check_meta(temp, c).good)
		return return_type(NULL);
	// copied from the old object_list::make_formal_dense_range_list()
	// initialize some bools to true
	// and set them false approriately in iterations
	bad_bool err(false);
	// bool is_pint_expr = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	check_type::const_iterator i(temp.begin());
	const check_type::const_iterator e(temp.end());
	for ( ; i!=e; i++) {
		STACKTRACE("for-loop");
		const count_ptr<pint_expr> p(i->is_a<pint_expr>());
		if (!p) {
			// is_pint_expr = false;
			cerr << "non-int expression found where single int "
				"is expected in dense range declaration.  "
				"ERROR!  " << endl;     // where?
			err.bad = true;
		} else {
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
			if (p->may_be_initialized()) {
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.
				cerr << "int expression is not initialized.  "
					"ERROR!  " << endl;     // where?
				err.bad = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
		}
	}
	if (err.bad) {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return return_type(NULL);
	} else if (is_static_constant) {
		STACKTRACE("is-constant");
		check_type::const_iterator j(temp.begin());
		const count_ptr<const_range_list>
			ret(new const_range_list(size()));
		for ( ; j!=temp.end(); j++) {
			// should be safe to do this, since we checked above
			INVARIANT(j->is_a<const pint_expr>());
			const int n = j->is_a<const pint_expr>()
				->static_constant_value();
			if (n <= 0) {
				cerr << "Integer size for a dense array "
					"must be positive, but got: "
					<< n << ".  ERROR!  " << endl;
				// where? callers figure out...
				return return_type(NULL);
			}
			ret->push_back(const_range(n));
		}
		return ret;
	} else if (is_initialized) {
		STACKTRACE("is-initialized");
		check_type::const_iterator j(temp.begin());
		const count_ptr<dynamic_meta_range_list>
			ret(new dynamic_meta_range_list(size()));
		for ( ; j!=temp.end(); j++) {
			// should be safe to do this, since we checked above
			ret->push_back(count_ptr<pint_range>(
				new pint_range(j->is_a<const pint_expr>())));
		}
		return ret;
	} else {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return return_type(NULL);
	}
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
// template class node_list<const range>;			// range_list

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_RANGE_CC__

