/**
	\file "Object/art_object_assign.cc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_assign.tcc,v 1.5.4.1 2005/06/24 22:51:11 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_ASSIGN_TCC__
#define	__OBJECT_ART_OBJECT_ASSIGN_TCC__

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL		0
#endif

#include <iostream>
#include <numeric>

#include "Object/art_object_assign.h"

#include "util/STL/list.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "Object/art_object_inst_ref_subtypes.h"

#include "util/what.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"
#include "util/ptrs_functional.h"

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE
using std::mem_fun_ref;
using util::dereference;
using std::bind2nd_argval;
using util::persistent_traits;

//=============================================================================
// class expression_assignment method definitions

// LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(expression_assignment, ..)

EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
typename EXPRESSION_ASSIGNMENT_CLASS::pool_type
EXPRESSION_ASSIGNMENT_CLASS::pool(class_traits<Tag>::assignment_chunk_size);

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
EXPRESSION_ASSIGNMENT_CLASS::operator new (size_t s, void*& p) {
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
EXPRESSION_ASSIGNMENT_CLASS::dump(ostream& o) const {
	NEVER_NULL(this->src);
	INVARIANT(!dests.empty());
	dumper dumpit(o);
	for_each(this->dests.begin(), this->dests.end(), dumpit);
	return this->src->dump(o << " = ") << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o the output stream.  
	\param i the initial index, used to suppress delimiter.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
EXPRESSION_ASSIGNMENT_CLASS::dumper::dumper(ostream& o, const size_t i) :
		index(i), os(o) {
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
	i->dump(os);
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
	if (!validate_dimensions_match(e, dim).good)
		err.bad = true;
	if (!validate_reference_is_uninitialized(e).good)
		err.bad = true;
	dest_ptr_type pb(e.template is_a<value_reference_type>());
	if (!pb) {
		cerr << "ERROR: Cannot initialize a " <<
			class_traits<Tag>::tag_name << " with a ";
		e->what(cerr) << " expression!" << endl;
		err.bad = true;
	} else if (!pb->initialize(src).good) {	// type check
		// if scalar, initialize for static analysis
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
	Assigns src value to each dest, after unpacking instances.  
 */
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::unroll(unroll_context& c) const {
	INVARIANT(!dests.empty());		// sanity check
	// works for scalars and multidimensional arrays alike
	typename value_reference_type::assigner the_assigner(*this->src);
	// will exit upon error
	bad_bool assign_err = 
		accumulate(this->dests.begin(), this->dests.end(), 
			bad_bool(false), the_assigner);
	if (assign_err.bad) {
		cerr << "ERROR: something went wrong in " <<
			class_traits<Tag>::tag_name << " assignment." << endl;
		THROW_EXIT;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
good_bool
EXPRESSION_ASSIGNMENT_CLASS::unroll_meta_evaluate(unroll_context& c) const {
	// would've exited upon error
	return this->unroll(c);
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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_ASSIGN_TCC__

