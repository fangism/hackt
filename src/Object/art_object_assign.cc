/**
	\file "art_object_assign.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_assign.cc,v 1.15 2005/02/27 22:54:08 fang Exp $
 */

#ifndef	__ART_OBJECT_ASSIGN_CC__
#define	__ART_OBJECT_ASSIGN_CC__

#define	DEBUG_LIST_VECTOR_POOL		0

#include <iostream>
#include <exception>
#include <numeric>

#include "art_object_assign.h"
#include "art_object_expr_param_ref.h"
#include "STL/list.tcc"
#include "persistent_object_manager.tcc"
#include "memory/list_vector_pool.tcc"

#include "art_object_type_hash.h"

#include "what.h"
#include "binders.h"
#include "compose.h"
#include "dereference.h"
#include "ptrs_functional.h"

//=============================================================================
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::pint_expression_assignment, 
		"pint-expression-assignment")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_expression_assignment, 
		"pbool-expression-assignment")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_expression_assignment,
		PBOOL_EXPR_ASSIGNMENT_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_expression_assignment,
		PINT_EXPR_ASSIGNMENT_TYPE_KEY)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
USING_UTIL_COMPOSE
using std::mem_fun_ref;
using util::dereference;
using std::bind2nd_argval;
using util::persistent_traits;

//=============================================================================
// class param_expression_assignment method definitions
	
param_expression_assignment::param_expression_assignment() :
		instance_management_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expression_assignment::~param_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return TRUE if there is an error condition in this iteration
		or earlier.
 */
