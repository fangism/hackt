/**
	\file "AST/art_parser_range.cc"
	Class method definitions for ART::parser, 
	related to ranges and range lists.  
	$Id: art_parser_range.cc,v 1.1.2.1 2005/05/12 00:43:48 fang Exp $
 */

#ifndef	__AST_ART_PARSER_RANGE_CC__
#define	__AST_ART_PARSER_RANGE_CC__

#include <exception>
#include <iostream>

#include "AST/art_parser_range_list.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_node_list.tcc"

// will need these come time for type-checking
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_expr.h"

#include "util/what.h"

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
SPECIALIZE_UTIL_WHAT(ART::parser::range, "(range)")
}

//=============================================================================
namespace ART {
using namespace entity;

namespace parser {
#include "util/using_ostream.h"

//=============================================================================
// class range method definitions

CONSTRUCTOR_INLINE
range::range(const expr* l) : lower(l), upper(NULL) {
	NEVER_NULL(lower); 
}

CONSTRUCTOR_INLINE
range::range(const expr* l, 
//		const string_punctuation_type* o, 
		const expr* u) : 
		lower(l), upper(u) {
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

/**
	Both expressions of the range should be of type pint.  
	No collective expressions, only single pints.  
	Do we check for constant cases?
	TO DO: finish me
	How do we interpret x[i]?
	\param c the context where to start resolving identifiers.  
	\return I don't know.
 */
never_ptr<const object>
range::check_build(context& c) const {
	never_ptr<const object>
		o(lower->check_build(c));	// useless return value
	// puts param_expr on stack
	const count_ptr<object> l(c.pop_top_object_stack());
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
		o = upper->check_build(c);
		// puts param_expr on stack
		// grab the last two expressions, 
		// check that they are both pints, 
		// and make a range object
		const count_ptr<object> u(c.pop_top_object_stack());
		const count_ptr<pint_expr> up(u.is_a<pint_expr>());
		if (u) {
			if (!up) {
				cerr << "Expression is not a pint-type, "
					"ERROR!  " << where(*upper) << endl;
				THROW_EXIT;
			}
			// check if expression is initialized
		} else {
			cerr << "Error resolving expression " << where(*upper)
				<< endl;
			THROW_EXIT;
		}
		// at this point, is ok

		// later: resolve constant if possible...
		// modularize this into a method in art_object_expr
		if (lp->is_static_constant() && up->is_static_constant()) {
			const int lb = lp->static_constant_value();
			const int ub = up->static_constant_value();
			if (lb > ub) {
				// error!  can't construct invalid const_range
				cerr << "Lower bound of range " <<
					where(*lower) << " is greater than "
					"upper bound " << where(*upper) <<
					".  ERROR!" << endl;
				c.push_object_stack(
					count_ptr<const_range>(NULL));
				// THROW_EXIT;
				// or let error get caught elsewhere?
			} else {
				// make valid constant range
				c.push_object_stack(count_ptr<const_range>(
					new const_range(lb, ub)));
			}
		} else {
			// can't determine validity of bounds statically
			c.push_object_stack(count_ptr<pint_range>(
				new pint_range(lp, up)));
		}
	} else {
		// but check that it is of type pint
		// and push it back onto stack
		// the caller will interpret it
		c.push_object_stack(l);
		// passing lp: ART::entity::object ambiguous base class
		//	of pint_expr :S
	}
	// not done yet
	return o;
}

range::return_type
range::check_index(context& c) const {
	const expr::return_type l(lower->check_expr(c));
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
		const expr::return_type u(upper->check_expr(c));
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
		// modularize this into a method in art_object_expr
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

//=============================================================================
// class range_list method definitions

range_list::range_list(const range* r) : parent_type(r) {
}

range_list::~range_list() { }

/**
	Note: limited to 4 dimensions.  

	Parent's check_build will result in each index dimension
	being pushed onto the context's object stack.  
	Grab them off the stack to form an object list.  

	Or just convert directly to a range list?
	No range list has two different semantics
	(depending on instantiation vs. reference),
	so just leave as object_list.  

	Should we take this opportunity to const-ify as much as possible?

	IMPORTANT:
	Only puts an object_list onto object_stack.  
	It is up to caller to interpret either as a sparse_range list
	in the case of an array declaration or as an index_list in the
	case of an indexed instance reference.  

	This is called by instance_array::check_build and 
	index_expr::check_build.  

	\return NULL, useless.
 */
never_ptr<const object>
range_list::check_build(context& c) const {
	parent_type::check_build(c);
	const count_ptr<object_list> ol(new object_list);
	size_t i = 0;
	for ( ; i<size(); i++) {
		const count_ptr<object> o(c.pop_top_object_stack());
		if (!o) {
			cerr << "Problem with dimension " << i+1 <<
				" of sparse_range_list between "
				<< where(*this) << endl;
			THROW_EXIT;		// terminate?
		} else if (!o.is_a<ART::entity::index_expr>()) {
			// pint_const *should* => index_expr ...
			// make sure each item is a range expression
			cerr << "Expression in dimension " << i+1 <<
				" of sparse_range_list is not valid!  "
				<< where(*this) << endl;
//			o->what(cerr << "object is a ") << endl;
//			o->dump(cerr << "object dump: ") << endl;
			THROW_EXIT;
		}
		// else o is an index_expr
		ol->push_front(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		c.push_object_stack(count_ptr<object>(NULL));
	} else {
//		c.push_object_stack(ol->make_sparse_range_list());
		// don't necessarily want to interpret as sparse_range
		// may want it as an index!
		c.push_object_stack(ol);
	}
	return never_ptr<const object>(NULL);
}

range_list::return_type
range_list::check_indices(context& c) const {
//	const return_type ret(new return_type::element_type);
//	NEVER_NULL(ret);
	typedef	std::vector<range::return_type> temp_type;
	temp_type temp;
	{
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		const range::return_type o((*i)->check_index(c));
		if (!o) {
			cerr << "Problem with dimension " << j <<
				" of sparse_range_list between "
				<< where(*this) << endl;
			THROW_EXIT;		// terminate?
			return return_type(NULL);
		}
#if 0
		else if (!o.is_a<entity::index_expr>()) {
			// pint_const *should* => index_expr ...
			// make sure each item is a range expression
			cerr << "Expression in dimension " << j <<
				" of sparse_range_list is not valid!  "
				<< where(*this) << endl;
//			o->what(cerr << "object is a ") << endl;
//			o->dump(cerr << "object dump: ") << endl;
			THROW_EXIT;
			return return_type(NULL);
		}
#endif
		// else o is an index_expr
		// collect first, store temporary, check for constness
		temp.push_back(o);
	}
	}

	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		return return_type(NULL);
	} else {
		count_ptr<dynamic_index_list>
			dyn_ret(new dynamic_index_list);
		NEVER_NULL(dyn_ret);
		bool is_const = true;
		temp_type::const_iterator i = temp.begin();
		const temp_type::const_iterator e = temp.end();
		for ( ; is_const && i!=e; i++) {
			dyn_ret->push_back(*i);
			if (!i->is_a<entity::const_index>()) {
				is_const = false;
			}
		}
		if (is_const) {
			count_ptr<const_index_list>
				const_ret(new const_index_list);
			NEVER_NULL(const_ret);
			// const_ret->reserve(size());
			for (i = temp.begin(); i!=e; i++) {
				const_ret->push_back(
					i->is_a<entity::const_index>());
			}
			return const_ret;
		}
		// don't necessarily want to interpret as sparse_range
		// may want it as an index!
		else return dyn_ret;
	}
}

//=============================================================================
// class dense_range_list method definitions

dense_range_list::dense_range_list(const expr* r) : parent_type(r) {
}

dense_range_list::~dense_range_list() {
}

/**
	Dense range lists are reserved for formal parameters and ports, 
	which must be dense arrays, cannot be sparse.  
	Limited to 4 dimensions.  
 */
never_ptr<const object>
dense_range_list::check_build(context& c) const {
	parent_type::check_build(c);
	const count_ptr<object_list> ol(new object_list);
	size_t i = 0;
	for ( ; i<size(); i++) {
		const count_ptr<object> o(c.pop_top_object_stack());
		if (!o) {
			cerr << "Problem with dimension " << i+1 <<
				" of dense_range_list between "
				<< where(*this) << endl;
			THROW_EXIT;		// terminate?
		} else if (!o.is_a<pint_expr>()) {
			// make sure that each item is an integer expr
			cerr << "Expression in dimension " << i+1 <<
				" of dense_range_list is not integer!  "
				<< where(*this) << endl;
			THROW_EXIT;
		}
		ol->push_front(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where(*this) << endl;
		c.push_object_stack(count_ptr<object>(NULL));
	} else {
		c.push_object_stack(ol->make_formal_dense_range_list());
	}
	return never_ptr<const object>(NULL);
}

/**
	Does basic checking of leaf nodes first.  
	\param temp sequence of intermediate type check results, 
		passed in as empty, populated and returned by reference.  
	\param c the parse context.  
 */
good_bool
dense_range_list::postorder_check(check_type& temp, context& c) const {
	INVARIANT(temp.empty());
	const_iterator i = begin();
	const const_iterator e = end();
	size_t j = 1;
	for ( ; i!=e; j++) {
		const expr::return_type o((*i)->check_expr(c));
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

dense_range_list::return_type
dense_range_list::check_formal_dense_ranges(context& c) const {
	check_type temp;
	if (!postorder_check(temp, c).good)
		return return_type(NULL);
	// copied from the old object_list::make_formal_dense_range_list()
	// initialize some bools to true
	// and set them false approriately in iterations
	bad_bool err(false);
	bool is_pint_expr = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	check_type::const_iterator i = temp.begin();
	const check_type::const_iterator e = temp.end();
	for ( ; i!=e; i++) {
		const count_ptr<pint_expr> p(i->is_a<pint_expr>());
		if (!p) {
			is_pint_expr = false;
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
			INVARIANT(p->is_loop_independent());
			INVARIANT(p->is_unconditional());
		}
	}
	if (err.bad) {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return return_type(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		const count_ptr<const_range_list>
			ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
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
		const_iterator j = begin();
		const count_ptr<dynamic_range_list>
			ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
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
template class node_list<const range>;			// range_list

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_RANGE_CC__

