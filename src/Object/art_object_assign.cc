/**
	\file "art_object_assign.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_assign.cc,v 1.8 2004/12/07 02:22:06 fang Exp $
 */

#include <iostream>
#include <numeric>

#include "art_object_assign.h"
#include "art_object_expr_param_ref.h"
#include "STL/list.tcc"
#include "persistent_object_manager.tcc"


#include "art_object_type_hash.h"

#include "binders.h"
#include "compose.h"
#include "ptrs_functional.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// from "compose.h"

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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_expression_assignment,
	PBOOL_EXPR_ASSIGNMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pbool_expression_assignment::pbool_expression_assignment() :
		param_expression_assignment(), src(), dests() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_expression_assignment::pbool_expression_assignment(
		const src_const_ptr_type& s) :
		param_expression_assignment(), src(s), dests() {
	assert(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_expression_assignment::~pbool_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
pbool_expression_assignment::size(void) const {
	return dests.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_expression_assignment::what(ostream& o) const {
	return o << "pbool-expr-assignment";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_expression_assignment::dump(ostream& o) const {
	assert(src);
	assert(!dests.empty());
	dumper dumpit(o);
	for_each (dests.begin(), dests.end(), dumpit);
	return src->dump(o << " = ") << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	assert(i);
	if (index) os << " = ";
	i->dump(os);
	index++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a destination instance reference to the assignment list.  
 */
bool
pbool_expression_assignment::append_param_instance_reference(
		const parent_type::dest_ptr_type& e) {
	// cache the value of dimensions to avoid recomputation?
	assert(e);
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
pbool_expression_assignment::unroll(void) const {
#if 0
	cerr << "pbool_expression_assignment::unroll(): "
		"Fang, finish me!" << endl;
#endif
	assert(!dests.empty());		// sanity check
	// works for scalars and multidimensional arrays alike
	pbool_instance_reference::assigner the_assigner(*src);
	// will exit upon error
	bool assign_err = 
		accumulate(dests.begin(), dests.end(), false, the_assigner);
	if (assign_err) {
		cerr << "ERROR: something went wrong in pbool assignment."
			<< endl;
		exit(1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_EXPR_ASSIGNMENT_TYPE_KEY)) {
	src->collect_transient_info(m);
#if 0
	for_each(dests.begin(), dests.end(), 
	bind2nd(mem_fun(&ex_list_type::value_type::collect_transient_info), m)
	);
	// bind2nd_argval?
#else
	dest_list_type::const_iterator iter = dests.begin();
	const dest_list_type::const_iterator end = dests.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#endif
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
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, src);
	m.write_pointer_list(f, dests);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_expression_assignment::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, src);
	m.read_pointer_list(f, dests);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class pint_expression_assignment method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_expression_assignment,
	PINT_EXPR_ASSIGNMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pint_expression_assignment::pint_expression_assignment() :
		param_expression_assignment(), src(), dests() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_expression_assignment::pint_expression_assignment(
		const src_const_ptr_type& s) :
		param_expression_assignment(), src(s), dests() {
	assert(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_expression_assignment::~pint_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
pint_expression_assignment::size(void) const {
	return dests.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_expression_assignment::what(ostream& o) const {
	return o << "pint-expr-assignment";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_expression_assignment::dump(ostream& o) const {
	assert(src);
	assert(!dests.empty());
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
	assert(i);
	if (index) os << " = ";
	i->dump(os);
	index++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a destination instance reference to the assignment list.  
 */
bool
pint_expression_assignment::append_param_instance_reference(
		const parent_type::dest_ptr_type& e) {
	// cache the value of dimensions to avoid recomputation?
	assert(e);
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
pint_expression_assignment::unroll(void) const {
	assert(!dests.empty());		// sanity check
	// works for scalars and multidimensional arrays alike
	pint_instance_reference::assigner the_assigner(*src);
	// will exit upon error
	bool assign_err = 
		accumulate(dests.begin(), dests.end(), false, the_assigner);
	if (assign_err) {
		cerr << "ERROR: something went wrong in pint assignment."
			<< endl;
		exit(1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_EXPR_ASSIGNMENT_TYPE_KEY)) {
	src->collect_transient_info(m);
#if 1
	for_each(dests.begin(), dests.end(), 
	unary_compose(
		bind2nd_argval(mem_fun_ref(
			&pint_instance_reference::collect_transient_info), m), 
		dereference<count_ptr, const pint_instance_reference>()
	)
	);
#else
	dest_list_type::const_iterator iter = dests.begin();
	const dest_list_type::const_iterator end = dests.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#endif
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
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, src);
	m.write_pointer_list(f, dests);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, src);
	m.read_pointer_list(f, dests);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

