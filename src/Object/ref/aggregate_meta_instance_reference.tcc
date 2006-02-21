/**
	\file "Object/ref/aggregate_meta_instance_reference.tcc"
	Implementation of aggregate_meta_instance_reference class.  
	$Id: aggregate_meta_instance_reference.tcc,v 1.1.2.3 2006/02/21 00:30:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/type/fundamental_type_reference.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class aggregate_meta_instance_reference method definitions

AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::aggregate_meta_instance_reference() :
		aggregate_meta_instance_reference_base(), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::aggregate_meta_instance_reference(
		const bool c) :
		aggregate_meta_instance_reference_base(c), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::
		~aggregate_meta_instance_reference() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::dimensions(void) const {
	return subreferences.front()->dimensions()
		+(this->_is_concatenation ? 0 : 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped from aggregate_meta_value_reference::dump().
	Accounts for cases where construction is partial.  
 */
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::dump(ostream& o,
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
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::dump_type_size(ostream& o) const {
	const subreference_ptr_type& front(subreferences.front());
	NEVER_NULL(front);
	front->get_type_ref()->dump(o);
	const size_t d = this->dimensions();
	if (d) {
		o << '{' << d << "-dim}";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const definition_base>
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::get_base_def(void) const {
	return subreferences.front()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::get_type_ref(void) const {
	return subreferences.front()->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregates cannot be scalar.  
 */
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context&) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::connect_port(
		instance_collection_base&, const unroll_context&) const {
	ICE_NEVER_CALL(cerr);
	return bad_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
const footprint_frame*
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::lookup_footprint_frame(
		const state_manager&) const {
	ICE_NEVER_CALL(cerr);
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shamelessly copy-modified from aggregate_meta_value_reference.  
	\param a is returned by reference as a constructed packed array
		of references.  
 */
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::unroll_references(
		const unroll_context& c, alias_collection_type& a) const {
	// collection collection type
	typedef	std::vector<alias_collection_type>	coll_coll_type;
	typedef	typename coll_coll_type::iterator	coll_coll_iterator;
	typedef	typename coll_coll_type::const_iterator
						const_coll_coll_iterator;
	typedef	typename alias_collection_type::key_type
							key_type;
		// should be multikey_index_type
	typedef	typename alias_collection_type::iterator
							target_iterator;
	coll_coll_type temp(subreferences.size());
{
	coll_coll_iterator ci(temp.begin());
	const const_iterator b(subreferences.begin()), e(subreferences.end());
	const_iterator i(b);
	// std::transform pattern
	for ( ; i!=e; ++i, ++ci) {
		// util::wtf_is(*i);
		const count_ptr<const parent_type>& lv(*i);
		if (lv->unroll_references(c, *ci).bad) {
			cerr << "Error unrolling subreference " <<
				distance(b, i) +1 << " of ";
			this->what(cerr) << endl;
			return bad_bool(true);
		}
		// else continue
	}
}               
	// aggregation, by concatenation or construction
	const size_t subdim = subreferences.front()->dimensions();
if (this->_is_concatenation) {
	// concatenation of arrays into like-dimension larger arrays
	FINISH_ME(Fang);
	return bad_bool(true);
} else {
	// is array construction
	if (subdim) {
		// is higher dimension array
		FINISH_ME(Fang);
		return bad_bool(true);
	} else {
		// is 1-D array, and all constituents are scalar
		// just copy pointer value-references over
		key_type k(1);
		k[0] = temp.size();
		a.resize(k);
		target_iterator ti(a.begin());
		const const_coll_coll_iterator b(temp.begin()), e(temp.end());
		const_coll_coll_iterator i(b);
		for ( ; i!=e; ++i, ++ti) {
			const key_type s(i->size());
			INVARIANT(!s.dimensions());
			*ti = i->front();
		}
		INVARIANT(ti == a.end());
		return bad_bool(false);
	}
}
}	// end method unroll_references

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::append_meta_instance_reference(
		const count_ptr<const meta_instance_reference_base>& p) {
	const count_ptr<const parent_type>
		next(p.template is_a<const parent_type>());
	if (next) {
		return this->append_meta_instance_reference(next);
	} else {
		cerr << "Can\'t append a ";
		p->what(cerr) << " to a ";
		this->what(cerr) << ".  " << endl;
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::append_meta_instance_reference(
		const count_ptr<const parent_type>& p) {
	NEVER_NULL(p);
	if (!subreferences.empty()) {
		// then we check against the first's dimensions
		const size_t fd = subreferences.front()->dimensions();
		const size_t pd = p->dimensions();
		if (fd != pd) {
			cerr << "Error: instance reference " <<
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
	Aggregates are non-scalar, and hence, cannot be ported!
 */
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<port_connection_base>
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>&) const {
	ICE_NEVER_CALL(cerr);
	return excl_ptr<port_connection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(this->subreferences);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	util::write_value(o, this->_is_concatenation);
	m.write_pointer_list(o, this->subreferences);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	util::read_value(i, this->_is_concatenation);
	m.read_pointer_list(i, this->subreferences);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__

