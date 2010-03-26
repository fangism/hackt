/**
	\file "Object/nonmeta_state.cc"
	$Id: nonmeta_state.cc,v 1.4.24.5 2010/03/26 01:31:24 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <functional>
#include "Object/nonmeta_state.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/def/footprint.h"
#else
#include "Object/state_manager.h"
#endif
#include "Object/global_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "common/TODO.h"
#include "util/binders.h"
#include "util/IO_utils.h"
#include "util/stacktrace.h"

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
nonmeta_state_base<Tag>::nonmeta_state_base(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const footprint& topfp
#else
		const state_manager& sm
#endif
		) :
		pool() {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const size_t s =
		topfp.template get_instance_pool<Tag>().total_entries() +1;
	// globally 1-indexed, so +1, reserve [0] as a dummy
#else
	const global_entry_pool<Tag>& p(sm.template get_pool<Tag>());
	const size_t s = p.size();
#endif
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& sm,
#endif
		const footprint& topfp) const {
	typedef	class_traits<Tag>		traits_type;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const global_entry_pool<Tag>& ip(sm.template get_pool<Tag>());
#endif
	const size_t s = this->pool.size();
	size_t i = FIRST_VALID_GLOBAL_NODE;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	INVARIANT(i);
#endif
	for ( ; i<s; ++i) {
		const instance_type& nsi(this->pool[i]);
		if (nsi.has_subscribers()) {
			o << traits_type::tag_name << "[" << i << "]: \"";
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			topfp.dump_canonical_name<Tag>(o, i-1);
#else
			const global_entry<Tag>& ge(ip[i]);
			ge.dump_canonical_name(o, topfp, sm);
#endif
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
nonmeta_state_manager::nonmeta_state_manager(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			const footprint& sm
#else
			const state_manager& sm
#endif
			) :
		bool_base_type(sm), 
		int_base_type(sm), 
		enum_base_type(sm), 
		channel_base_type(sm) {
	STACKTRACE_VERBOSE;
	typedef	global_entry_pool<channel_tag>	channel_entry_pool_type;
	typedef	channel_base_type::pool_type	channel_state_pool_type;
	typedef	channel_state_pool_type::iterator	channel_state_iterator;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint& topfp(sm);		// alias
	const state_instance<channel_tag>::pool_type&
		tcp(topfp.get_instance_pool<channel_tag>());
	const size_t s = tcp.total_entries() +1;
#else
	const channel_entry_pool_type&
		cep(sm.get_pool<channel_tag>());
	const size_t s = cep.size();
#endif
	channel_state_iterator i(channel_base_type::pool.begin());
	const channel_state_iterator e(channel_base_type::pool.end());
	const size_t d = distance(i, e);
	STACKTRACE_INDENT_PRINT("d = " << d << ", s = " << s << endl);
	INVARIANT(d == s);
	size_t j = 1;	// 1-indexed, skip NULL entry
	for (++i; i!=e; ++i, ++j) {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_instance<channel_tag>&
			ce(topfp.get_instance<channel_tag>(j-1)); // 0-based
#else
		const global_entry<channel_tag>& ce(cep[j]);
#endif
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
nonmeta_state_manager::dump_struct(ostream& o,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& sm, 
#endif
		const footprint& topfp) const {
	{
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const size_t bools =
			topfp.get_instance_pool<bool_tag>().total_entries() +1;
#else
		const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
		const size_t bools = bool_base_type::pool.size();
#endif
		size_t i = FIRST_VALID_GLOBAL_NODE;
		INVARIANT(i);
		for ( ; i<bools; ++i) {
			o << "bool[" << i << "]: \"";
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			topfp.dump_canonical_name<bool_tag>(o, i-1);
#else
			bp[i].dump_canonical_name(o, topfp, sm);
#endif
			o << "\" ";
			// no static structural information
			// bool_pool[i].dump_struct(o);
			o << endl;
		}
	}{
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const size_t ints =
			topfp.get_instance_pool<int_tag>().total_entries() +1;
#else
		const global_entry_pool<int_tag>& ip(sm.get_pool<int_tag>());
		const size_t ints = int_base_type::pool.size();
#endif
		size_t i = FIRST_VALID_GLOBAL_NODE;
		INVARIANT(i);
		for ( ; i<ints; ++i) {
			o << "int[" << i << "]: \"";
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			topfp.dump_canonical_name<int_tag>(o, i-1);
#else
			ip[i].dump_canonical_name(o, topfp, sm);
#endif
			o << "\" ";
			// no static structural information
			// int_pool[i].dump_struct(o);
			o << endl;
		}
	}{
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const size_t enums =
			topfp.get_instance_pool<enum_tag>().total_entries() +1;
#else
		const global_entry_pool<enum_tag>& ip(sm.get_pool<enum_tag>());
		const size_t enums = enum_base_type::pool.size();
#endif
		size_t i = FIRST_VALID_GLOBAL_NODE;
		INVARIANT(i);
		for ( ; i<enums; ++i) {
			o << "enum[" << i << "]: \"";
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			topfp.dump_canonical_name<enum_tag>(o, i-1);
#else
			ip[i].dump_canonical_name(o, topfp, sm);
#endif
			o << "\" ";
			// no static structural information
			// enum_pool[i].dump_struct(o);
			o << endl;
		}
	}{
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const size_t chans =
			topfp.get_instance_pool<channel_tag>().total_entries() +1;
#else
		const global_entry_pool<channel_tag>&
			cp(sm.get_pool<channel_tag>());
		const size_t chans = channel_base_type::pool.size();
#endif
		size_t i = FIRST_VALID_GLOBAL_NODE;
		INVARIANT(i);
		for ( ; i<chans; ++i) {
			o << "chan[" << i << "]: \"";
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			topfp.dump_canonical_name<channel_tag>(o, i-1);
#else
			cp[i].dump_canonical_name(o, topfp, sm);
#endif
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& sm,
#endif
		const footprint& topfp) const {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	nonmeta_state_base<bool_tag>::__dump_all_subscriptions(o, topfp);
	nonmeta_state_base<int_tag>::__dump_all_subscriptions(o, topfp);
	nonmeta_state_base<enum_tag>::__dump_all_subscriptions(o, topfp);
	nonmeta_state_base<channel_tag>::__dump_all_subscriptions(o, topfp);
#else
	nonmeta_state_base<bool_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<int_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<enum_tag>::__dump_all_subscriptions(o, sm, topfp);
	nonmeta_state_base<channel_tag>::__dump_all_subscriptions(o, sm, topfp);
#endif
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

