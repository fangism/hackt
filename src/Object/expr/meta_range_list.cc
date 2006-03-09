/**
	\file "Object/art_object_expr.cc"
	Class method definitions for semantic expression.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: meta_range_list.cc,v 1.11.4.1 2006/03/09 05:51:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_RANGE_LIST_CC__
#define	__HAC_OBJECT_EXPR_META_RANGE_LIST_CC__

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/common/multikey_index.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pint_range.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/persistent_type_hash.h"

#include "util/reserve.h"
#include "util/stacktrace.h"
#include "util/multikey.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {

SPECIALIZE_UTIL_WHAT(HAC::entity::const_range_list, 
		"const-range-list")
SPECIALIZE_UTIL_WHAT(HAC::entity::dynamic_meta_range_list, 
		"dynamic_meta_range_list")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::const_range_list, CONST_RANGE_LIST_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::dynamic_meta_range_list, DYNAMIC_RANGE_LIST_TYPE_KEY, 0)

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
using util::persistent_traits;
using util::persistent_object_manager;
using std::istream;
#include "util/using_ostream.h"
using std::_Select1st;
using std::_Select2nd;
using util::write_value;
using util::read_value;

//=============================================================================
// class meta_range_list method definitions

//=============================================================================
// class const_range_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::const_range_list() : meta_range_list(), list_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::const_range_list(const size_t s) :
		meta_range_list(), list_type() {
	util::reserve(AS_A(list_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implicit conversion.  
 */
const_range_list::const_range_list(const list_type& l) :
		meta_range_list(), list_type(l) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implicit conversion.  
 */
