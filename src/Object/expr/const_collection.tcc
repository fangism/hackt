/**
	\file "Object/expr/const_collection.tcc"
	Class implementation of collections of expression constants.  
	This file was moved from "Object/expr/const_collection.cc"
 	$Id: const_collection.tcc,v 1.19.12.1.2.1 2006/12/22 04:10:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_CONST_COLLECTION_TCC__
#define	__HAC_OBJECT_EXPR_CONST_COLLECTION_TCC__

// flags for controlling conditional compilation, mostly for debugging

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif

#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#endif

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <exception>
#include <algorithm>
#include <stdexcept>

#include "Object/expr/const_collection.h"
#include "Object/expr/const_param.h"
#include "Object/traits/class_traits.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_visitor.h"

#include "util/persistent_object_manager.h"
#include "util/multikey.h"
#include "util/stacktrace.h"
#include "util/what.tcc"
#include "util/memory/count_ptr.tcc"

//=============================================================================
// partial specializations

/**
	Specialized specialization macro for const_collection template classes.
	Must appear in the util namespace.  
 */
#define	SPECIALIZE_PERSISTENT_TRAITS_CONST_COLLECTION_FULL_DEFINITION(Tag, Key)\
template <>								\
struct persistent_traits<const_collection<Tag> > {			\
	typedef	const_collection<Tag>		type;			\
	static const persistent::hash_key	type_key;		\
	static const int			type_ids[5];		\
	typedef	binder_new_functor<type,persistent,int>			\
						constructor_type;	\
	static const constructor_type		empty_constructors[5];	\
};									\
									\
const persistent::hash_key						\
persistent_traits<const_collection<Tag> >::type_key(Key);		\
									\
const persistent_traits<const_collection<Tag> >::constructor_type	\
persistent_traits<const_collection<Tag> >::empty_constructors[5] = {	\
	persistent_traits<const_collection<Tag> >::constructor_type(0),	\
	persistent_traits<const_collection<Tag> >::constructor_type(1),	\
	persistent_traits<const_collection<Tag> >::constructor_type(2),	\
	persistent_traits<const_collection<Tag> >::constructor_type(3),	\
	persistent_traits<const_collection<Tag> >::constructor_type(4)	\
};									\
									\
const int								\
persistent_traits<const_collection<Tag> >::type_ids[5] = {		\
persistent_object_manager::register_persistent_type<const_collection<Tag> >(\
	0, &empty_constructors[0]),					\
persistent_object_manager::register_persistent_type<const_collection<Tag> >(\
	1, &empty_constructors[1]),					\
persistent_object_manager::register_persistent_type<const_collection<Tag> >(\
	2, &empty_constructors[2]),					\
persistent_object_manager::register_persistent_type<const_collection<Tag> >(\
	3, &empty_constructors[3]),					\
persistent_object_manager::register_persistent_type<const_collection<Tag> >(\
	4, &empty_constructors[4])					\
};


//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
#include "util/using_ostream.h"
using namespace util::memory;
using util::persistent_traits;

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
// the robust list_vector_pool requires this.  
#endif

