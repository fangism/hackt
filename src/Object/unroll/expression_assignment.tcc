/**
	\file "Object/unroll/expression_assignment.tcc"
	Method definitions pertaining to connections and assignments.  
	This file came from "Object/art_object_assign.tcc"
		in a previoius life.  
 	$Id: expression_assignment.tcc,v 1.19 2006/11/21 05:00:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_TCC__
#define	__HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_TCC__

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL		0
#endif

#include <iostream>
#include <numeric>

#include "Object/unroll/expression_assignment.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_collection.h"
#include "Object/ref/meta_value_reference.h"

#include "util/what.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"
#include "util/stacktrace.h"
#include "util/packed_array.tcc"	// for ~value_reference_collection_type

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE
using std::mem_fun_ref;
using util::dereference;
using util::bind2nd_argval;
using util::persistent_traits;

//=============================================================================
// class expression_assignment method definitions

// LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(expression_assignment, ..)

EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
typename EXPRESSION_ASSIGNMENT_CLASS::pool_type
EXPRESSION_ASSIGNMENT_CLASS::pool(traits_type::assignment_chunk_size);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void*
EXPRESSION_ASSIGNMENT_CLASS::operator new (size_t s) {
	INVARIANT(sizeof(this_type) == s);
	return pool.allocate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void*
EXPRESSION_ASSIGNMENT_CLASS::operator new (size_t s, void* p) {
	INVARIANT(sizeof(this_type) == s);
	NEVER_NULL(p); return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void
EXPRESSION_ASSIGNMENT_CLASS::operator delete (void* p) {
	this_type* t = reinterpret_cast<this_type*>(p);
	NEVER_NULL(t);
	pool.deallocate(t);
}

//-----------------------------------------------------------------------------
/**
	Private empty constructor.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
EXPRESSION_ASSIGNMENT_CLASS::expression_assignment() :
		parent_type(), src(), dests() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s the right-most expression in the assignment, 
		called the source value. 
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
EXPRESSION_ASSIGNMENT_CLASS::expression_assignment(
		const src_const_ptr_type& s) :
		parent_type(), src(s), dests() {
	NEVER_NULL(this->src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
EXPRESSION_ASSIGNMENT_CLASS::~expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the number of left-expressions, or destinations.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
size_t
EXPRESSION_ASSIGNMENT_CLASS::size(void) const {
	return this->dests.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
ostream&
EXPRESSION_ASSIGNMENT_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out assignment statement with sequential expressions.  
	Consider using ostream_iterator and copy.  
	\param o the output stream.
	\return the output stream.
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
ostream&
EXPRESSION_ASSIGNMENT_CLASS::dump(ostream& o,
		const expr_dump_context& dc) const {
	NEVER_NULL(this->src);
	INVARIANT(!dests.empty());
	dumper dumpit(o, dc);
	for_each(this->dests.begin(), this->dests.end(), dumpit);
	return this->src->dump(o << " = ", dc) << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o the output stream.  
	\param i the initial index, used to suppress delimiter.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
EXPRESSION_ASSIGNMENT_CLASS::dumper::dumper(ostream& o,
		const expr_dump_context& c, 
		const size_t i) :
		index(i), os(o), _context(c) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When index is -, delimiter will not be printed.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void
EXPRESSION_ASSIGNMENT_CLASS::dumper::operator() (
		const typename dest_list_type::value_type& i) {
	NEVER_NULL(i);
	if (index) os << " = ";
	i->dump(os, _context);
	index++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a destination instance reference to the assignment list.  
	\param e new destination expression.  
	\return true if there is an error, else false.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
bad_bool
EXPRESSION_ASSIGNMENT_CLASS::append_simple_param_meta_value_reference(
		const typename parent_type::dest_ptr_type& e) {
	// cache the value of dimensions to avoid recomputation?
	NEVER_NULL(e);
	bad_bool err(false);
	size_t dim = this->src->dimensions();
	if (!this->validate_dimensions_match(e, dim).good)
		err.bad = true;
	dest_ptr_type pb(e.template is_a<value_reference_type>());
	if (!pb) {
		cerr << "ERROR: Cannot initialize a " <<
			traits_type::tag_name << " with a ";
		e->what(cerr) << " expression!" << endl;
		err.bad = true;
	}
	if (err.bad) {
		cerr << "Error initializing item " << size()+1 <<
			" of assign-list.  " << endl;
		this->dests.push_back(dest_ptr_type(NULL));
	} else {
		this->dests.push_back(pb);
	}
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns values for a single source-destination pair.  
	Called by __unroll().
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::assign_dest(const value_reference_type& lv, 
		const const_collection_type& rv, const unroll_context& c) {
	// lvalue reference collection
	value_reference_collection_type temp;
	if (lv.unroll_lvalue_references(c, temp).bad) {
		cerr << "ERROR: unrolling lvalue references in " <<
			traits_type::tag_name << " assignment." << endl;
		return good_bool(false);
	}
	typedef	typename value_reference_collection_type::key_type
					lvalue_key_type;
	typedef	typename const_collection_type::key_type
					rvalue_key_type;
	// these should be the same type
	const lvalue_key_type ls(temp.size());
	const rvalue_key_type rs(rv.array_dimensions());
	if (ls != rs) {
		cerr << "lvalue/rvalue references dimensions mismatch "
			"in " << traits_type::tag_name <<
			" assignment." << endl;
		cerr << "\tgot: " << ls << " and: " << rs << endl;
		return good_bool(false);
	}
	typedef	typename value_reference_collection_type::const_iterator
						lvalue_iterator;
	typedef	typename const_collection_type::const_iterator
						rvalue_iterator;
	lvalue_iterator li(temp.begin());
	rvalue_iterator ri(rv.begin()), re(rv.end());
	// c'mon gcc, auto-vectorize this loop!
	bool assign_err = false;
	size_t k = 1;
	for ( ; ri!=re; ++ri, ++li, ++k) {
		NEVER_NULL(*li);
		if (!(**li = *ri).good) {
			assign_err = true;
			cerr << "Error: lvalue referenced at position "
				<< k << " is already assigned value "
				<< **li << endl;
		}
	}
	if (assign_err) {
		cerr << "At least one error in assignment." << endl;
		return good_bool(false);
	}
	INVARIANT(li == temp.end());
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function.
	TODO: deprecate assign_value_collection.
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::assign_dests(const const_dest_iterator& b, 
		const const_dest_iterator& e, const const_collection_type& v, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	const_dest_iterator i(b);
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		if (!assign_dest(**i, v, c).good) {
			cerr << "ERROR: at position " <<
				std::distance(b, i)+1 <<
				" of expression-assignment list." << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for unrolling a single dest = src assignment.  
	Called during unroll-assignment of template formal parameters.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::__unroll(const unroll_context& c, 
		const value_reference_type& lv, 
		const src_const_ptr_type& rv) {
	STACKTRACE_VERBOSE;
	INVARIANT(rv);
	// works for scalars and multidimensional arrays alike
	const count_ptr<const const_param>
		src_values(rv->unroll_resolve_rvalues(c, rv));
	if (!src_values) {
		rv->dump(
			cerr << "ERROR: failed to resolve source values of ",
			expr_dump_context::error_mode) <<
			" in " << traits_type::tag_name <<
			" assignment." << endl;
		return good_bool(false);
	}
	const count_ptr<const const_expr_type>
		scalar_const(src_values.template is_a<const const_expr_type>());
	const count_ptr<const const_collection_type>
		bunch_of_consts(src_values
			.template is_a<const const_collection_type>());
	if (scalar_const) {
		static const multikey_index_type blank;
		// temporary 0-D, scalar value
		const_collection_type the_lonesome_value(blank);
		*the_lonesome_value.begin() =
			scalar_const->static_constant_value();
		return assign_dest(lv, the_lonesome_value, c);
	} else {
		NEVER_NULL(bunch_of_consts);
		return assign_dest(lv, *bunch_of_consts, c);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns src value to each dest, after unpacking instances.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(!dests.empty());		// sanity check
	INVARIANT(this->src);
	// works for scalars and multidimensional arrays alike
	const count_ptr<const const_param>
		src_values(this->src->unroll_resolve_rvalues(c, this->src));
	if (!src_values) {
		this->src->dump(
			cerr << "ERROR: failed to resolve source values of ",
			expr_dump_context::error_mode) <<
			" in " << traits_type::tag_name <<
			" assignment." << endl;
		return good_bool(false);
	}
	const count_ptr<const const_expr_type>
		scalar_const(src_values.template is_a<const const_expr_type>());
	const count_ptr<const const_collection_type>
		bunch_of_consts(src_values
			.template is_a<const const_collection_type>());
	if (scalar_const) {
		static const multikey_index_type blank;
		// temporary 0-D, scalar value
		const_collection_type the_lonesome_value(blank);
		*the_lonesome_value.begin() =
			scalar_const->static_constant_value();
		return assign_dests(this->dests.begin(), this->dests.end(),
			the_lonesome_value, c);
	} else {
		NEVER_NULL(bunch_of_consts);
		return assign_dests(this->dests.begin(), this->dests.end(),
			*bunch_of_consts, c);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits dynamic pointers, and registers them with persistent
	object manager.
	\param m the persistent object manager.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void
EXPRESSION_ASSIGNMENT_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->src->collect_transient_info(m);
	for_each(this->dests.begin(), this->dests.end(), 
	unary_compose(
		bind2nd_argval(mem_fun_ref(
			&value_reference_type::collect_transient_info), m), 
		dereference<count_ptr<const value_reference_type> >()
	)
	);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void
EXPRESSION_ASSIGNMENT_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, this->src);
	m.write_pointer_list(f, this->dests);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
void
EXPRESSION_ASSIGNMENT_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, this->src);
	m.read_pointer_list(f, this->dests);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_TCC__

