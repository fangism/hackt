/**
	\file "Object/inst/value_collection.tcc"
	Method definitions for parameter instance collection classes.
	This file was "Object/art_object_value_collection.tcc"
		in a previous life.  
 	$Id: value_collection.tcc,v 1.10.2.1 2006/02/12 06:15:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_COLLECTION_TCC__
#define	__HAC_OBJECT_INST_VALUE_COLLECTION_TCC__

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif

#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0 && DEBUG_LIST_VECTOR_POOL
#endif

#ifndef ENABLE_STACKTRACE
#define ENABLE_STACKTRACE				0
#endif

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/common/extern_templates.h"
// #define EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#include "Object/inst/value_collection.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/common/dump_flags.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_param_meta_value_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/def/definition_base.h"
#include "Object/common/namespace.h"
// #include "Object/unroll/unroll_context.h"
#include "Object/unroll/unroll_context_value_resolver.h"

#include "util/memory/list_vector_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"
#include "util/multikey_qmap.tcc"		// include "qmap.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/compose.h"
#include "util/binders.h"
#include "util/ptrs_functional.h"
#include "util/dereference.h"
#include "util/indent.h"
#include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::indent;
using util::auto_indent;
using util::write_value;
using util::read_value;
using util::persistent_traits;
using util::memory::never_ptr;
using util::multikey_generator;

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// struct pint_instance method definitions
// not really methods...

#if 0
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
#endif

//=============================================================================
// class value_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
VALUE_COLLECTION_CLASS::value_collection(const size_t d) :
		parent_type(d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
VALUE_COLLECTION_CLASS::value_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial copy constructor.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
VALUE_COLLECTION_CLASS::value_collection(const this_type& t, 
		const footprint& f) :
		parent_type(t, f), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
VALUE_COLLECTION_CLASS::~value_collection() {
//	STACKTRACE_DTOR("~value_collection()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
VALUE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
VALUE_COLLECTION_CLASS::what(ostream& o) const {
#if 0
	return o << "pint-inst<" << dimensions << ">";
#else
	return o << util::what<this_type>::name();
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
VALUE_COLLECTION_CLASS::type_dump(ostream& o) const {
	return o << class_traits<Tag>::tag_name << '^' << this->dimensions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const param_type_reference>
VALUE_COLLECTION_CLASS::get_param_type_ref(void) const {
	return class_traits<Tag>::built_in_type_ptr;
		// declared in "traits/class_traits.h"
		// initialized in "art_built_ins.cc"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
VALUE_COLLECTION_CLASS::get_type_ref(void) const {
	return get_param_type_ref();
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
VALUE_COLLECTION_TEMPLATE_SIGNATURE
good_bool
VALUE_COLLECTION_CLASS::initialize(const init_arg_type& e) {
	NEVER_NULL(e);
	INVARIANT(!ival);
	if (this->dimensions == 0) {
		if (may_type_check_actual_param_expr(*e).good) {
			ival = e;
			return good_bool(true);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigning default value(s) to parameters is only valid
	in the context of template-formal parameters.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
good_bool
VALUE_COLLECTION_CLASS::assign_default_value(
		const count_ptr<const param_expr>& p) {
	const count_ptr<const expr_type> i(p.template is_a<const expr_type>());
	if (i && may_type_check_actual_param_expr(*i).good) {
		ival = i;
		return good_bool(true);
	}
	else return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as a default_value in the context of template
	formal parameters.
	\return pointer to default value expression.
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const param_expr>
VALUE_COLLECTION_CLASS::default_value(void) const {
//	STACKTRACE("VALUE_COLLECTION_CLASS::default_value()");
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as an initial_value outside the context
	of template formals.  
	\return pointer to initial value expression.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename VALUE_COLLECTION_CLASS::expr_type>
VALUE_COLLECTION_CLASS::initial_value(void) const {
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
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
VALUE_COLLECTION_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<simple_param_meta_value_reference>(
		new simple_meta_value_reference_type(
			never_ptr<this_type>(const_cast<this_type*>(this))));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
VALUE_COLLECTION_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<nonmeta_instance_reference_base>(
		new simple_nonmeta_instance_reference_type(
			never_ptr<this_type>(const_cast<this_type*>(this))));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a param was passed to a formal 
	parameter in a template.  
	Should also check dimensionality and size.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
good_bool
VALUE_COLLECTION_CLASS::may_type_check_actual_param_expr(
		const param_expr& pe) const {
	const never_ptr<const expr_type> pi(IS_A(const expr_type*, &pe));
	if (!pi) {
		// useful error message?
		return good_bool(false);
	}
	// this says that the only instantiation statement for this parameter
	// in the original declaration, which in this case was in the ports.  
	// only for formal parameters is this assertion valid.  
	INVARIANT(this->index_collection.size() <= 1);
	// check dimensions (is conservative with dynamic sizes)
	return this->may_check_expression_dimensions(*pi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a param was passed to a formal 
	parameter in a template.  
	Should also check dimensionality and size.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
good_bool
VALUE_COLLECTION_CLASS::must_type_check_actual_param_expr(
		const const_param& pe, const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// only for formal parameters is this assertion valid.  
	// this says that the only instantiation statement for this parameter
	// in the original declaration, which in this case was in the ports.  
	INVARIANT(this->index_collection.size() <= 1);
	// check dimensions (is conservative with dynamic sizes)
	return this->must_check_expression_dimensions(pe, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No need to virtualize this method as long as 
	the dimension-specific subclasses have no pointers that 
	need to be visited.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
void
VALUE_COLLECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key, this->dimensions)) {
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
VALUE_COLLECTION_TEMPLATE_SIGNATURE
VALUE_COLLECTION_CLASS*
VALUE_COLLECTION_CLASS::make_array(
		const scopespace& o, const string& n, const size_t D) {
	switch(D) {
		case 0:	return new value_array<Tag,0>(o, n);
		case 1:	return new value_array<Tag,1>(o, n);
		case 2:	return new value_array<Tag,2>(o, n);
		case 3:	return new value_array<Tag,3>(o, n);
		case 4:	return new value_array<Tag,4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
void
VALUE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE("value_collection<>::write_object_base()");
	parent_type::write_object_base(m, f);
	m.write_pointer(f, ival);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_COLLECTION_TEMPLATE_SIGNATURE
void
VALUE_COLLECTION_CLASS::load_object_base(const persistent_object_manager& m, 
		istream& f) {
	STACKTRACE("value_collection<>::load_object_base()");
	parent_type::load_object_base(m, f);
	m.read_pointer(f, ival);
}

//=============================================================================
// class value_array method_definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
VALUE_ARRAY_CLASS::value_array() :
		parent_type(D), collection(), cached_values(D) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
VALUE_ARRAY_CLASS::value_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection(), cached_values(D) {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial copy constructor for the footprint use.  
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
VALUE_ARRAY_CLASS::value_array(const this_type& t, const footprint& f) :
		parent_type(t, f), collection(t.collection), 
		cached_values(D) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
VALUE_ARRAY_CLASS::~value_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep copy for footprint map.  
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
instance_collection_base*
VALUE_ARRAY_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
ostream&
VALUE_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
bool
VALUE_ARRAY_CLASS::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Arrays cannot be loop variables.  
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
bool
VALUE_ARRAY_CLASS::is_loop_variable(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
ostream&
VALUE_ARRAY_CLASS::dump_unrolled_values(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_value_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
ostream&
VALUE_ARRAY_CLASS::key_value_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << " = " << p.second << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.  
	\param ranges fully-specified range of indices to instantiate.  
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
good_bool
VALUE_ARRAY_CLASS::instantiate_indices(const const_range_list& ranges) {
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	good_bool ret(true);
	do {
#if 0
		multikey_index_type::const_iterator
			ci = key_gen.begin();
		for ( ; ci!=key_gen.end(); ci++)
			cerr << '[' << *ci << ']';
		cerr << endl;
#endif
		element_type& pi = collection[key_gen];
		if (pi.instantiated) {
			cerr << "ERROR: Index " << key_gen << " of " <<
				this->get_qualified_name() <<
				" already instantiated!" << endl;
			ret.good = false;
			// THROW_EXIT;
		}
		pi.instantiated = true;
		// sanity check: shouldn't start out valid
		INVARIANT(!pi.valid);
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
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
VALUE_ARRAY_TEMPLATE_SIGNATURE
const_index_list
VALUE_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
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
	list<pint_value_type> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr<const const_index> >()
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
	to valid dynamic allocation in meta_instance_reference methods.  
	TODO: re-write lookup mechanism completely
	TODO: what if owner is top-level? cannot be formal, 
		but could be loop variable.  Is this OK?
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
good_bool
VALUE_ARRAY_CLASS::lookup_value(value_type& v, 
		const multikey_index_type& i, 
		const unroll_context& c) const {
	INVARIANT(D == i.dimensions());
	if (this->owner.template is_a<const definition_base>()) {
		INVARIANT(!c.empty());
#if 1
		if (!this->is_template_formal()) {
			// must be definition local
			// can't be loop variable b/c would be scalar
			const pair<bool, const parent_type*>
				_r(unroll_context_value_resolver<Tag>()
					.operator()(c, *this, v));
			INVARIANT(!_r.first);
			// I hate this ugly code...
			// the rest is COPIED from the end of this method
			const this_type&
				_val_array(IS_A(const this_type&, *_r.second));
			const key_type index(i);
			const element_type& pi(_val_array.collection[index]);
			if (pi.valid) {
				v = pi.value;
			} else {
				cerr << "ERROR: reference to uninitialized " <<
					class_traits<Tag>::tag_name << ' ' <<
					this->get_qualified_name() << " at index: " <<
					i << endl;
			}
			return good_bool(pi.valid);
		}
		// else is template formal, lookup actual
#endif
		const count_ptr<const const_param>
			ac(c.lookup_actual(*this));
		NEVER_NULL(ac);
		const count_ptr<const const_collection_type>
			sc(ac.template is_a<const const_collection_type>());
		NEVER_NULL(sc);
		v = (*sc)[i];
		return good_bool(true);
	}
	// else is top-level
	const key_type index(i);
	const element_type& pi(collection[index]);
	if (pi.valid) {
		v = pi.value;
	} else {
		cerr << "ERROR: reference to uninitialized " <<
			class_traits<Tag>::tag_name << ' ' <<
			this->get_qualified_name() << " at index: " <<
			i << endl;
	}
	return good_bool(pi.valid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value, using an index.
	Only call this if this is non-scalar (array).  
	\return true on error.
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
bad_bool
VALUE_ARRAY_CLASS::assign(const multikey_index_type& k, const value_type i) {
	// convert from generic to dimension-specific
	// for efficiency, consider an unsafe pointer version, to save copying
	const key_type index(k);
	element_type& pi = collection[index];
	return (pi = i);	// convert good_bool to bad_bool implicitly
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
void
VALUE_ARRAY_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
	// write out the instance map
	this->collection.write(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_ARRAY_TEMPLATE_SIGNATURE
void
VALUE_ARRAY_CLASS::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	// load the instance map
	this->collection.read(f);
}

//-----------------------------------------------------------------------------
// class value_array<Tag,0> specialization method definitions

#if 1 && 0
// reminder: pint_scalar == pint_array<0>
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pint_scalar, 64);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
VALUE_SCALAR_CLASS::value_array() :
		parent_type(0), the_instance(),
		cached_value(const_expr_type::default_value), 
		cache_validity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
VALUE_SCALAR_CLASS::value_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance(),
		cached_value(const_expr_type::default_value), 
		cache_validity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
VALUE_SCALAR_TEMPLATE_SIGNATURE
VALUE_SCALAR_CLASS::value_array(const scopespace& o, const string& n, 
		const count_ptr<const pint_const>& i) :
		parent_type(o, n, 0, i), the_instance() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial copy-constructor for footprint use.  
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
VALUE_SCALAR_CLASS::value_array(const this_type& t, const footprint& f) :
		parent_type(t, f),
		the_instance(t.the_instance), 
		cached_value(const_expr_type::default_value), 
		cache_validity(false) {
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
VALUE_SCALAR_CLASS::~value_array() {
	STACKTRACE_DTOR("~value_scalar()");
//	STACKTRACE_STREAM << "@ " << this << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deep copy-constructor for making a footprint copy of this collection.  
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
instance_collection_base*
VALUE_SCALAR_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
ostream&
VALUE_SCALAR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
bool
VALUE_SCALAR_CLASS::is_partially_unrolled(void) const {
	return the_instance.instantiated;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a loop variable if the parent scopespace
	doesn't not contain it!
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
bool
VALUE_SCALAR_CLASS::is_loop_variable(void) const {
	INVARIANT(this->owner);
	return !this->owner->lookup_member(this->key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
ostream&
VALUE_SCALAR_CLASS::dump_unrolled_values(ostream& o) const {
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
VALUE_SCALAR_TEMPLATE_SIGNATURE
good_bool
VALUE_SCALAR_CLASS::instantiate_indices(const const_range_list& r) {
	INVARIANT (r.empty());
	// 0-D, or scalar
	if (the_instance.instantiated) {
		// should never happen... but just in case
		cerr << "ERROR: Already instantiated!" << endl;
		return good_bool(false);
		// THROW_EXIT;
	}
	the_instance.instantiated = true;
	INVARIANT(!the_instance.valid);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.  
	\param l is list of indices, which may be under-specified, 
		or even empty.
	\return empty index list, always.
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
const_index_list
VALUE_SCALAR_CLASS::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: VALUE_SCALAR_CLASS::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	// calling this is probably not intended, and is an error.  
	// DIE;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version assumes collection is a scalar.  
	\param v the value reference at which to store back the resolved value.
	\param c the unroll context.  
	\return true if lookup found a valid value.  
	TODO: propagage actual context changes to value_array.
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
good_bool
VALUE_SCALAR_CLASS::lookup_value(value_type& v, 
		const unroll_context& c) const {
	if (this->owner.template is_a<const definition_base>()) {
		INVARIANT(!c.empty());
#if 1
		if (!this->is_template_formal() && !this->is_loop_variable()) {
			// must be definition local
			// can't be loop variable b/c would be scalar
			const pair<bool, const parent_type*>
				_r(unroll_context_value_resolver<Tag>()
					.operator()(c, *this, v));
			INVARIANT(!_r.first);
			// I hate this ugly code...
			// the rest is COPIED from the end of this method
			const this_type&
				_val(IS_A(const this_type&, *_r.second));
			if (!_val.the_instance.instantiated) {
				cerr << "ERROR: Reference to uninstantiated " <<
					class_traits<Tag>::tag_name << ' ' <<
					this->get_qualified_name() << "!" << endl;
				return good_bool(false);
			}
			if (_val.the_instance.valid) {
				v = _val.the_instance.value;
			} else {
				this->dump(cerr <<
					"ERROR: use of uninitialized ", 
					dump_flags::default_value) << endl;
			}
			return good_bool(_val.the_instance.valid);
		}
		// else is template formal, lookup actual
#endif
		const count_ptr<const const_param>
			ac(c.lookup_actual(*this));
		NEVER_NULL(ac);
		const count_ptr<const expr_type>
			sc(ac.template is_a<const expr_type>());
		NEVER_NULL(sc);
		v = sc->static_constant_value();
		return good_bool(true);
	} else {
		// NOT TRUE. see test case template/009,040,041.in
		// INVARIANT(c.empty());
		// no need for context in top-level!
	}
	// else is top-level
	if (!the_instance.instantiated) {
		cerr << "ERROR: Reference to uninstantiated " <<
			class_traits<Tag>::tag_name << ' ' <<
			this->get_qualified_name() << "!" << endl;
		return good_bool(false);
	}
	if (the_instance.valid) {
		v = the_instance.value;
	} else {
		this->dump(cerr << "ERROR: use of uninitialized ", 
			dump_flags::default_value) << endl;
	}
	return good_bool(the_instance.valid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
good_bool
VALUE_SCALAR_CLASS::lookup_value(value_type& v, 
		const multikey_index_type& i, 
		const unroll_context&) const {
	cerr << "FATAL: VALUE_SCALAR_CLASS::lookup_value(int&, multikey) "
		"should never be called!" << endl;
	DIE;
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value.
	Only call this if this is scalar, 0-D.
	Decision: should we allow multiple assignments of the same value?
	\return true on error, false on success.  
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
VALUE_SCALAR_CLASS::assign(const value_type i) {
	// convert good_bool to bad_bool implicitly
	return (the_instance = i);
	// error message perhaps?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
VALUE_SCALAR_CLASS::assign(const multikey_index_type& k, const value_type i) {
	// this should never be called
	cerr << "FATAL: VALUE_SCALAR_CLASS::assign(multikey, int) "
		"should never be called!" << endl;
	DIE;
	return bad_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
void
VALUE_SCALAR_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE("value_scalar::write_object()");
	parent_type::write_object_base(m, f);
	// write out the instance
	write_value(f, the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_SCALAR_TEMPLATE_SIGNATURE
void
VALUE_SCALAR_CLASS::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE("value_scalar::load_object()");
	parent_type::load_object_base(m, f);
	// load the instance
	read_value(f, the_instance);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_TCC__