bool
param_expression_assignment::instance_reference_appender::operator () (
		const bool b, const object_list::value_type& o) {
	index++;
	if (!o) {
		cerr << "ERROR: in creating item " << index <<
			" of assign-list." << endl;
		return true;
	}
	const dest_ptr_type i(o.is_a<param_instance_reference>());
	if (!i) {
		cerr << "ERROR: unhandled case for item " << index <<
			" of assign-list." << endl;
		return true;
	}
	return ex_ass.append_param_instance_reference(i) || b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints error to stderr if dimensions don't match.  
	\todo later redirect output to some error stream context.  
	\param s pointer to the destination of assignment.  
	\return true if dimensions of the source matches d.  
 */
bool
param_expression_assignment::validate_dimensions_match(
		const dest_const_ptr_type& s, const size_t d) const {
	const size_t s_dim = s->dimensions();
	if (s_dim != d) {
		cerr << "ERROR: dimensions of expression " << size() +1 <<
			" (" << s_dim << ") doesn't match that of the rhs ("
			<< d << ")." << endl;
		return false;
	} else	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks that the referenced instance is not already initialized.
	Only checks for scalars and not multidimensional collections.  
 */
bool
param_expression_assignment::validate_reference_is_uninitialized(
		const dest_const_ptr_type& s) const {
	if (s->must_be_initialized()) {
		// definitely initialized or formal
		cerr << "ERROR: expression " << size() +1 <<
			"is already initialized!" << endl;
		// don't care if it's same value... still an error
		return false;
	} else	return true;
}

//=============================================================================
// class pbool_expression_assignment method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pbool_expression_assignment, 32)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pbool_expression_assignment::pbool_expression_assignment() :
		param_expression_assignment(), src(), dests() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s the right-most expression in the assignment, 
		called the source value. 
 */
pbool_expression_assignment::pbool_expression_assignment(
		const src_const_ptr_type& s) :
		param_expression_assignment(), src(s), dests() {
	NEVER_NULL(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_expression_assignment::~pbool_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the number of left-expressions, or destinations.  
 */
size_t
pbool_expression_assignment::size(void) const {
	return dests.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
pbool_expression_assignment::what(ostream& o) const {
	return o << "pbool-expr-assignment";
}
#else
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_expression_assignment)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out assignment statement with sequential expressions.  
	Consider using ostream_iterator and copy.  
	\param o the output stream.
	\return the output stream.
 */
ostream&
pbool_expression_assignment::dump(ostream& o) const {
	NEVER_NULL(src);
	INVARIANT(!dests.empty());
	dumper dumpit(o);
	for_each(dests.begin(), dests.end(), dumpit);
	return src->dump(o << " = ") << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o the output stream.  
	\param i the initial index, used to suppress delimiter.  
 */
pbool_expression_assignment::dumper::dumper(ostream& o, const size_t i) :
		index(i), os(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When index is -, delimiter will not be printed.  
 */
void
pbool_expression_assignment::dumper::operator() (
		const dest_list_type::value_type& i) {
	NEVER_NULL(i);
	if (index) os << " = ";
	i->dump(os);
	index++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a destination instance reference to the assignment list.  
	\param e new destination expression.  
	\return true if there is an error, else false.  
 */
bool
pbool_expression_assignment::append_param_instance_reference(
		const parent_type::dest_ptr_type& e) {
	// cache the value of dimensions to avoid recomputation?
	NEVER_NULL(e);
	bool err = false;
	size_t dim = src->dimensions();
	if (!validate_dimensions_match(e, dim))
		err = true;
	if (!validate_reference_is_uninitialized(e))
		err = true;
	dest_ptr_type pb(e.is_a<pbool_instance_reference>());
	if (!pb) {
		cerr << "ERROR: Cannot initialize a bool parameter with a ";
		e->what(cerr) << " expression!" << endl;
		err = true;
	} else if (!pb->initialize(src)) {	// type check
		// if scalar, initialize for static analysis
		err = true;
	}
	if (err) {
		cerr << "Error initializing item " << size()+1 <<
			" of assign-list.  " << endl;
		dests.push_back(dest_ptr_type(NULL));
	} else		dests.push_back(pb);
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns src value to each dest, after unpacking instances.  
 */
void
pbool_expression_assignment::unroll(unroll_context& c) const {
	INVARIANT(!dests.empty());		// sanity check
	// works for scalars and multidimensional arrays alike
	pbool_instance_reference::assigner the_assigner(*src);
	// will exit upon error
	bool assign_err = 
		accumulate(dests.begin(), dests.end(), false, the_assigner);
	if (assign_err) {
		cerr << "ERROR: something went wrong in pbool assignment."
			<< endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits dynamic pointers, and registers them with persistent
	object manager.
	\param m the persistent object manager.  
 */
void
pbool_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	src->collect_transient_info(m);
	for_each(dests.begin(), dests.end(), 
	unary_compose(
		bind2nd_argval(mem_fun_ref(
			&pbool_instance_reference::collect_transient_info), m), 
		dereference<count_ptr<const pbool_instance_reference> >()
	)
	);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_expression_assignment::construct_empty(const int i) {
	return new pbool_expression_assignment();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_expression_assignment::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, src);
	m.write_pointer_list(f, dests);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_expression_assignment::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, src);
	m.read_pointer_list(f, dests);
}

//=============================================================================
// class pint_expression_assignment method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pint_expression_assignment, 64)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pint_expression_assignment::pint_expression_assignment() :
		param_expression_assignment(), src(), dests() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs and initalizes an integer assignment with a
	source expression.  
	\param s the rightmost (source) expression.  
 */
pint_expression_assignment::pint_expression_assignment(
		const src_const_ptr_type& s) :
		param_expression_assignment(), src(s), dests() {
	NEVER_NULL(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_expression_assignment::~pint_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the number of destinations to assign.  
 */
size_t
pint_expression_assignment::size(void) const {
	return dests.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
pint_expression_assignment::what(ostream& o) const {
	return o << "pint-expr-assignment";
}
#else
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_expression_assignment)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_expression_assignment::dump(ostream& o) const {
	NEVER_NULL(src);
	INVARIANT(!dests.empty());
	dumper dumpit(o);
	for_each (dests.begin(), dests.end(), dumpit);
	return src->dump(o << " = ") << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_expression_assignment::dumper::dumper(ostream& o, const size_t i) :
		index(i), os(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When index is -, delimiter will not be printed.  
 */
void
pint_expression_assignment::dumper::operator() (
		const dest_list_type::value_type& i) {
	NEVER_NULL(i);
	if (index) os << " = ";
	i->dump(os);
	index++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a destination instance reference to the assignment list.  
	\param e new destination expression.  
	\return true if there is an error, else false.  
 */
bool
pint_expression_assignment::append_param_instance_reference(
		const parent_type::dest_ptr_type& e) {
	// cache the value of dimensions to avoid recomputation?
	NEVER_NULL(e);
	bool err = false;
	size_t dim = src->dimensions();
	if (!validate_dimensions_match(e, dim))
		err = true;
	if (!validate_reference_is_uninitialized(e))
		err = true;
	dest_ptr_type pi(e.is_a<pint_instance_reference>());
	if (!pi) {
		cerr << "ERROR: Cannot initialize an int parameter with a ";
		e->what(cerr) << " expression!" << endl;
		err = true;
	} else if (!pi->initialize(src)) {	// type check
		// if scalar, initialize for static analysis
		err = true;
	}
	if (err) {
		cerr << "Error initializing item " << size()+1 <<
			" of assign-list.  " << endl;
		dests.push_back(dest_ptr_type(NULL));
	} else		dests.push_back(pi);
	return err;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns src value to each dest, after unpacking instances.  
 */
void
pint_expression_assignment::unroll(unroll_context& c) const {
	INVARIANT(!dests.empty());		// sanity check
	// works for scalars and multidimensional arrays alike
	pint_instance_reference::assigner the_assigner(*src);
	// will exit upon error
	bool assign_err = 
		accumulate(dests.begin(), dests.end(), false, the_assigner);
	if (assign_err) {
		cerr << "ERROR: something went wrong in pint assignment."
			<< endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	src->collect_transient_info(m);
	for_each(dests.begin(), dests.end(), 
	unary_compose(
		bind2nd_argval(mem_fun_ref(
			&pint_instance_reference::collect_transient_info), m), 
		dereference<count_ptr<const pint_instance_reference> >()
	)
	);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_expression_assignment::construct_empty(const int i) {
	return new pint_expression_assignment();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, src);
	m.write_pointer_list(f, dests);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, src);
	m.read_pointer_list(f, dests);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_ASSIGN_CC__

