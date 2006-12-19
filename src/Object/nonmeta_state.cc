/**
	\file "Object/nonmeta_state.cc"
	$Id: nonmeta_state.cc,v 1.1.2.1 2006/12/19 23:44:02 fang Exp $
 */

#include <iostream>
#include "Object/nonmeta_state.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/chan_traits.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class nonmeta_state_base method definitions

template <class Tag>
nonmeta_state_base<Tag>::nonmeta_state_base(const state_manager& sm) :
		pool() {
	const global_entry_pool<Tag>& p(sm.get_pool<Tag>());
	const size_t s = p.size();
	this->pool.resize(s);
}

template <class Tag>
nonmeta_state_base<Tag>::~nonmeta_state_base() { }

//=============================================================================
// class nonmeta_state_manager method definitions

nonmeta_state_manager::nonmeta_state_manager(const state_manager& sm) :
		bool_base_type(sm), 
		int_base_type(sm), 
		channel_base_type(sm) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_state_manager::~nonmeta_state_manager() { }

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

