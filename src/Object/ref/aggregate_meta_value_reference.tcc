/**
	\file "Object/ref/aggregate_meta_value_reference.tcc"
	Implementation of aggregate_meta_value_reference class.  
	$Id: aggregate_meta_value_reference.tcc,v 1.1.2.5 2006/02/19 21:57:36 fang Exp $
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
#include "Object/def/definition_base.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
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
		if (*i) (*i)->dump(o << delim, c);
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
never_ptr<const param_value_collection>
AGGREGATE_META_VALUE_REFERENCE_CLASS::get_coll_base(void) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<const param_value_collection>(NULL);
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
		+(_is_concatenation ? 0 : 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother checking at parse time, leave until unroll time.  
	\pre no subreferences may be NULL, array must be non-empty.
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::may_be_initialized(void) const {
	INVARIANT(!subreferences.empty());
	const_iterator i(subreferences.begin());
	const const_iterator e(subreferences.end());
	for ( ; i!=e; ++i) {
		if (!(*i)->may_be_initialized())
			return false;
		// else keep checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really should use an expression visitor for this...
	\pre no subreferences may be NULL.
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::must_be_initialized(void) const {
	INVARIANT(!subreferences.empty());
	const_iterator i(subreferences.begin());
	const const_iterator e(subreferences.end());
	for ( ; i!=e; ++i) {
		if (!(*i)->must_be_initialized())
			return false;
		// else keep checking...
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	INVARIANT(!subreferences.empty());
	const_iterator i(subreferences.begin());
	const const_iterator e(subreferences.end());
	for ( ; i!=e; ++i) {
		if (!(*i)->is_static_constant())
			return false;
		// else keep checking...
	}
	return true;
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
	return true;
} else {
	FINISH_ME(Fang);
	return false;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_value(
		const unroll_context& c, value_type& i) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concatenations and constructions cannot be scalar values.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::resolve_value(value_type& i) const {
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
	Packs subreferences into appropriate const_collection.  
	Don't forget to check dimension limits.  
	\pre subreferences non-empty, all non-NULL.
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const_param>
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve(
		const unroll_context& c) const {
	typedef count_ptr<const_collection_type>	return_type;
	typedef	vector<count_ptr<const_param> >		temp_type;
	typedef	temp_type::const_iterator		temp_iterator;
	typedef	temp_type::const_reference		temp_reference;
	typedef	typename parent_type::unroll_resolver	unroll_resolver_type;
	typedef typename const_collection_type::key_type	key_type;
	typedef typename const_collection_type::iterator	target_iterator;

	const size_t subdim = subreferences.front()->dimensions();
	temp_type temp;
	util::reserve(temp, subreferences.size());	// pre-allocate
	transform(subreferences.begin(), subreferences.end(), 
		back_inserter(temp), unroll_resolver_type(c));
if (this->_is_concatenation) {
	FINISH_ME(Fang);
	return return_type(NULL);
} else if (!subdim) {
	// we are constructing 1-dimension array from scalar subrefs.
	key_type size_1d(1);
	size_1d[0] = temp.size();
	const return_type ret(new const_collection_type(size_1d));
	NEVER_NULL(ret);
	target_iterator ti(ret->begin());
	const temp_iterator b(temp.begin()), e(temp.end());
	temp_iterator i(b);
	for ( ; i!=e; ++i, ++ti) {
		temp_reference p(*i);
		if (!p) {
			cerr << "Error unroll-resolving sub-reference "
				<< distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return return_type(NULL);
		}
		const count_ptr<const_expr_type>
			ce(p.template is_a<const_expr_type>());
		if (ce) {
			*ti = ce->static_constant_value();
		} else {
		const count_ptr<const_collection_type>
			cc(p.template is_a<const_collection_type>());
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
	// TODO: size-checking
	FINISH_ME(Fang);
	return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(subreferences);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, _is_concatenation);
	m.write_pointer_list(o, subreferences);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	read_value(i, _is_concatenation);
	m.read_pointer_list(i, subreferences);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__

