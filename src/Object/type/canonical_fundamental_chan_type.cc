/**
	\file "Object/type/canonical_fundamental_chan_type.cc"
	$Id: canonical_fundamental_chan_type.cc,v 1.1.4.2 2007/01/12 03:11:43 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_CONSTRUCTORS		0
#define	STACKTRACE_DESTRUCTORS		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <algorithm>
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/canonical_type.h"	// for generic data-type
#include "Object/type/channel_type_reference_base.h"
#include "Object/def/datatype_definition_base.h"
#include "Object/def/fundamental_channel_footprint.h"
#include "Object/persistent_type_hash.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"

#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.h"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"
#include "util/what.h"

namespace util {
// TODO: specialize reconstructor for de-serialization
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::canonical_fundamental_chan_type_base, 
	CANONICAL_FUNDAMENTAL_CHANNEL_TYPE_TYPE_KEY, 0)

SPECIALIZE_UTIL_WHAT(HAC::entity::canonical_fundamental_chan_type_base, 
	"canonical-fund-chan-type-base")

namespace memory {
using HAC::entity::canonical_fundamental_chan_type_base;

/**
	Comparison operator for set sorting.  
	Dereference-and-compare pointees.  
	TODO: guarantee that this is being used, as opposed to pointer
		address comparison.  
 */
bool
operator < (const count_ptr<const canonical_fundamental_chan_type_base>& l, 
		const count_ptr<const canonical_fundamental_chan_type_base>& r) {
	NEVER_NULL(l);
	NEVER_NULL(r);
	return *l < *r;
}

}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
// using util::memory::operator<;
using util::persistent_traits;
using std::cin;

//=============================================================================
// static initializers

// only needed for debugging, really
REQUIRES_STACKTRACE_STATIC_INIT
/**
	Need to partially order static global destructors because
	canonical_datatypes may contain pint_consts.  
	Any other shared pooled types we've forgotten?
 */
REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(pint_const)

//=============================================================================
/**
	This pool should be constructed before
	canonical_fundamental_chan_type_base's
	for proper destructor ordering.  
 */
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(fundamental_channel_footprint)

//=============================================================================
// class canonical_fundamental_chan_type method definitions

/**
	This must be initialized before the global registry, to ensure
	orderly destruction!
 */
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(canonical_fundamental_chan_type_base)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Global set of all instantiated built-in channel types.  
	Instantiated types should register and check with this set
	to eliminate redundancy.  
 */
canonical_fundamental_chan_type_base::global_registry_type
canonical_fundamental_chan_type_base::global_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private default constructor.  
 */
