/**
	\file "Object/nonmeta_state.cc"
	$Id: nonmeta_state.cc,v 1.1.2.6 2007/01/13 21:06:53 fang Exp $
 */

#include <iostream>
#include <functional>
#include "Object/nonmeta_state.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/type/canonical_fundamental_chan_type.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::mem_fun_ref;

//=============================================================================
// class nonmeta_state_base method definitions

template <class Tag>
nonmeta_state_base<Tag>::nonmeta_state_base(const state_manager& sm) :
		pool() {
	const global_entry_pool<Tag>& p(sm.template get_pool<Tag>());
	const size_t s = p.size();
	this->pool.resize(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
nonmeta_state_base<Tag>::~nonmeta_state_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
nonmeta_state_base<Tag>::reset() {
	for_each(pool.begin(), pool.end(), mem_fun_ref(&instance_type::reset));
}

//=============================================================================
// class nonmeta_state_manager method definitions

/**
	This also allocates the ChannelState's fields according to their
	corresponding channel types.  
	\pre must have already refreshed the channel type footprint summaries.
 */
nonmeta_state_manager::nonmeta_state_manager(const state_manager& sm) :
		bool_base_type(sm), 
		int_base_type(sm), 
		enum_base_type(sm), 
		channel_base_type(sm) {
	typedef	global_entry_pool<channel_tag>	channel_entry_pool_type;
	typedef	channel_base_type::pool_type	channel_state_pool_type;
	typedef	channel_state_pool_type::iterator	channel_state_iterator;
	const channel_entry_pool_type&
		cep(sm.get_pool<channel_tag>());
	const size_t s = cep.size();
	channel_state_iterator i(channel_base_type::pool.begin());
	const channel_state_iterator e(channel_base_type::pool.end());
	INVARIANT(size_t(distance(i, e)) == s);
	size_t j = 1;	// 1-indexed, skip NULL entry
	for (++i; i!=e; ++i, ++j) {
		const global_entry<channel_tag>& ce(cep[j]);
		i->resize(ce.channel_type->footprint_size());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_state_manager::~nonmeta_state_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	0-initializes data.  
 */
void
nonmeta_state_manager::reset(void) {
	bool_base_type::reset();
	int_base_type::reset();
	enum_base_type::reset();
	channel_base_type::reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_state_manager::dump_struct(ostream& o, const state_manager& sm, 
		const footprint& topfp) const {
	{
		const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
		const size_t bools = bool_base_type::pool.size();
		size_t i = FIRST_VALID_NODE;
		for ( ; i<bools; ++i) {
			o << "bool[" << i << "]: \"";
			bp[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// bool_pool[i].dump_struct(o);
			o << endl;
		}
	}{
		const global_entry_pool<int_tag>& ip(sm.get_pool<int_tag>());
		const size_t ints = int_base_type::pool.size();
		size_t i = FIRST_VALID_NODE;
		for ( ; i<ints; ++i) {
			o << "int[" << i << "]: \"";
			ip[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// int_pool[i].dump_struct(o);
			o << endl;
		}
	}{
		const global_entry_pool<enum_tag>& ip(sm.get_pool<enum_tag>());
		const size_t enums = enum_base_type::pool.size();
		size_t i = FIRST_VALID_NODE;
		for ( ; i<enums; ++i) {
			o << "enum[" << i << "]: \"";
			ip[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// enum_pool[i].dump_struct(o);
			o << endl;
		}
	}{
		const global_entry_pool<channel_tag>&
			cp(sm.get_pool<channel_tag>());
		const size_t chans = channel_base_type::pool.size();
		size_t i = FIRST_VALID_NODE;
		for ( ; i<chans; ++i) {
			o << "chan[" << i << "]: \"";
			cp[i].dump_canonical_name(o, topfp, sm);
			o << "\" ";
			// no static structural information
			// channel_pool[i].dump_struct(o);
			o << endl;
		}
	}
	return o;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

