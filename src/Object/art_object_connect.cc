/**
	\file "art_object_connect.cc"
	Method definitions pertaining to connections and assignments.  
 */

#include <iostream>

#include "art_parser_debug.h"		// need this?
#include "art_parser_base.h"
#include "art_object_connect.h"
#include "art_object_expr.h"
#include "art_object_IO.tcc"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class param_expression_assignment method definitions
	
param_expression_assignment::param_expression_assignment() :
//		object(), 
		instance_management_base()
//		ex_list()
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expression_assignment::~param_expression_assignment() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
param_expression_assignment::what(ostream& o) const {
	return o << "param-expr-assignment";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_expression_assignment::dump(ostream& o) const {
	assert(ex_list.size() > 1);
	ex_list_type::const_iterator iter = ex_list.begin();
	const ex_list_type::const_iterator end = ex_list.end();
	assert(*iter);
	(*iter)->dump(o);
	for (iter++ ; iter!=end; iter++) {
		assert(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
param_expression_assignment::append_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::prepend_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_front(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unpacks the right-most (last) expression and assigns its
	value(s) to all other expressions in the list on the left. 
 */
void
param_expression_assignment::unroll(void) const {
	cerr << "param_expression_assignment::unroll(): "
		"Fang, finish me!" << endl;
	assert(ex_list.size() > 1);		// sanity check
	// Evaluate last expression
	const ex_list_type::value_type& rhs = ex_list.back();
	// need to verify sizes and types?
	// should already be type verified (pint vs. pbool)
}
#endif

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
param_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PARAM_EXPR_ASSIGNMENT_TYPE)) {
#if 0
	for_each(ex_list.begin(), ex_list.end(), 
	bind2nd(mem_fun(&ex_list_type::value_type::collect_transient_info), m)
	);
#else
	ex_list_type::const_iterator iter = ex_list.begin();
	const ex_list_type::const_iterator end = ex_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
param_expression_assignment::construct_empty(void) {
	return new param_expression_assignment();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer_list(f, ex_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
param_expression_assignment::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer_list(f, ex_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNBORN OBSOLETE
void
param_expression_assignment::unroll_params(void) const {
	// first make sure rhs is initialized
	// what if rhs expression refers to not-yet-unroll instantiation?
	// need to follow dependencies
	// need a method to search for writers to 

	const ex_list_type::const_iterator last = --ex_list.end();
	const size_t dim = (*last)->dimensions();
	// Evaluate:
	// is the reference valid?
	// is it initialized?
	// what type, what dimension?
	// may be collective, if so, must be dense.  

#if 0
	switch (dim) {
	case 0:
		param_const foo = (*last)->evaluate();

		// Dispatch dimension-specific unrolling sub-routine?

		ex_list_type::iterator iter = ex_list.begin();
		for ( ; iter!=last; iter++) {
			assign
		}
	}
#endif
}
#endif

//=============================================================================
// class pbool_expression_assignment method definitions

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
	cerr << "pbool_expression_assignment::unroll(): "
		"Fang, finish me!" << endl;
	assert(!dests.empty());		// sanity check
	// Evaluate source expression, src.
	// need to verify sizes and types?
	// should already be type verified (pbool)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_EXPR_ASSIGNMENT_TYPE)) {
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
object*
pbool_expression_assignment::construct_empty(void) {
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
	cerr << "pint_expression_assignment::unroll(): "
		"Fang, finish me!" << endl;
	assert(!dests.empty());		// sanity check
	// Evaluate source expression, src.
	// need to verify sizes and types?
	// should already be type verified (pint)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_expression_assignment::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_EXPR_ASSIGNMENT_TYPE)) {
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
object*
pint_expression_assignment::construct_empty(void) {
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
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		object(), instance_management_base(), inst_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i instance reference to connect, may not be NULL.
 */
void
instance_reference_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_back(i);
}

//=============================================================================
// class aliases_connection method definitions

aliases_connection::aliases_connection() : instance_reference_connection() { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection::what(ostream& o) const {
	return o << "alias-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
aliases_connection::dump(ostream& o) const {
	assert(inst_list.size() > 1);
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	assert(*iter);
	(*iter)->dump(o);
	for (iter++ ; iter!=end; iter++) {
		assert(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::prepend_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_front(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::unroll(void) const {
	cerr << "aliases_connection::unroll(): "
		"Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, ALIAS_CONNECTION_TYPE)) {
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
aliases_connection::construct_empty(void) {
	return new aliases_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer_list(f, inst_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
aliases_connection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer_list(f, inst_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class port_connection method definitions

/**
	Private empty constructor.
 */
port_connection::port_connection() :
		instance_reference_connection(), inst(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(
		count_const_ptr<simple_instance_reference> i) :
		instance_reference_connection(), inst(i) {
	assert(inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::what(ostream& o) const {
	return o << "port-connection";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_connection::dump(ostream& o) const {
	assert(inst);
	inst->dump(o) << " (";

	if (!inst_list.empty()) {
		inst_list_type::const_iterator iter = inst_list.begin();
		const inst_list_type::const_iterator end = inst_list.end();
		if (*iter)
			(*iter)->dump(o);
		else o << " ";
		for (iter++ ; iter!=end; iter++) {
			o << ", ";
			if (*iter)
				(*iter)->dump(o);
		}
	}
	return o << ");";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::unroll(void) const {
	cerr << "port_connection::unroll(): "
		"Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PORT_CONNECTION_TYPE)) {
	assert(inst);
	inst->collect_transient_info(m);
	inst_list_type::const_iterator iter = inst_list.begin();
	const inst_list_type::const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		// port connection arguments may be NULL
		if (*iter)
			(*iter)->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
port_connection::construct_empty(void) {
	return new port_connection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst);
	m.write_pointer_list(f, inst_list);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_connection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst);
	m.read_pointer_list(f, inst_list);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
#if 0
NOT READY TO UNVEIL
// class dynamic_connection_assignment method definitions

dynamic_connection_assignment::dynamic_connection_assignment(
		never_const_ptr<scopespace> s) :
		connection_assignment_base(), dscope(s) {
	// check that dscope is actually a loop or conditional
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
//=============================================================================
}	// end namespace entity
}	// end namespace ART

