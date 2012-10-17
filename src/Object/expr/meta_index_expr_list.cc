/**
	\file "Object/expr/meta_index_expr_list.cc"
	Definition of meta index expression lists.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_index_expr_list.cc,v 1.22 2011/04/02 01:45:58 fang Exp $
 */

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/expr/dynamic_meta_index_list.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/common/multikey_index.hh"
#include "Object/expr/meta_index_expr.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/persistent_type_hash.hh"

#include "util/stacktrace.hh"
#include "util/reserve.hh"
#include "util/multikey.hh"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/compose.hh"
#include "util/conditional.hh"		// for compare_if
#include "util/dereference.hh"

//=============================================================================
namespace util {

SPECIALIZE_UTIL_WHAT(HAC::entity::const_index_list, 
		"const-index-list")
SPECIALIZE_UTIL_WHAT(HAC::entity::dynamic_meta_index_list, 
		"dynamic-index-list")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::const_index_list, CONST_INDEX_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::dynamic_meta_index_list, DYNAMIC_INDEX_LIST_TYPE_KEY, 0)

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
using util::persistent_traits;
using util::persistent_object_manager;
using std::distance;
using std::istream;
#include "util/using_ostream.hh"
using std::mem_fun_ref;
using util::dereference;
USING_UTIL_COMPOSE
using util::write_value;
using util::read_value;
using util::reserve;

//=============================================================================
// class meta_index_list method definitions

/**
	Static function call to optimize the case of simple
	pointer copy-ing.  
 */
count_ptr<const const_index_list>
meta_index_list::unroll_resolve_indices(const count_ptr<const this_type>& _this, 
		const unroll_context& c) {
	typedef	count_ptr<const const_index_list>	return_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(_this);
	const return_type ret(_this.is_a<const const_index_list>());
	if (ret)
		return ret;
	else	return return_type(new const_index_list(
			IS_A(const dynamic_meta_index_list&, *_this)
			.unroll_resolve_indices(c)));
}

//=============================================================================
// class const_index_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list::const_index_list() : meta_index_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Depending on implementation type, this will reserve space in 
	advance in a vector-like fashion.  
	\param s the expecte size of the sequence.  
 */
