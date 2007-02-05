/**
	\file "Object/nonmeta_state.cc"
	$Id: nonmeta_state.cc,v 1.2.2.2 2007/02/05 04:32:32 fang Exp $
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
#include "util/binders.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::mem_fun_ref;
using std::for_each;
using util::bind2nd_argval;
using util::write_value;
using util::read_value;

//=============================================================================
// class nonmeta_state_base method definitions

template <class Tag>
nonmeta_state_base<Tag>::nonmeta_state_base() : pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
nonmeta_state_base<Tag>::__dump_all_subscriptions(ostream& o, 
		const state_manager& sm, const footprint& topfp) const {
	typedef	class_traits<Tag>		traits_type;
	const global_entry_pool<Tag>& ip(sm.template get_pool<Tag>());
	const size_t s = this->pool.size();
	size_t i = FIRST_VALID_NODE;
	for ( ; i<s; ++i) {
		const instance_type& nsi(this->pool[i]);
		if (nsi.has_subscribers()) {
			const global_entry<Tag>& ge(ip[i]);
			o << traits_type::tag_name << "[" << i << "]: \"";
			ge.dump_canonical_name(o, topfp, sm);
			o << "\" : ";
			nsi.dump_subscribers(o) << endl;
		}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
nonmeta_state_base<Tag>::__dump_state(ostream& o) const {
	typedef	typename pool_type::const_iterator	const_iterator;
	typedef	state_data_extractor<Tag>		extractor_type;
	const_iterator i(++pool.begin()), e(pool.end());
	size_t j = 1;
	for ( ; i!=e; ++i, ++j) {
		extractor_type::dump(o << '[' << j << "]\t", 
			extractor_type()(*i));
		// print event subscribers too?
		i->dump_subscribers(o << ", subs: ") << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpoint data value, but not subscribers.  
 */
template <class Tag>
bool
nonmeta_state_base<Tag>::save_checkpoint(ostream& o) const {
	const size_t s = pool.size();
	write_value(o, s);
	for_each(pool.begin(), pool.end(), 
		bind2nd_argval(mem_fun_ref(&instance_type::write), o)
	);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpoint data value, but not subscribers.  
 */
template <class Tag>
bool
nonmeta_state_base<Tag>::load_checkpoint(istream& i) {
	size_t s;
	read_value(i, s);
	pool.resize(s);
	for_each(pool.begin(), pool.end(), 
		bind2nd_argval(mem_fun_ref(&instance_type::read), i)
	);
	return !i;
}

//=============================================================================
// class nonmeta_state_manager method definitions

nonmeta_state_manager::nonmeta_state_manager() :
		bool_base_type(), 
		int_base_type(), 
		enum_base_type(), 
		channel_base_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
/**
	Formatted printing of names of all instances.  
 */
ostream&
nonmeta_state_manager::dump_state(ostream& o) const {
	o << "bool states:" << endl;
	nonmeta_state_base<bool_tag>::__dump_state(o);
	o << "int states:" << endl;
	nonmeta_state_base<int_tag>::__dump_state(o);
	o << "enum states:" << endl;
	nonmeta_state_base<enum_tag>::__dump_state(o);
	o << "channel states:" << endl;
	nonmeta_state_base<channel_tag>::__dump_state(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Formatted printing of names of all instances.  
 */
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Name all variables with subscribers at the moment.  
 */
ostream&
nonmeta_state_manager::dump_all_subscriptions(ostream& o, 
		const state_manager& sm, const footprint& topfp) const {
	nonmeta_state_base<bool_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<int_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<enum_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<channel_tag>::__dump_all_subscriptions(o, sm, topfp);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpoints only the VALUE of the data, excluding the 
	state of subscription.  
	\return true to signal an error.
 */
bool
nonmeta_state_manager::save_checkpoint(ostream& o) const {
	return nonmeta_state_base<bool_tag>::save_checkpoint(o) ||
		nonmeta_state_base<int_tag>::save_checkpoint(o) ||
		nonmeta_state_base<enum_tag>::save_checkpoint(o) ||
		nonmeta_state_base<channel_tag>::save_checkpoint(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the checkpointed VALUE of data, excluding subscription state.
 */
bool
nonmeta_state_manager::load_checkpoint(istream& i) {
	return nonmeta_state_base<bool_tag>::load_checkpoint(i) ||
		nonmeta_state_base<int_tag>::load_checkpoint(i) ||
		nonmeta_state_base<enum_tag>::load_checkpoint(i) ||
		nonmeta_state_base<channel_tag>::load_checkpoint(i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

