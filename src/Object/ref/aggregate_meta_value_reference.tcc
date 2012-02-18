/**
	\file "Object/ref/aggregate_meta_value_reference.tcc"
	Implementation of aggregate_meta_value_reference class.  
	$Id: aggregate_meta_value_reference.tcc,v 1.14 2007/04/15 05:52:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE			0
#endif

#include <iostream>
#include <algorithm>
#include <iterator>
#include "Object/ref/aggregate_meta_value_reference.h"
#include "Object/ref/aggregate_reference_collection_base.tcc"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/meta_value_reference.h"
#include "Object/def/definition_base.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/const_param.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/type/param_type_reference.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.h"
#include "util/reserve.h"
#include "util/multikey.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::distance;
using std::copy;
using std::transform;
using std::back_inserter;
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class aggregate_meta_value_reference method definitions

AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_VALUE_REFERENCE_CLASS::aggregate_meta_value_reference() :
		aggregate_meta_value_reference_base(), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_VALUE_REFERENCE_CLASS::~aggregate_meta_value_reference() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Accounts for cases where construction is partial.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_VALUE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	const char* delim = (this->_is_concatenation ? " # " : ", ");
	if (!this->_is_concatenation) o << "{ ";
if (subreferences.size()) {
	const_iterator i(subreferences.begin());
	const const_iterator e(subreferences.end());
	if (*i) (*i)->dump(o, c);
	for (++i; i!=e; ++i) {
		o << delim;
		if (*i) (*i)->dump(o, c);
	}
}
	if (!this->_is_concatenation) o << " }";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregates have more than one base collection!
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const param_value_placeholder>
AGGREGATE_META_VALUE_REFERENCE_CLASS::get_coll_base(void) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<const param_value_placeholder>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just adds a reference to the end of the list.  (push_back)
	Dimension checking done later.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::append_meta_value_reference(
		const count_ptr<const expr_base_type>& p) {
	NEVER_NULL(p);
	if (!subreferences.empty()) {
		// then we check against the first's dimensions
		const size_t fd = subreferences.front()->dimensions();
		const size_t pd = p->dimensions();
		if (fd != pd) {
			cerr << "Error: value reference " <<
				subreferences.size() +1 << " is " << pd
				<< "-dimensional, but the former references "
				"in the aggregate are " << fd <<
				"-dimensional." << endl;
			return good_bool(false);
		}
		// punt detail dimension checks until unroll-time.
	}
	subreferences.push_back(p);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a subreference to the end of the list.
	\return good if argument was of the correct type.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::append_meta_value_reference(
		const count_ptr<const param_expr>& p) {
	const count_ptr<const expr_base_type>
		next(p.template is_a<const expr_base_type>());
	if (next) {
		return this->append_meta_value_reference(next);
	} else {
		cerr << "Can\'t append a ";
		p->what(cerr) << " to a ";
		this->what(cerr) << ".  " << endl;
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is a concatenation, then resulting dimension is the 
	same as constituents.  If is array-construction, then 
	dimension is 1 greater than constituents.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
AGGREGATE_META_VALUE_REFERENCE_CLASS::dimensions(void) const {
	return subreferences.front()->dimensions()
		+(this->_is_concatenation ? 0 : 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just conservatively return false.
	Following this, the reference is expected to produce a
	constant SCALAR value, which this cannot produce.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::is_relaxed_formal_dependent(void) const {
	INVARIANT(!subreferences.empty());
	const_iterator i(subreferences.begin());
	const const_iterator e(subreferences.end());
	for ( ; i!=e; ++i) {
		if ((*i)->is_relaxed_formal_dependent())
			return true;
		// else keep checking...
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename AGGREGATE_META_VALUE_REFERENCE_CLASS::value_type
AGGREGATE_META_VALUE_REFERENCE_CLASS::static_constant_value(void) const {
	ICE_NEVER_CALL(cerr);
	return value_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for equivalence between formal references to formal parameters, 
	for template signature equivalence.  
	This may conservatively return false.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::must_be_equivalent(
		const expr_base_type& b) const {
	STACKTRACE_VERBOSE;
	const this_type* const vr = IS_A(const this_type*, &b);
if (vr) {
	if (this->_is_concatenation != vr->_is_concatenation) {
		return false;
	}
	const size_t ls = subreferences.size();
	const size_t rs = vr->subreferences.size();
	INVARIANT(ls && rs);
	if (ls != rs) {
		return false;
	}
	const_iterator li(subreferences.begin()), ri(vr->subreferences.begin());
	const const_iterator le(subreferences.end());
	for ( ; li!=le; ++li, ++ri) {
		if (!(*li)->must_be_equivalent(**ri))
			return false;
	}
	INVARIANT(ri == vr->subreferences.end());
	return true;
} else {
	// don't necessarily need to finish, could just leave as is
	FINISH_ME(Fang);
	return false;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_defined(
		const unroll_context&, pbool_value_type&) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_value(
		const unroll_context&, value_type&) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::resolve_value(value_type&) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregate references cannot be indexed... yet.
	(I think it'd be a bad idea.)
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
AGGREGATE_META_VALUE_REFERENCE_CLASS::resolve_dimensions(void) const {
	ICE_NEVER_CALL(cerr);
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_dimensions(
		const unroll_context&) const {
	ICE_NEVER_CALL(cerr);
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregate values never result in scalars, be it concatenation
	or array construction, so this will always fail.  
	\return NULL to signal error.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename AGGREGATE_META_VALUE_REFERENCE_CLASS::const_expr_type>
AGGREGATE_META_VALUE_REFERENCE_CLASS::__unroll_resolve_rvalue(
		const unroll_context&, 
		const count_ptr<const expr_base_type>&) const {
	cerr << "Error: got " << util::what<this_type>::name() << 
		" where scalar value was expected." << endl;
	return count_ptr<const const_expr_type>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Packs subreferences into appropriate const_collection.  
	Don't forget to check dimension limits.  
	\pre subreferences non-empty, all non-NULL.
	\pre for concatenation, subreferences must never be scalar.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const const_param>
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_rvalues(
		const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	typedef count_ptr<const const_collection_type>	return_type;
	typedef count_ptr<const_collection_type>	temp_ret_type;
	typedef	vector<count_ptr<const const_param> >	temp_type;
	typedef	temp_type::const_iterator		temp_iterator;
	typedef	temp_type::const_reference		temp_reference;
	typedef	typename parent_type::unroll_resolver	unroll_resolver_type;
	typedef typename const_collection_type::key_type	key_type;
	typedef typename const_collection_type::iterator	target_iterator;

	INVARIANT(p == this);
	const size_t subdim = subreferences.front()->dimensions();
	temp_type temp;
	util::reserve(temp, subreferences.size());	// pre-allocate
	transform(subreferences.begin(), subreferences.end(), 
		back_inserter(temp), unroll_resolver_type(c));
	const temp_iterator b(temp.begin()), e(temp.end());
if (this->_is_concatenation) {
	// most of this code copy-modified from construction case, below
	typedef	vector<return_type>			sub_type;
	// transfer temp to sub
	sub_type sub;
	sub.reserve(temp.size());
	temp_iterator i(b);
	for ( ; i<e; ++i) {
		// we already statically know the dimensionality of sub-refs
		if (!*i) {
			cerr << "Error unroll-resolving " << subdim << 
				"D sub-reference "
				<< distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return return_type(NULL);
		}
		sub.push_back(i->template is_a<const const_collection_type>());
		// sub-references must never be scalar!
		NEVER_NULL(sub.back());	// assert cast succeeded
		INVARIANT(sub.back()->dimensions() == this->dimensions());
	}
	// evaluate trailing dimensions (after 1st must match)
	// typedef	typename const_collection_type::key_type	array_size_type;
	// typedef	vector<array_size_type>			size_array_type;
	typedef	size_array_type::const_iterator	size_iterator;
	typedef	typename sub_type::const_iterator	const_sub_iterator;
	size_array_type sub_sizes;
	const const_sub_iterator sb(sub.begin()), se(sub.end());
{
	sub_sizes.reserve(temp.size());
	const_sub_iterator si(sb);
	for ( ; si!=se; ++si) {
		sub_sizes.push_back((*si)->array_dimensions());
	}
}
	if (!check_concatenable_subarray_sizes(sub_sizes).good) {
		// already have error message
		return return_type(NULL);
	}
	// start by copying first chunk of values
	const temp_ret_type ret(new const_collection_type(**sb));
{
	// the magic: the resulting collection is just serial composition
	// of constituents' values from the packed-arrays!  Holy guacamole!
	const_sub_iterator si(sb+1);
	for ( ; si!=se; ++si) {
		*ret += **si;
	}
}
	return ret;
} else if (!subdim) {
	// we are constructing 1-dimension array from scalar subrefs.
	key_type size_1d(1);
	size_1d[0] = temp.size();
	const temp_ret_type ret(new const_collection_type(size_1d));
	NEVER_NULL(ret);
	target_iterator ti(ret->begin());
	temp_iterator i(b);
	for ( ; i!=e; ++i, ++ti) {
		temp_reference pr(*i);
		if (!pr) {
			cerr << "Error unroll-resolving sub-reference "
				<< distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return return_type(NULL);
		}
		const count_ptr<const const_expr_type>
			ce(pr.template is_a<const const_expr_type>());
		if (ce) {
			*ti = ce->static_constant_value();
		} else {
		const return_type
			cc(pr.template is_a<const const_collection_type>());
		if (cc) {
			*ti = cc->static_constant_value();
		} else {
			ICE(cerr, 
				cerr << "Unhandled case of constant resolution."
					<< endl;
			);
		}
		}
	}
	INVARIANT(ti == ret->end());
	return ret;
} else {
	// we are constructing N-dimension array from N-1-dim. subrefs.
	// constituents must be at least 1D
	typedef	vector<return_type>			sub_type;
	// transfer temp to sub
	sub_type sub;
	sub.reserve(temp.size());
	temp_iterator i(b);
	for ( ; i<e; ++i) {
		// we already statically know the dimensionality of sub-refs
		if (!*i) {
			cerr << "Error unroll-resolving " << subdim << 
				"D sub-reference "
				<< distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return return_type(NULL);
		}
		sub.push_back(i->template is_a<const const_collection_type>());
		NEVER_NULL(sub.back());	// assert cast succeeded
		INVARIANT(sub.back()->dimensions() == subdim);
	}
	// all dimensions must match for array construction
	typedef	typename const_collection_type::key_type	array_size_type;
	// typedef	vector<array_size_type>			size_array_type;
	typedef	typename size_array_type::const_iterator	size_iterator;
	typedef	typename sub_type::const_iterator	const_sub_iterator;
	size_array_type sub_sizes;
	const const_sub_iterator sb(sub.begin()), se(sub.end());
{
	sub_sizes.reserve(temp.size());
	const_sub_iterator si(sb);
	for ( ; si!=se; ++si) {
		sub_sizes.push_back((*si)->array_dimensions());
	}
}
	if (!check_constructible_subarray_sizes(sub_sizes).good) {
		// already have error message
		return return_type(NULL);
	}
{
	const array_size_type& head_size(sub_sizes.front());
	array_size_type new_size(head_size.size() +1);
	new_size.front() = 0;	// reset 1st dimension as 0, then grow
	copy(head_size.begin(), head_size.end(), new_size.begin() +1);
	const temp_ret_type ret(new const_collection_type(new_size));

	// the magic: the resulting collection is just serial composition
	// of constituents' values from the packed-arrays!  Holy guacamole!
	const_sub_iterator si(sb);
	for ( ; si!=se; ++si) {
		*ret += **si;
	}
	return ret;
}
}	// end if (!subdim)
}	// end method unroll_resolve_rvalues

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregates subreference collections into one packed collection. 
	\param a cell-written collection of lvalue references, 
		unrolled by this aggregate reference.  
	NOTE: rvalue collections should be obtained by unroll_resolve_rvalues.
		(Perhaps we should name it so..., call this unroll_lvalues.)
	\pre subreferences have equal dimension, but specific
		sizes in each dimension are yet unknown.  
	\return bad if any of the following error conditions occurs.
		Any subreference contains a non-lvalue reference, 
		because all references need to be assignable.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_lvalue_references(
		const unroll_context& c,
		value_reference_collection_type& a) const {
	// collection collection type
	typedef	vector<value_reference_collection_type>	coll_coll_type;
	typedef	typename coll_coll_type::iterator	coll_coll_iterator;
	typedef	typename coll_coll_type::const_iterator
						const_coll_coll_iterator;
	typedef	typename value_reference_collection_type::key_type
							key_type;
		// should be multikey_index_type
	typedef	typename value_reference_collection_type::iterator
							target_iterator;
	coll_coll_type temp(subreferences.size());
{
	coll_coll_iterator ci(temp.begin());
	const const_iterator b(subreferences.begin()), e(subreferences.end());
	const_iterator i(b);
	// std::transform pattern
	for ( ; i!=e; ++i, ++ci) {
		const count_ptr<const parent_type>
			lv(i->template is_a<const parent_type>());
		if (!lv) {
			cerr << "Error unrolling aggregate lvalue reference, "
				"got rvalue in subreference " <<
				distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return bad_bool(true);
		} else if (lv->unroll_lvalue_references(c, *ci).bad) {
			cerr << "Error unrolling subreference " <<
				distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return bad_bool(true);
		}
		// else continue
	}
}
	// collect and evaluate subreference dimensions
	size_array_type sub_sizes;
{
	const const_coll_coll_iterator sb(temp.begin()), se(temp.end());
	sub_sizes.reserve(temp.size());
	const_coll_coll_iterator si(sb);
	for ( ; si!=se; ++si) {
		sub_sizes.push_back(si->size());        // multikey
	}
}
	// aggregation, by concatenation or construction
	// the magic resizing happens here
	if (!check_and_resize_packed_array(a, sub_sizes).good) {
		// already have error message
		return bad_bool(true);
	}
	// use same code for all cases, even with sub-dim == 0
	// just copy pointer value-references over
	const const_coll_coll_iterator b(temp.begin()), e(temp.end());
	const_coll_coll_iterator i(b);
	for ( ; i!=e; ++i) {
		// operator overload for growing packed_array_generic
		a += *i;
	}
	return bad_bool(false);
}	// end method unroll_lvalue_references

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to unroll_resolve_rvalues that returns resolved
	values suitable for nonmeta representations.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename AGGREGATE_META_VALUE_REFERENCE_CLASS::expr_base_type>
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return this->unroll_resolve_rvalues(c, p)
		.template is_a<const expr_base_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Bound transformer functor.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
struct AGGREGATE_META_VALUE_REFERENCE_CLASS::positional_substituter {
	typedef	count_ptr<const expr_base_type>	return_type;
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

};	// end class positional_substituter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs default parameter expression substitution
	copy-on-write if any substitutions occurred.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename AGGREGATE_META_VALUE_REFERENCE_CLASS::expr_base_type>
AGGREGATE_META_VALUE_REFERENCE_CLASS::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const expr_base_type>		return_type;
	INVARIANT(p == this);
	const count_ptr<this_type> temp(new this_type);
	NEVER_NULL(temp);
	util::reserve(temp->subreferences, subreferences.size());
		// pre-allocate
	transform(subreferences.begin(), subreferences.end(), 
		back_inserter(temp->subreferences),
		positional_substituter(f, e));
	if (equal(subreferences.begin(), subreferences.end(), 
			temp->subreferences.begin())) {
		// no substitutions, return this
		return p;
	} else {
		return temp;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(this->subreferences);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, this->_is_concatenation);
	m.write_pointer_list(o, this->subreferences);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	read_value(i, this->_is_concatenation);
	m.read_pointer_list(i, this->subreferences);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__

