/**
	\file "art_object_instance_pint.cc"
	Method definitions for parameter instance collection classes.
 	$Id: art_object_instance_pint.cc,v 1.12.4.3.4.2 2005/01/25 20:34:02 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PINT_CC__
#define	__ART_OBJECT_INSTANCE_PINT_CC__

#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1
#define ENABLE_STACKTRACE				1

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr_param_ref.h"	// for pint/pbool_instance_reference
#include "art_built_ins.h"
#include "art_object_type_hash.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "memory/list_vector_pool.h"
#include "what.h"
#include "STL/list.tcc"
#include "multikey_qmap.tcc"		// include "qmap.tcc"
#include "persistent_object_manager.tcc"
#include "compose.h"
#include "binders.h"
#include "ptrs_functional.h"
#include "indent.h"
#include "stacktrace.h"
#include "static_trace.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"


STATIC_TRACE_BEGIN("instance_pint")

//=============================================================================
namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array<0>, "pint_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array<1>, "pint_array<1>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array<2>, "pint_array<2>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array<3>, "pint_array<3>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array<4>, "pint_array<4>")
}

//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// for composition functors
using std::dereference;
using std::mem_fun_ref;
using util::indent;
using util::auto_indent;
USING_STACKTRACE

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// struct pint_instance method definitions
// not really methods...

bool
operator == (const pint_instance& p, const pint_instance& q) {
	INVARIANT(p.instantiated && q.instantiated);
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const pint_instance& p) {
	INVARIANT(p.instantiated);
	if (p.valid) {
		return o << p.value;
	} else	return o << "?";
}

//=============================================================================
// class pint_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instance_collection, 
	PINT_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instance_collection::pint_instance_collection(const size_t d) :
		parent_type(d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a special case used by built-in definition construction, 
	so we restrict the default argument to constant scalar integer.  
	This way, we can safely omit the call to
	type_check_actual_param_expr(*i).
 */
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n, const size_t d, 
		const count_ptr<const pint_const>& i) :
		parent_type(o, n, d), 
		ival(i) {
	/***
		INVARIANT(type_check_actual_param_expr(*i));
		This causes problem... calls pure virtual dimensions()
		during construction phase, resulting in a run-time
		"pure virtual method called" abort trap.  
	***/
#if 0
	INVARIANT(type_check_actual_param_expr(*i));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::~pint_instance_collection() {
	STACKTRACE("~pint_instance_collection()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instance_collection::what(ostream& o) const {
	return o << "pint-inst<" << dimensions << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
pint_instance_collection::get_type_ref(void) const {
	return pint_type_ptr;
		// defined in "art_built_ins.h"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes a parameter instance with an expression, 
	for the sake of COMPILE-TIME analysis only.
	The real assignment will be tracked in a param_expression_assignment
		object at UNROLL-TIME.  

	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	Only bother initializing scalar variables, 
		ignore for initialization of non-scalars.  

	MAKE sure this is not a template_formal, 
	template-formal parameters are NEVER initialized, 
	only given default values.  

	\param e the rvalue expression.
	\return false if there was error.  
	\sa may_be_initialized
	\sa must_be_initialized
 */
bool
pint_instance_collection::initialize(const init_arg_type& e) {
	NEVER_NULL(e);
	INVARIANT(!ival);
	if (dimensions == 0) {
		if (type_check_actual_param_expr(*e)) {
			ival = e;
			return true;
		} else {
			return false;
		}
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigning default value(s) to parameters is only valid
	in the context of template-formal parameters.  
 */
bool
pint_instance_collection::assign_default_value(count_ptr<const param_expr> p) {
	const count_ptr<const pint_expr> i(p.is_a<const pint_expr>());
	if (i && type_check_actual_param_expr(*i)) {
		ival = i;
		return true;
	}
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as a default_value in the context of template
	formal parameters.
	\return pointer to default value expression.
 */
count_ptr<const param_expr>
pint_instance_collection::default_value(void) const {
	STACKTRACE("pint_instance_collection::default_value()");
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as an initial_value outside the context
	of template formals.  
	\return pointer to initial value expression.  
 */
count_ptr<const pint_expr>
pint_instance_collection::initial_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a param reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
	Different: param type reference are always referred to in the global
		scope because they cannot be templated!
		Therefore, cache them in the global (or built-in) namespace.  
	\return NULL.
 */
count_ptr<instance_reference_base>
pint_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<param_instance_reference>(
		new pint_instance_reference(
			never_ptr<pint_instance_collection>(
			const_cast<pint_instance_collection*>(this))));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a pint was passed to a formal 
	pint parameter in a template.  
	Should also check dimensionality and size.  
 */
bool
pint_instance_collection::type_check_actual_param_expr(const param_expr& pe) const {
	const never_ptr<const pint_expr> pi(IS_A(const pint_expr*, &pe));
	if (!pi) {
		// useful error message?
		return false;
	}
	// only for formal parameters is this assertion valid.  
	INVARIANT(index_collection.size() <= 1);
	// check dimensions (is conservative with dynamic sizes)
	return check_expression_dimensions(*pi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No need to virtualize this method as long as 
	the dimension-specific subclasses have no pointers that 
	need to be visited.  
 */
void
pint_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		PINT_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	parent_type::collect_transient_info_base(m);
	// Is ival really crucial in object?  will be unrolled anyhow
	if (ival)
		ival->collect_transient_info(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection*
pint_instance_collection::make_pint_array(
		const scopespace& o, const string& n, const size_t D) {
	switch(D) {
		case 0:	return new pint_array<0>(o, n);
		case 1:	return new pint_array<1>(o, n);
		case 2:	return new pint_array<2>(o, n);
		case 3:	return new pint_array<3>(o, n);
		case 4:	return new pint_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Later: will become dimension-specific.
 */
persistent*
pint_instance_collection::construct_empty(const int i) {
	// later convert to lookup table...
	switch(i) {
		case 0:	return new pint_array<0>();
		case 1:	return new pint_array<1>();
		case 2:	return new pint_array<2>();
		case 3:	return new pint_array<3>();
		case 4:	return new pint_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	parent_type::write_object_base(m, f);
	m.write_pointer(f, ival);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::load_object_base(persistent_object_manager& m, 
		istream& f) {
	parent_type::load_object_base(m, f);
	m.read_pointer(f, ival);
}

//=============================================================================
// class pint_array method_definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
pint_array<D>::pint_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
pint_array<D>::pint_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
pint_array<D>::~pint_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
bool
pint_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
ostream&
pint_array<D>::dump_unrolled_values(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_value_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
ostream&
pint_array<D>::key_value_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << " = " << p.second << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.  
	\param i fully-specified range of indices to instantiate.  
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
void
pint_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
	NEVER_NULL(i);
	// indices is a range_expr_list (base class)
	// resolve into constants now using const_range_list
	// if unable, (b/c uninitialized) then report error
	const_range_list ranges;	// initially empty
	if (!i->resolve_ranges(ranges)) {
		// ranges is passed and returned by reference
		// fail
		cerr << "ERROR: unable to resolve indices "
			"for instantiation: ";
		i->dump(cerr) << endl;
		THROW_EXIT;
	} 
	// else success
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	// later: factor this out into common helper class
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	do {
#if 0
		multikey_base<pint_value_type>::const_iterator
			ci = key_gen.begin();
		for ( ; ci!=key_gen.end(); ci++)
			cerr << '[' << *ci << ']';
		cerr << endl;
#endif
		pint_instance& pi = collection[key_gen];
		if (pi.instantiated) {
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen << 
				"already instantiated!" << endl;
			THROW_EXIT;
		}
		pi.instantiated = true;
		// sanity check: shouldn't start out valid
		INVARIANT(!pi.valid);
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands indices which may be under-specified into explicit
	indices for the implicit subslice, if it is densely packed.  
	Depends on the current state of the collection.  
	\param l is list of indices, which may be under-specified, 
		or even empty.
	\return fully-specified index list, or empty list if there is error.
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
const_index_list
pint_array<D>::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, collection.is_compact());
	}
	// else construct slice
	list<value_type> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr, const const_index>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr, const const_index>()
		)
	);
	return const_index_list(l, 
		collection.is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assumes that index resolves down to a single integer.  
	Returns value of a single integer, if it can be resolved.  
	If integer is uninitialized, report as error.  

	TODO: really this should take a const_index_list argument, 
	to valid dynamic allocation in pint_instance_reference methods.  
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
bool
pint_array<D>::lookup_value(value_type& v, 
		const multikey_base<pint_value_type>& i) const {
	INVARIANT(D == i.dimensions());
	const pint_instance& pi = collection[i];
	if (pi.valid) {
		v = pi.value;
	} else {
		cerr << "ERROR: reference to uninitialized pint " <<
			get_qualified_name() << " at index: " << i << endl;
	}
	return pi.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate values.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or 
		is uninitialized; true on success.
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
bool
pint_array<D>::lookup_value_collection(
		list<value_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, pint_value_type> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const pint_instance& pi = collection[key_gen];
		// INVARIANT(pi.instantiated);	// else earlier check failed
		if (!pi.instantiated)
			cerr << "FATAL: reference to uninstantiated pint index "
				<< key_gen << endl;
		else if (!pi.valid)
			cerr << "ERROR: reference to uninitialized pint index "
				<< key_gen << endl;
		ret &= (pi.valid && pi.instantiated);
		l.push_back(pi.value);
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value, using an index.
	Only call this if this is non-scalar (array).  
	\return true on error.
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
bool
pint_array<D>::assign(const multikey_base<pint_value_type>& k,
		const value_type i) {
	pint_instance& pi = collection[k];
	return !(pi = i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
void
pint_array<D>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_object_base(m, f);
	// write out the instance map
	collection.write(f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PINT_ARRAY_TEMPLATE_SIGNATURE
void
pint_array<D>::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	INVARIANT(f.good());
	STRIP_POINTER_INDEX(f, m);
	load_object_base(m, f);
	// load the instance map
	collection.read(f);
	STRIP_OBJECT_FOOTER(f);
}
}

//-----------------------------------------------------------------------------
// class pint_array<0> specialization method definitions

// reminder: pint_scalar == pint_array<0>
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pint_scalar, 64);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_array<0>::pint_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_array<0>::pint_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_array<0>::pint_array(const scopespace& o, const string& n, 
		const count_ptr<const pint_const>& i) :
		parent_type(o, n, 0, i), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_array<0>::~pint_array() {
	STACKTRACE("~pint_scalar()");
	STACKTRACE_STREAM << "@ " << this << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_array<0>::is_partially_unrolled(void) const {
	return the_instance.instantiated;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_array<0>::dump_unrolled_values(ostream& o) const {
	return o << auto_indent << the_instance;	// << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of parameter integer.  
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.  
	\param i indices must be NULL because this is not an array.
 */
void
pint_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	// 0-D, or scalar
	if (the_instance.instantiated) {
		// should never happen... but just in case
		cerr << "ERROR: Already instantiated!" << endl;
		THROW_EXIT;
	}
	the_instance.instantiated = true;
	INVARIANT(!the_instance.valid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.  
	\param l is list of indices, which may be under-specified, 
		or even empty.
	\return empty index list, always.
 */
const_index_list
pint_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: pint_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	// calling this is probably not intended, and is an error.  
	// DIE;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version assumes collection is a scalar.  
	\return true if lookup found a valid value.  
 */
bool
pint_array<0>::lookup_value(value_type& v) const {
	if (!the_instance.instantiated) {
		cerr << "ERROR: Reference to uninstantiated pint!" << endl;
		return false;
	}
	if (the_instance.valid) {
		v = the_instance.value;
	} else {
		dump(cerr << "ERROR: use of uninitialized ") << endl;
	}
	return the_instance.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_array<0>::lookup_value_collection(
		list<value_type>& l, const const_range_list& r) const {
	cerr << "WARNING: pint_array<0>::lookup_value_collection(...) "
		"should never be called." << endl;
	// DIE;
	INVARIANT(r.empty());
	value_type i;
	const bool ret = lookup_value(i);
	l.push_back(i);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
 */
bool
pint_array<0>::lookup_value(value_type& v, 
		const multikey_base<pint_value_type>& i) const {
	cerr << "FATAL: pint_array<0>::lookup_value(int&, multikey_base) "
		"should never be called!" << endl;
	DIE;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value.
	Only call this if this is scalar, 0-D.
	Decision: should we allow multiple assignments of the same value?
	\return true on error, false on success.  
 */
bool
pint_array<0>::assign(const value_type i) {
	return !(the_instance = i);
		// error message perhaps?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_array<0>::assign(const multikey_base<pint_value_type>& k, 
		const value_type i) {
	// this should never be called
	cerr << "FATAL: pint_array<0>::assign(multikey_base, int) "
		"should never be called!" << endl;
	DIE;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_array<0>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_object_base(m, f);
	// write out the instance
	write_value(f, the_instance);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_array<0>::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	INVARIANT(f.good());
	STRIP_POINTER_INDEX(f, m);
	load_object_base(m, f);
	// load the instance
	read_value(f, the_instance);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// explicit template instantiations (not needed)

#if 0
template class pint_array<0>;
template class pint_array<1>;
template class pint_array<2>;
template class pint_array<3>;
template class pint_array<4>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance_pint")

#endif	// __ART_OBJECT_INSTANCE_PINT_CC__

