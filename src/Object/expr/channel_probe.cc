/**
	\file "Object/expr/channel_probe.cc"
	$Id: channel_probe.cc,v 1.2.8.1 2007/04/29 05:56:27 fang Exp $
 */

#include <iostream>
#include "Object/expr/channel_probe.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/expr/expr_visitor.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/pbool_const.h"
#include "Object/traits/instance_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/type/canonical_generic_datatype.h"
#include "Object/type/data_type_reference.h"
#include "Object/nonmeta_context.h"
#include "Object/nonmeta_variable.h"
#include "Object/nonmeta_state.h"
#include "util/persistent_object_manager.tcc"
#include "util/what.h"
#include "util/new_functor.h"
#include "util/memory/count_ptr.tcc"

namespace util {
	using HAC::entity::channel_probe;
	SPECIALIZE_UTIL_WHAT(channel_probe, "channel-probe")

	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		channel_probe, NONMETA_CHANNEL_PROBE_EXPR_TYPE_KEY, 0)
namespace memory {
	// explicitly instantiate, to prevent optimizing away at -O3
	template class count_ptr<const channel_probe>;
}
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;

//=============================================================================
// class channel_probe method definitions

channel_probe::channel_probe() : parent_type(), chan() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_probe::channel_probe(const chan_ref_ptr_type& c) :
		parent_type(), chan(c) {
	NEVER_NULL(chan);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_probe::~channel_probe() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(channel_probe)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_probe::dump(ostream& o, const expr_dump_context& c) const {
	return chan->dump(o << '#', c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: should chan be checked?
 */
count_ptr<const data_type_reference>
channel_probe::get_unresolved_data_type_ref(void) const {
	return bool_traits::built_in_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: should chan be checked?
 */
canonical_generic_datatype
channel_probe::get_resolved_data_type_ref(const unroll_context& c) const {
	return bool_traits::built_in_type_ptr->make_canonical_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_probe::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const bool_expr>
channel_probe::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const bool_expr>& p) const {
	INVARIANT(p == this);
	const chan_ref_ptr_type oc(chan->unroll_resolve_copy(c, chan));
	if (!oc) {
		return count_ptr<const bool_expr>(NULL);
	} else if (oc == chan) {
		return p;
	} else {
		return count_ptr<const this_type>(new this_type(oc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pbool_const>
channel_probe::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	typedef	count_ptr<const pbool_const>	const_ptr_type;
	typedef	const_ptr_type			return_type;
	INVARIANT(p == this);
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	if (!chan_index) {
		chan->dump(cerr << "at ",
			expr_dump_context::default_value) << endl;
		return return_type(NULL);
	} else {
		const ChannelState&
			nc(c.values.get_pool<channel_tag>()[chan_index]);
		return return_type(new pbool_const(nc.probe()));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
channel_probe::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_probe::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(chan);
	chan->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_probe::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, chan);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_probe::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, chan);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