//=============================================================================
// class const_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
CONST_COLLECTION_CLASS::const_collection(const size_t d) :
		expr_base_type(), parent_const_type(), values(d) {
	INVARIANT(d <= 4);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
CONST_COLLECTION_CLASS::const_collection(const key_type& k) :
		expr_base_type(), parent_const_type(), values(k) {
	INVARIANT(k.size() <= 4);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
CONST_COLLECTION_CLASS::~const_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
ostream&
CONST_COLLECTION_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out values in structured format.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
ostream&
CONST_COLLECTION_CLASS::dump(ostream& o) const {
#if 0
	return values.dump(o);	// too verbose
#else
	return values.dump_values(o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The expr_dump_context parameter is currrently unused.  
	Possible uses might include reformatting arrays?
	For now, just wraps around the default.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
ostream&
CONST_COLLECTION_CLASS::dump(ostream& o, const expr_dump_context&) const {
	return dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
size_t
CONST_COLLECTION_CLASS::dimensions(void) const {
	return values.dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We defer the constant is_true to the scalar const expression
	type's policy.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::is_true(void) const {
	if (!values.dimensions()) {
		const const_expr_type temp(values.front());
		return temp.is_true();
	}	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return heap-allocated copy.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename CONST_COLLECTION_CLASS::parent_const_type>
CONST_COLLECTION_CLASS::static_constant_param(void) const {
	typedef	count_ptr<const parent_const_type>	return_type;
	return return_type(new this_type(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A packed array of constants always has constant dimensions.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::has_static_constant_dimensions(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return dense range list representation of the instance
		collection if it is indeed compact, else an empty list.  
	No dimensions or indices are implicit.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
const_range_list
CONST_COLLECTION_CLASS::static_constant_dimensions(void) const {
	const_range_list ret;
	const key_type first(values.first_key()), last(values.last_key());
	typename key_type::const_iterator
		f_iter(first.begin()), l_iter(last.begin());
	for ( ; f_iter != first.end(); f_iter++, l_iter++) {
		ret.push_back(const_range(*f_iter, *l_iter));
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Given constant values, this should be precise, i.e. may <=> must.
	\return true if expressions may be equivalent (conservative).
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::may_be_equivalent(const param_expr& e) const {
	const never_ptr<const this_type>
		p(IS_A(const this_type*, &e));
	if (p) {
		// precisely
		return (values.dimensions() == p->values.dimensions() &&
			values.size() == p->values.size() &&
			values == p->values);
	} else {
		// conservatively
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given constant values, this should be precise, i.e. may <=> must.
	\return true if the expressions must be equivalent, 
		conservatively false.
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::must_be_equivalent(const param_expr& e) const {
	const never_ptr<const this_type>
		p(IS_A(const this_type*, &e));
	if (p) {
		// precisely
		return (values.dimensions() == p->values.dimensions() &&
			values.size() == p->values.size() &&
			values == p->values);
	} else {
		// conservatively
		return false;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up one value indexed.  
	\pre k must have same dimensions as the collection.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
typename CONST_COLLECTION_CLASS::value_type
CONST_COLLECTION_CLASS::operator [] (const key_type& k) const {
	return values[k];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Identical to the regular must_be_equivalent method.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::must_be_equivalent(const expr_base_type& p) const {
	STACKTRACE_VERBOSE;
	const this_type* const
		pcc = IS_A(const this_type*, &p);
	if (pcc) {
		return (values.dimensions() == pcc->values.dimensions() &&
			values.size() == pcc->values.size() &&
			values == pcc->values);
	} else {
		// BUG FIXED (2005-07-20) forgot the scalar const case...
		const const_expr_type* const
			psc = IS_A(const const_expr_type*, &p);
		if (psc) {
			if (values.dimensions())
				return false;
			else	return psc->static_constant_value() == values.front();
		} else {
			// conservatively
			return false;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
void
CONST_COLLECTION_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See comment for static_constant_value().
	\return good if successful.
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
good_bool
CONST_COLLECTION_CLASS::unroll_resolve_value(
		const unroll_context&, value_type& v) const {
#if 0
	cerr << "Never supposed to call "
		"CONST_COLLECTION_CLASS::unroll_resolve_value()." << endl;
	THROW_EXIT;
	return good_bool(false);
#else
	INVARIANT(!values.dimensions());
	v = values.front();
	return good_bool(true);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return itself, there's nothing to substitute.
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename CONST_COLLECTION_CLASS::expr_base_type>
CONST_COLLECTION_CLASS::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See comment for static_constant_value().
	\return good if successful.
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
good_bool
CONST_COLLECTION_CLASS::resolve_value(value_type& v) const {
#if 0
	cerr << "Never supposed to call CONST_COLLECTION_CLASS::resolve_value()."
		<< endl;
	THROW_EXIT;
	return good_bool(false);
#else
	INVARIANT(!values.dimensions());
	v = values.front();
	return good_bool(true);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	2005-07-08:
	It is possible for this to be called after a valur-reference
	has been resolved down to a scalar constant, 
	in which case it should be equivalent to a pint_const or pbool_const.  
	\pre this is 0-dimensional, or scalar.  
	\return the value of the only member.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
typename CONST_COLLECTION_CLASS::value_type
CONST_COLLECTION_CLASS::static_constant_value(void) const {
#if 0
	cerr << "Never supposed to call CONST_COLLECTION_CLASS::static_constant_value()." << endl;
	THROW_EXIT;
	return -1;
#else
	INVARIANT(!values.dimensions());
	return *values.begin();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
const_index_list
CONST_COLLECTION_CLASS::resolve_dimensions(void) const {
	STACKTRACE_VERBOSE;
	const_index_list ret;
	const key_type first(values.first_key());
	const key_type last(values.last_key());
	typename key_type::const_iterator f_iter(first.begin());
	typename key_type::const_iterator l_iter(last.begin());
	for ( ; f_iter != first.end(); f_iter++, l_iter++) {
		ret.push_back(
			// is reference-counted pointer type
			const_index_list::const_index_ptr_type(
				new const_range(*f_iter, *l_iter)));
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename CONST_COLLECTION_CLASS::const_expr_type>
CONST_COLLECTION_CLASS::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	if (values.dimensions()) {
		cerr << "Error: got non-scalar " <<
			util::what<this_type>::name() <<
			" values where scalar constant expected." << endl;
		return return_type(NULL);
	} else {
		return return_type(new const_expr_type(values.front()));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference-counted pointer to self.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const const_param>
CONST_COLLECTION_CLASS::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return p.template is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p this pointer, redundant, reference counted.  
	\return the reference-counted pointer, a shared shallow copy.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename CONST_COLLECTION_CLASS::expr_base_type>
CONST_COLLECTION_CLASS::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return p.template is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0 && USE_NONMETA_RESOLVE
CONST_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const typename CONST_COLLECTION_CLASS::parent_const_type>
CONST_COLLECTION_CLASS::nonmeta_resolve_copy(const nonmeta_context_base&, 
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return p.template is_a<const this_type>();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs and returns a value slice specfied by the index list
	and returns the same type (possibly of lesser dimensions)
	with copied/extracted values.  
	NOTE: only called from simple_meta_value_reference::unroll_resolve_rvalues
		thus far.  
	\param il the list of indices to visit.  
	\throw std::out_of_range exception if indices are out of range.  
	\return a compact constant collection with the 
		selected subset of values.
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
CONST_COLLECTION_CLASS
CONST_COLLECTION_CLASS::make_value_slice(const const_index_list& il) const {
if (il.empty()) {
	// won't happen, as called from meta_value_reference<>::unroll_resolve_rvalues.
	return *this;
} else {
	STACKTRACE_VERBOSE;
	const const_range_list crl(il.collapsed_dimension_ranges());
#if ENABLE_STACKTRACE
	il.dump(STACKTRACE_INDENT << "il = ") << endl;
	crl.dump(STACKTRACE_INDENT << "crl = ") << endl;
#endif
try {
	this_type ret(crl.resolve_sizes());	// may throw const_range
	generic_index_generator_type key_gen(il.size());
	const key_type l(il.lower_multikey());
	const key_type u(il.upper_multikey());
	STACKTRACE_INDENT_PRINT("l = " << l << endl);
	STACKTRACE_INDENT_PRINT("u = " << u << endl);
	if (!this->values.range_check(l) || !this->values.range_check(u)) {
		// can't return it, drat...
		throw std::out_of_range(
			"packed_array_generic index is out of range.");
	}
	key_gen.get_lower_corner() = l;
	key_gen.get_upper_corner() = u;
	key_gen.initialize();
	typename array_type::iterator coll_iter(ret.values.begin());
	do {
		*coll_iter = this->values[key_gen];
		coll_iter++;
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	INVARIANT(coll_iter == ret.values.end());
	return ret;
} catch (const_range_list::bad_range r) {
	// convert exception to std::range_error
	const_range::diagnose_bad_range(cerr << "got: ", r) << endl;
	cerr << "Error during attempt to extrace slice of values from " <<
		util::what<this_type>::name() << endl;
	throw std::range_error("indices contain bad range.");
}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For two dense value collections to be comparable, they must
	have the same dimensions.  
	The rest is just a value-for-value lexicographical compare.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
bool
CONST_COLLECTION_CLASS::operator < (const const_param& p) const {
	const this_type* pp(IS_A(const this_type*, &p));
	if (pp) {
		INVARIANT(values.size() == pp->values.size());
		// offsets don't matter, just dimensions
		return std::lexicographical_compare(
			this->begin(), this->end(), pp->begin(), pp->end()
		);
	} else {
		// we must have a scalar const_expr_type
		const const_expr_type& pc(IS_A(const const_expr_type&, p));
		INVARIANT(!this->dimensions());
		return this->front() < pc.static_constant_value();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped operator overloading for value concatenation.  
	\pre same as packed_array_generic::operator += 's
	May assert fail if preconditions not met.  
	\param r constant value collection to concatenate.  
	\return this
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
CONST_COLLECTION_CLASS&
CONST_COLLECTION_CLASS::operator += (const this_type& r) {
	this->values += r.values;
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing because there are no pointers to collect and visit.  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
void
CONST_COLLECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	const char s = values.dimensions();
	INVARIANT(s >= 0);
	INVARIANT(s <= 4);
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
void
CONST_COLLECTION_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	values.write(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONST_COLLECTION_TEMPLATE_SIGNATURE
void
CONST_COLLECTION_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	values.read(f);
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_EXPR_CONST_COLLECTION_TCC__