const_range_list::const_range_list(const alt_list_type& l) :
		meta_range_list(), list_type(l.begin(), l.end()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit conversion from an index list to a range list.  
	Only available for constants, of course.
	Converts x[n] to x[n..n].
	Useful for static range-checking of indices.  
 */
const_range_list::const_range_list(const const_index_list& i) :
		meta_range_list(), list_type() {
	const_index_list::const_iterator j(i.begin());
	for ( ; j!=i.end(); j++) {
		const count_ptr<const_index> k(*j);
		NEVER_NULL(k);
		const count_ptr<pint_const> p(k.is_a<pint_const>());
		const count_ptr<const_range> r(k.is_a<const_range>());
		if (p) {
			const int min_max = p->static_constant_value();
			push_back(const_range(min_max, min_max));	// copy
		} else {
			NEVER_NULL(r);
			push_back(*r);		// deep copy
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list::~const_range_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(const_range_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range_list::dump(ostream& o) const {
	return dump(o, expr_dump_context::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++)
		i->dump(o, c);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_range_list::dump_force(ostream& o) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++)
		i->dump_force(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
const_range_list::size(void) const {
	return list_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if all ranges in list are valid.  
 */
bool
const_range_list::is_valid(void) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		if (!i->is_sane())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for overlap between two static multidimensional index ranges.  
	If argument is actually dynamic, can only conservatively return false.  
	\return a const_range_list which is either false if there's no overlap, 
		or the valid overlap range if there is overlap.  
 */
const_range_list
const_range_list::static_overlap(const meta_range_list& r) const {
#if 0
	// DEBUG
	cerr << "In const_range_list::static_overlap with this = "
		<< this << endl;
	r.dump(cerr << "const meta_range_list& r = ") << endl;
#endif
	const const_range_list* s = IS_A(const const_range_list*, &r);
	const_range_list ret;	// initially empty
	if (s) {
		// dimensionality should match, or else!
		INVARIANT(size() == s->size());    
		const_iterator i(begin());
		const_iterator j(s->begin());
		for ( ; i!=end(); i++, j++) {
			// check for index overlap in ALL dimensions
			const const_range& ir(*i);
			const const_range& jr(*j);
			const_range o(ir.static_overlap(jr));
			ret.push_back(o);
			if (!o.empty())
				continue;
			else return const_range_list();
			// if there is any dimension without overlap, false
		}
		// else there is some overlap in all dimensions
		return ret;
	} else {	// argument is dynamic, not static
		// conservatively return false
		// overlap is possible but not definite, can't reject now.  
		return const_range_list();		// empty list
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Takes a list of ranges and converts back to indices, using a 
	reference index list, resembling the original index list.  
	Useful for converting results from multidimensional_sparse_set
	queries (range lists) back into index form where needed.  
	\param il the reference index list.
 */
const_index_list
const_range_list::revert_to_indices(const const_index_list& il) const {
	// let's actually bother to check consistency of the reference list
	const_index_list ret;
	// have a better idea... punt this for now
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNNECESSARY
/**
	\param il should be a reference index list for this range list.  
	\return copy of this range list with fewer dimensions, 
		collapsing the dimensions that were indexed by single ints.  
 */
const_range_list
const_range_list::collapsed_dimension_ranges(
		const const_index_list& il) const {
	const_index_list ret;
	INVARIANT(size() == il.size());
	const_iterator i(begin());
	const_index_list::const_iterator j(il.begin());
	for ( ; j!=il.end(); i++, j++) {
		const count_ptr<const pint_const>	// or pint_const
			pi(j->is_a<pint_const>());
		if (pi) {
			INVARIANT(i != end());
			INVARIANT(i->first == pi->static_constant_value());
			INVARIANT(i->first == i->second);
		} else {
			const count_ptr<const const_range>
				pr(j->is_a<const_range>());
			NEVER_NULL(pr);
			INVARIANT(pr->first == i->first);
			INVARIANT(pr->second == i->second);
			ret.push_back(*pr);
		}
	}
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ABANDONING, see comments within
/**
	Collapses the multidimensional range list using an
	reference index list as an argument.  
	For each index element of the argument that is an int, 
	not a range, shorten the range list in that dimension.  
	Make sure that the range is unity, as a sanity check.  
	Warning: list modifies itself by deleting elements, 
		may break with old shitty compiler libraries, 
		depending on the implementation.  
	\param il is the reference index list.  
 */
void
const_range_list::collapse_dimensions_wrt_indices(const const_index_list& il) {
	INVARIANT(size() >= il.size());
	iterator i(begin());
	const_index_list::const_iterator j(il.begin());
	for ( ; j!=il.end(); i++, j++) {
		const count_ptr<const pint_const>	// or pint_const
			pi(j->is_a<pint_const>());
		if (pi) {
			INVARIANT(i != end());
			INVARIANT(i->first == i->second);
#if 1
			i = erase(i);	// potential libstdc++ bug? don't trust

//			iterator ipp(i);
//			ipp++;
//			erase(i, ipp);	// doesn't work either...
#else
			// shit code here... virtual erasing:
			iterator iw(i);
			const_iterator ir(iw);
			ir++;		// read one-ahead
			for ( ; ir!=end(); iw++, ir++) {
				iw->first = ir->first;
				iw->second = ir->second;
			}
			iw->first = iw->second = -666;	// garbage
			erase(iw);	// be-tail
#endif
		} else {
			const count_ptr<const const_range>
				pr(j->is_a<const_range>());
			NEVER_NULL(pr);
			INVARIANT(pr->first == i->first);
#if 1
			if (pr->second != i->second) {
				cerr << "pr->second = " << pr->second << endl;
				cerr << "i->second = " << i->second << endl;
			}
			// following assertion produces different results
			// on gcc-3.2,3.3 vs 3.4
#endif
			INVARIANT(pr->second == i->second);
		}
	}
	// any remaining indices are kept
}
ABANDONING
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Size equality of two multidimensional ranges.  
	Also reports error in size mismatch to stderr.  
 */
bool
const_range_list::is_size_equivalent(const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	int dim = 1;
	const_iterator i(begin());
	const_iterator j(c.begin());
	for ( ; i!=end(); i++, j++, dim++) {
		const int ldiff = i->second -i->first;
		const int rdiff = j->second -j->first;
		if (ldiff != rdiff) {
			cerr << "Size of dimension " << dim <<
				" does not match!  got: " << ldiff+1 <<
				" and " << rdiff+1 << "." << endl;
			return false;
		}
	}
	INVARIANT(j == c.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether two multidimensional range lists are identical.  
	Not just whether or not the size of the spanned ranges are equal.  
 */
bool
const_range_list::operator == (const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	const_iterator i(begin());
	const_iterator j(c.begin());
	for ( ; i!=end(); i++, j++) {
		if (*i != *j)
			return false;
	}
	INVARIANT(j == c.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
const_range_list::must_be_formal_size_equivalent(
		const meta_range_list& rl) const {
	const const_range_list* const crl = IS_A(const const_range_list*, &rl);
	if (crl) {
		return is_size_equivalent(*crl);
	} else {
		const dynamic_meta_range_list* const
			drl = IS_A(const dynamic_meta_range_list*, &rl);
		INVARIANT(drl);
		return drl->must_be_formal_size_equivalent(*this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range_list::resolve_ranges(const_range_list& r) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
const_range_list::unroll_resolve_rvalues(const_range_list& r, 
		const unroll_context& c) const {
	r = *this;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
excl_ptr<multikey_index_type>
const_range_list::lower_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), _Select1st<const_range>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<multikey_index_type>
const_range_list::upper_multikey(void) const {
	typedef	excl_ptr<multikey_index_type>	return_type;
	return_type ret(multikey_index_type::make_multikey(size()));
	NEVER_NULL(ret);
	transform(begin(), end(), ret->begin(), _Select2nd<const_range>());
	return ret;
}
#else
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_range_list::lower_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), _Select1st<const_range>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
multikey_index_type
const_range_list::upper_multikey(void) const {
	typedef	multikey_index_type	return_type;
	return_type ret(size());
	transform(begin(), end(), ret.begin(), _Select2nd<const_range>());
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k the multikey generator to assign and fill.  
 */
template <size_t D>
void
const_range_list::make_multikey_generator(
		multikey_generator<D, pint_value_type>& k) const {
	typedef multikey_generator<D, pint_value_type>	arg_type;
	INVARIANT(size() <= D);  // else error on user!
	typename arg_type::base_type::iterator li(k.lower_corner.begin());
	typename arg_type::base_type::iterator ui(k.upper_corner.begin());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

// explicit instantiation thereof
// I know, this could be a pain to maintain, 
// this is only temporary, bear with me.
#define	INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(D)	\
template void							\
const_range_list::make_multikey_generator(			\
	multikey_generator<D, pint_value_type>& ) const;

// INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(0)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(1)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(2)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(3)
INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR(4)

#undef	INSTANTIATE_CONST_RANGE_LIST_MULTIKEY_GENERATOR

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the sizes of the ranges spanned.  
	Return type should be pint_const_collection::array_type::key_type
	\pre this->size() must be valid.
 */
multikey_index_type
const_range_list::resolve_sizes(void) const {
	multikey_index_type ret(size());
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		if (i->empty()) {
			// then we have a bad range
			THROW_EXIT;
		}
		ret[j] = i->size();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
const_range_list::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
const_range_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const const_range& cr(*i);
		write_value(f, cr.first);
		write_value(f, cr.second);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
const_range_list::load_object(const persistent_object_manager& m, istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		const_range cr;
		read_value(f, cr.first);
		read_value(f, cr.second);
		push_back(cr);
	}
}

//=============================================================================
// class dynamic_meta_range_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_range_list::dynamic_meta_range_list() :
		meta_range_list(), list_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_range_list::dynamic_meta_range_list(const size_t s) :
		meta_range_list(), list_type() {
	util::reserve(AS_A(list_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dynamic_meta_range_list::~dynamic_meta_range_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(dynamic_meta_range_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dynamic_meta_range_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		(*i)->dump(o, c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
dynamic_meta_range_list::size(void) const {
	return list_type::size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if any of the range depend on a relaxed formal parameter
		and also prints an error.  
 */
bool
dynamic_meta_range_list::is_relaxed_formal_dependent(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		const count_ptr<const pint_range> pr(*i);
		NEVER_NULL(pr);
		if (pr->is_relaxed_formal_dependent()) {
			cerr << "ERROR: range at position " <<
				std::distance(begin(), i)+1 <<
				" depends on a formal parameter (forbidden)."
				<< endl;
			return true;
		}
		// else continue checking
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_range_list::is_static_constant(void) const {
	const_iterator i(begin());
	for ( ; i!=end(); i++) {
		const count_ptr<const pint_range> pr(*i);
		NEVER_NULL(pr);
		if (!pr->is_static_constant())
			return false;
		// else continue checking
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overlap is indefinite with dynamic ranges, conservatively.  
 */
const_range_list
dynamic_meta_range_list::static_overlap(const meta_range_list& r) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r reference to return value of const_range_list, 
		where final range bounds are saved.  
	\return true if success, false if fail.  
 */
good_bool
dynamic_meta_range_list::resolve_ranges(const_range_list& r) const {
	// resolve ranges each step, each dynamic_range
	r.clear();
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const_range c;
		const count_ptr<const pint_range> ip(*i);
		if (ip->resolve_range(c).good) {
			r.push_back(c);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
dynamic_meta_range_list::unroll_resolve_rvalues(const_range_list& r, 
		const unroll_context& c) const {
	INVARIANT(r.empty());
	// write as transform?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const_range cr;
		const count_ptr<const pint_range> ip(*i);
		if (ip->unroll_resolve_range(c, cr).good) {
			r.push_back(cr);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
dynamic_meta_range_list::must_be_formal_size_equivalent(
		const meta_range_list& rl) const {
	const const_range_list* const crl = IS_A(const const_range_list*, &rl);
	if (crl) {
		INVARIANT(size() == crl->size());
		const_iterator i(begin());
		const const_iterator e(end());
		const_range_list::const_iterator j(crl->begin());
		// use some std:: algorithm ... later
		for ( ; i!=e; i++, j++) {
			// NEVER_NULL(...)
			if (!(*i)->must_be_formal_size_equivalent(*j))
				return false;
		}
	} else {
		const dynamic_meta_range_list* const
			drl = IS_A(const dynamic_meta_range_list*, &rl);
		INVARIANT(drl);
		INVARIANT(size() == drl->size());
		const_iterator i(begin());
		const const_iterator e(end());
		const_iterator j(drl->begin());
		// use some std:: algorithm ... later
		for ( ; i!=e; i++, j++) {
			// NEVER_NULL(...)
			if (!(*i)->must_be_formal_size_equivalent(**j))
				return false;
		}
	}
	// else no errors found
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
dynamic_meta_range_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const pint_range> ip(*i);
		ip->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
dynamic_meta_range_list::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, size());		// how many exprs to expect?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const pint_range> ip(*i);
		m.write_pointer(f, ip);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_meta_range_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	size_t s, i=0;
	read_value(f, s);		// how many exprs to expect?
	for ( ; i<s; i++) {
		count_ptr<pint_range> ip;
		m.read_pointer(f, ip);
#if 1
		if (ip)
			m.load_object_once(ip);
#endif
		push_back(ip);
	}
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

#endif	// __HAC_OBJECT_AEXPR_META_RANGE_LIST_CC__