const_index_list::const_index_list(const size_t s) :
		meta_index_list(), parent_type() {
	util::reserve(AS_A(parent_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special constructor: copies index list from first argument, 
	and the second argument is a pair of lists that explicitly
	fills in the remaining implicit indices, as resolved from
	multikey_map::is_compact_slice.
	\param l the base index list, may be under-specified.
	\param f the fill lists of explicit indices (upper and lower), 
		or empty if the set's subslice was not densely populated.
 */
const_index_list::const_index_list(const const_index_list& l, 
		const pair<list<pint_value_type>, list<pint_value_type> >& f) :
		meta_index_list(), parent_type(l) {
	if (f.first.empty()) {
		INVARIANT(f.second.empty());
		clear();
	} else {
		INVARIANT(!f.second.empty());
		const size_t f_size = f.first.size();
		const size_t s_size __ATTRIBUTE_UNUSED__ = f.second.size();
		INVARIANT(f_size == s_size);
		const size_t skip = size();
		INVARIANT(skip <= f_size);
		size_t i = 0;
		const_iterator this_iter(begin());
		list<pint_value_type>::const_iterator f_iter(f.first.begin());
		list<pint_value_type>::const_iterator s_iter(f.second.begin());
		for ( ; i<skip; i++, this_iter++, f_iter++, s_iter++) {
			// sanity check against arguments
			NEVER_NULL(*this_iter);
			INVARIANT((*this_iter)->lower_bound() == *f_iter);
			INVARIANT((*this_iter)->upper_bound() == *s_iter);
		}
		for ( ; i<f_size; i++, f_iter++, s_iter++) {
			INVARIANT(*f_iter <= *s_iter);
			push_back(count_ptr<const_range>(
				new const_range(*f_iter, *s_iter)));
		}
		INVARIANT(size() == f_size);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list::~const_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_index_list::dump(ostream& o) const {
	return dump(o, expr_dump_context::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_index_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		const count_ptr<const pint_expr>
			b(i->is_a<const pint_expr>());
		// consider passing 'brief' context to b->dump
		// see old definition
		if (b)
			b->dump(o << '[', c) << ']';
		else	(*i)->dump(o, c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_index_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A dimension is collapsed if the index is a single integer, 
	and otherwise not collapsed if it's a range.  
	A non-collapsed dimension of size 1 is indicated
	as a range [i..i].  
	\return the number of dimensions are collapsed.  
 */
size_t
const_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		if (i->is_a<const pint_const>())
			ret++;
		else INVARIANT(i->is_a<const const_range>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return list of ranges without the collapsed dimensions.  
		No error is possible, since indices are resolved as constants.  

	Useful for finding actual dimensions and sizes of the reference.  
 */
const_range_list
const_index_list::collapsed_dimension_ranges(void) const {
	const_range_list ret;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		const count_ptr<const const_range>
			cr(i->is_a<const const_range>());
		if (cr)
			ret.push_back(*cr);	// will copy
		else INVARIANT(i->is_a<const pint_const>());
		// continue
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Error diagnostic.  
	\return true if any of the indices are negative.  
 */
bool
const_index_list::negative(void) const {
	// TODO: find_if algorithm?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		if ((*i)->negative()) {
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper to list parent's push_back that checks that
	expression is a 0-dimensional pint_inst reference.  
 */
void
const_index_list::push_back(const const_index_ptr_type& i) {
	// check dimensionality
	NEVER_NULL(i);
	INVARIANT(i->dimensions() == 0);
	parent_type::push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::is_static_constant(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_index_list::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
const_index_list::unroll_resolve_indices(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Each index should be a scalar integer.  
	Error otherwise.
	\param k placeholder for returning allocated and assigned key.  
	\return true if resolved successfully.
 */
bool
const_index_list::resolve_multikey(
		excl_ptr<multikey_index_type>& k) const {
	k = excl_ptr<multikey_index_type>(
		multikey_index_type::make_multikey(size()));
	NEVER_NULL(k);
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j=0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const const_index> ip(*i);
		const count_ptr<const pint_const> pc(ip.is_a<pint_const>());
		if (pc)
			(*k)[j] = pc->static_constant_value();
		else 	return false;
	}
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
excl_ptr<multikey_index_type>
const_index_list::lower_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_index_list::upper_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}
#else
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_index_list::lower_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_index_list::upper_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			dereference<count_ptr<const const_index> >()
		)
	);
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given two resolved lists of constant indices, determine
	whether they are dimensionally equal.  
	Bear in mind that collapsed dimensions are to be ignored
		in the two argument lists. 
 */
bool
const_index_list::equal_dimensions(const const_index_list& l) const {
	// compare_if defined in "util/conditional.h"
	return compare_if(begin(), end(), l.begin(), l.end(), 
		mem_fun_ref(&const_index_ptr_type::is_a<const const_range>), 
		mem_fun_ref(&const_index_ptr_type::is_a<const const_range>), 
		binary_compose(
			mem_fun_ref(&const_index::range_size_equivalent), 
			dereference<count_ptr<const const_index> >(), 
			dereference<count_ptr<const const_index> >()
		)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_index_list::must_be_equivalent_indices(const meta_index_list& l) const {
	const const_index_list* const cl = IS_A(const const_index_list*, &l);
	if (cl) {
		// here, we DO NOT skip collapsed dimensions for comparison
		return std::equal(begin(), end(), cl->begin(), 
		binary_compose(
			mem_fun_ref(&const_index::must_be_equivalent_index), 
			dereference<count_ptr<const const_index> >(), 
			dereference<count_ptr<const const_index> >()
		)
		);
	} else {
		const dynamic_meta_index_list* const
			dl = IS_A(const dynamic_meta_index_list*, &l);
		return dl->must_be_equivalent_indices(*this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return shallow copy of self, no substitutions.  
 */
count_ptr<const meta_index_list>
const_index_list::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const meta_index_list>& l) const {
	INVARIANT(l == this);
	return l;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
const_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(*this);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_index_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer_list(f, AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_index_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer_list(f, AS_A(parent_type&, *this));
	m.load_once_pointer_list(*this);
}

//=============================================================================
// class dynamic_meta_index_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_index_list::dynamic_meta_index_list() :
		meta_index_list(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_index_list::dynamic_meta_index_list(const size_t s) :
		meta_index_list(), parent_type() {
	util::reserve(AS_A(parent_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_index_list::~dynamic_meta_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_meta_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_meta_index_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const pint_expr>
			b(i->is_a<const pint_expr>());
		// was b->dump_brief
		if (b)
			b->dump(o << '[', c) << ']';
		else	(*i)->dump(o, c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dynamic_meta_index_list::push_back(const count_ptr<const meta_index_expr>& i) {
	NEVER_NULL(i);
	if (i->dimensions() != 0) {
		cerr << "i->dimensions = " << i->dimensions() << endl;
		INVARIANT(i->dimensions() == 0);
	}
	parent_type::push_back(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_meta_index_list::size(void) const {
	return parent_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counts which dimensions are collapsed.  
	See description in const_index_list::dimensions_collapsed().  
 */
size_t
dynamic_meta_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		if (i->is_a<const pint_expr>())
			ret++;
		else INVARIANT(i->is_a<const meta_range_expr>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_index_list::is_static_constant(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if (!(*i)->is_static_constant())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_index_list::is_relaxed_formal_dependent(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		if ((*i)->is_relaxed_formal_dependent()) {
			cerr << "ERROR: index at position " <<
				distance(begin(), i)+1 << " is dependent "
				"on a relaxed formal parameter." << endl;
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_index_list::must_be_equivalent_indices(
		const meta_index_list& l) const {
	const const_index_list* const cl = IS_A(const const_index_list*, &l);
	if (cl) {
		INVARIANT(size() == cl->size());
		// heterogenous comparison between const_index, meta_index_expr
		return std::equal(begin(), end(), cl->begin(), 
		binary_compose(
			mem_fun_ref(&meta_index_expr::must_be_equivalent_index), 
			dereference<count_ptr<const meta_index_expr> >(), 
			dereference<count_ptr<const const_index> >()
		)
		);
	} else {
		const dynamic_meta_index_list* const
			dl = IS_A(const dynamic_meta_index_list*, &l);
		INVARIANT(size() == dl->size());
		return std::equal(begin(), end(), dl->begin(), 
		binary_compose(
			mem_fun_ref(&meta_index_expr::must_be_equivalent_index), 
			dereference<count_ptr<const meta_index_expr> >(), 
			dereference<count_ptr<const meta_index_expr> >()
		)
		);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dynamic_meta_index_list::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Similar to resolve_index except that this takes a context 
	argument for unroll-time resolution.  
	\return populated const_index_list if all indices have been 
		successful resolved, else an empty list.
 */
const_index_list
dynamic_meta_index_list::unroll_resolve_indices(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const_index_list ret;
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const meta_index_expr> ind(*i);
		const count_ptr<const const_index>
			c_ind(ind.is_a<const const_index>());
		if (c_ind) {
			// direct reference copy
			ret.push_back(c_ind);
		} else {
			const count_ptr<const const_index>
				r_ind(ind->unroll_resolve_index(c));
			if (r_ind) {
				ret.push_back(r_ind);
			} else {
				// failed to resolve as constant!
				return const_index_list();
			}
		}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visibility hidden is ok.  
	Copied from meta_value_reference::positional_substituter.
	Consider making this a helper class template.  
 */
struct dynamic_meta_index_list::positional_substituter {
	typedef count_ptr<const meta_index_expr>	return_type;
	const template_formals_manager&		formals;
	const dynamic_param_expr_list&		exprs;

	positional_substituter(const template_formals_manager& f,
			const dynamic_param_expr_list& e) :
			formals(f), exprs(e) {
	}

	return_type
	operator () (const return_type& p) {
		return p->substitute_default_positional_parameters(
				formals, exprs, p);
	}

};      // end class positional_substituter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const meta_index_list>
dynamic_meta_index_list::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const meta_index_list>& l) const {
	INVARIANT(l == this);
	const count_ptr<this_type> temp(new this_type);
	NEVER_NULL(temp);
	util::reserve(*temp, parent_type::size());
	transform(begin(), end(), back_inserter(*temp),
		positional_substituter(f, e));
	if (equal(begin(), end(), temp->begin())) {
		// no substitutions, return this
		return l;
	} else {
		return temp;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
bool
dynamic_meta_index_list::resolve_multikey(
		excl_ptr<multikey_index_type>& k) const {
	k = excl_ptr<multikey_index_type>(
		multikey_index_type::make_multikey(size()));
	NEVER_NULL(k);
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		const count_ptr<const meta_index_expr> ip(*i);
		const count_ptr<const pint_expr> pi(ip.is_a<pint_expr>());
		// assert(pi); ?
		if (pi) {
			if (!pi->resolve_value((*k)[j])) {
				cerr << "Unable to resolve pint_expr at "
					"index position " << j << endl;
				return false;
			}
		} else {
			cerr << "Index at position " << j << 
				"is not a pint_expr!" << endl;
			return false;
		}
	}
	// nothing went wrong
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_meta_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(*this);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_meta_index_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer_list(f, AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_meta_index_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer_list(f, AS_A(parent_type&, *this));
	m.load_once_pointer_list(*this);
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