canonical_fundamental_chan_type_base::canonical_fundamental_chan_type_base() :
		persistent(), 
		datatype_list(), 
		footprint_cache(NULL) {
	STACKTRACE_CTOR_VERBOSE;
#if STACKTRACE_CONSTRUCTORS
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type_base::canonical_fundamental_chan_type_base(
		const datatype_list_type& d) :
		persistent(), 
		datatype_list(d), 
		footprint_cache(NULL) {
	STACKTRACE_CTOR_VERBOSE;
#if STACKTRACE_CONSTRUCTORS
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default destructor
canonical_fundamental_chan_type_base::~canonical_fundamental_chan_type_base() {
	STACKTRACE_DTOR_VERBOSE;
#if STACKTRACE_DESTRUCTORS
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
canonical_fundamental_chan_type_base::what(ostream& o) const {
	return o << "canonical-fund-chan-type";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ripped from canonical_generic_chan_type::dump()
 */
ostream&
canonical_fundamental_chan_type_base::dump(ostream& o, const char d) const {
	typedef	datatype_list_type::const_iterator	const_iterator;
	o << "chan";
	channel_type_reference_base::dump_direction(o, d);
	o << '(';
	INVARIANT(datatype_list.size());
	const_iterator i(datatype_list.begin());
	const const_iterator e(datatype_list.end());
	i->dump(o);
	for (i++; i!=e; i++) {
		i->dump(o << ", ");
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default: non-directional.
 */
ostream&
canonical_fundamental_chan_type_base::dump(ostream& o) const {
	return dump(o, '\0');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Comparison for set membership test.  
 */
bool
canonical_fundamental_chan_type_base::operator < (const this_type& r) const {
#if ENABLE_STACKTRACE
	this->dump(cerr << "comparing for ordering: ") << " vs. ";
	r.dump(cerr) << " ... ";
#endif
	const bool ret = std::lexicographical_compare(
		datatype_list.begin(), datatype_list.end(), 
		r.datatype_list.begin(), r.datatype_list.end());
#if ENABLE_STACKTRACE
	cerr << (ret ? "<" : ">=") << endl;
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type equivalence test.
	Ripped from
	canonical_generic_chan_type::must_be_connectibly_type_equivalent().
 */
bool
canonical_fundamental_chan_type_base::operator == (const this_type& t) const {
	typedef datatype_list_type::const_iterator      const_iterator;
#if 0
	this->dump(cerr << "comparing for equiv: ") << " vs. ";
	t.dump(cerr) << endl;
#endif
	if (datatype_list.size() != t.datatype_list.size())
		return false;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; ++i, ++j) {
		if (!i->must_be_connectibly_type_equivalent(*j))
			return false;
	}
	// else everything matches
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a built-in canonical channel type to a unique, re-usable, 
	type description.  
	Use with global registry avoids unnecessary replication.  
	TODO: concern with thread-safety (side-effect)?
 */
count_ptr<const canonical_fundamental_chan_type_base>
canonical_fundamental_chan_type_base::register_type(
		const datatype_list_type& d) {
	typedef	count_ptr<const this_type>		return_type;
	const return_type probe(new this_type(d));
	return register_globally(probe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const canonical_fundamental_chan_type_base>
canonical_fundamental_chan_type_base::register_globally(
		const count_ptr<const this_type>& p) {
	typedef global_registry_type::iterator		iterator;
	STACKTRACE_VERBOSE;
	const std::pair<iterator, bool> t(global_registry.insert(p));
#if ENABLE_STACKTRACE
	if (t.second) {
		STACKTRACE_INDENT_PRINT("new chan() registered." << endl);
	} else {
		STACKTRACE_INDENT_PRINT("old chan() re-used." << endl);
	}
	(*t.first)->dump(STACKTRACE_STREAM) << endl;
#endif
	NEVER_NULL(*t.first);
	return *t.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For debugging dump all registered built-in channel types.  
 */
ostream&
canonical_fundamental_chan_type_base::dump_global_registry(ostream& o) {
	typedef global_registry_type::const_iterator	const_iterator;
	o << "global chan() type registry:" << endl;
	const_iterator i(global_registry.begin());
	const const_iterator e(global_registry.end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->dump(o << '\t') << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor.
	TODO: support sized-types.  
 */
struct canonical_fundamental_chan_type_base::channel_footprint_refresher {
	fundamental_channel_footprint& 			cfp;
	explicit
	channel_footprint_refresher(fundamental_channel_footprint& f) :
		cfp(f) { }

	void
	operator () (const canonical_generic_datatype& d) {
		d.get_base_def()->count_channel_member(cfp);
	}

};	// end struct channel_footprint_refresher

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-evaluates the data-type list to produce a size summary
	of channel components.  
	TODO: accommodate sized-types.  
 */
void
canonical_fundamental_chan_type_base::refresh_footprint(void) const {
	// over-write previous, if any, else make new one, no need to reset
	footprint_cache = excl_ptr<fundamental_channel_footprint>(
		new fundamental_channel_footprint);
	// with pool-allocation, should be fast
	NEVER_NULL(footprint_cache);
	for_each(datatype_list.begin(), datatype_list.end(), 
		channel_footprint_refresher(*footprint_cache)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Refreshes all footprint summaries for registered channel types.  
 */
void
canonical_fundamental_chan_type_base::refresh_all_footprints(void) {
	typedef global_registry_type::iterator	iterator;
	iterator i(global_registry.begin());
	const iterator e(global_registry.end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->refresh_footprint();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type_base::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_collector_ref(m)
	);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type_base::write_pointer(
		const persistent_object_manager& m, ostream& o, 
		const count_ptr<const this_type>& t) {
	if (t) {
		typedef global_registry_type::const_iterator	const_iterator;
		const const_iterator f(global_registry.find(t));
		INVARIANT(f != global_registry.end());
	}
	m.write_pointer(o, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const canonical_fundamental_chan_type_base>
canonical_fundamental_chan_type_base::read_pointer(
		const persistent_object_manager& m, istream& i) {
	const count_ptr<canonical_fundamental_chan_type_base> temp;
	m.read_pointer(i, temp);
	// TODO: load global map! (responsibility here)
	if (temp) {
		m.load_object_once(temp);
//		m.please_delete(&*temp);	// don't keep around
		return register_globally(temp);
	} else {
		return count_ptr<const this_type>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type_base::write_object(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = datatype_list.size();
	util::write_value(o, s);
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_writer<canonical_generic_datatype>(
			&canonical_generic_datatype::write_object_base, m, o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: pointers are loaded into a heap-allocated structure, 
	but need to end up in the globally-managed set.
	Thus, the caller, in addition to calling m.read_pointer(), 
	needs to call a substituting function to load the structure into 
	a map and get the mapped pointer.  
	This effectively transfers data from non-mapped to mapped pointer.
	Don't forget to call m.mark_delete(), if necessary.
 */
void
canonical_fundamental_chan_type_base::load_object(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	util::read_value(i, s);
	datatype_list.resize(s);
	for_each(datatype_list.begin(), datatype_list.end(), 
		util::persistent_loader<canonical_generic_datatype>(
			&canonical_generic_datatype::load_object_base, m, i)
	);
	// Q: need to load the datatypes because need to immediately
	// sort using datatypes as keys for ordering comparison?
	// A: no, only addresses, not pointees, are needed in comparison!
}

//=============================================================================
// class canonical_fundamental_chan_type method definitions

canonical_fundamental_chan_type::canonical_fundamental_chan_type() :
		base_chan_type(), direction('\0') {
	STACKTRACE_CTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type::canonical_fundamental_chan_type(
		const base_chan_ptr_type& c) :
		base_chan_type(c), direction('\0') {
	STACKTRACE_CTOR_VERBOSE;
	NEVER_NULL(base_chan_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_fundamental_chan_type::~canonical_fundamental_chan_type() {
	STACKTRACE_DTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
canonical_fundamental_chan_type::dump(ostream& o) const {
	return base_chan_type->dump(o, direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check type equivalence.
	Ignores direction flag for type checking.
 */
bool
canonical_fundamental_chan_type::must_be_connectibly_type_equivalent(
		const this_type& t) const {
	NEVER_NULL(base_chan_type);
	NEVER_NULL(t.base_chan_type);
	const bool ret = (base_chan_type == t.base_chan_type) ||
		(*base_chan_type == *t.base_chan_type);
	if (!ret) {
		type_mismatch_error(cerr, *this, t);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For channels, connectibility == collectibility
	because there are no relaxed parameters.  
 */
bool
canonical_fundamental_chan_type::must_be_collectibly_type_equivalent(
		const this_type& t) const {
	return must_be_connectibly_type_equivalent(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Standard error message for mismatched types.  
 */
ostream&
canonical_fundamental_chan_type::type_mismatch_error(ostream& o,
		const this_type& l, const this_type& r) {
	o << "ERROR: types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(base_chan_type);
	base_chan_type->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	// TODO: verify that pointer is a member of global registry, 
	// and not a stray!
	base_type::write_pointer(m, o, base_chan_type);
	util::write_value(o, direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_fundamental_chan_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_VERBOSE;
	base_chan_type = base_type::read_pointer(m, i);	// special!
	util::read_value(i, direction);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

