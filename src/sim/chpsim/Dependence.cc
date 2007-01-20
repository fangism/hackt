/**
	\file "sim/chpsim/Dependence.cc"
	$Id: Dependence.cc,v 1.1.2.5 2007/01/20 07:26:13 fang Exp $
 */

#include "sim/chpsim/Dependence.h"
#include "sim/chpsim/DependenceCollector.h"
#include "Object/nonmeta_context.h"
#include "Object/nonmeta_state.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/STL/valarray_iterator.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::begin;
using std::end;
using std::copy;
using entity::nonmeta_state_base;
using entity::event_subscribers_type;
#include "util/using_ostream.h"
//=============================================================================
// class dependence_set_base method definitions

/**
	Valarray assignment with resizing.  
 */
template <class Tag>
dependence_set_base<Tag>&
dependence_set_base<Tag>::operator = (const this_type& d) {
	this->_set.resize(d._set.size());
	copy(begin(d._set), end(d._set), begin(this->_set));
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
dependence_set_base<Tag>::__import(const dependence_collector_base<Tag>& d) {
	const dependence_index_set_type& r(d._set);
	this->_set.resize(r.size());
	copy(r.begin(), r.end(), begin(this->_set));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Subscribes the event index to the notification list for
	all dependent variables in this set.  
	\param c the run-time state/context object.  
	\param ei the event index to subscribe.
 */
template <class Tag>
void
dependence_set_base<Tag>::__subscribe(const nonmeta_context_base& c, 
		const event_index_type ei) const {
	const node_index_type* i(begin(this->_set)), *e(end(this->_set));
	typename nonmeta_state_base<Tag>::pool_type&
		pool(c.values.template get_pool<Tag>());
	for ( ; i!=e; ++i) {
		pool[*i].subscribe(ei);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unsubscribes the event index from the notification list for
	all dependent variables in this set.  
	\param c the run-time state/context object.  
	\param ei the event index to unsubscribe.
 */
template <class Tag>
void
dependence_set_base<Tag>::__unsubscribe(const nonmeta_context_base& c, 
		const event_index_type ei) const {
	const node_index_type* i(begin(this->_set)), *e(end(this->_set));
	typename nonmeta_state_base<Tag>::pool_type&
		pool(c.values.template get_pool<Tag>());
	for ( ; i!=e; ++i) {
		pool[*i].unsubscribe(ei);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true of any of the variables in this set have the 
		@ei event subscribed to it.  
	Remember an event is subscribed to ALL or NONE of its deps.  
 */
template <class Tag>
bool
dependence_set_base<Tag>::__subscribed_to_any(const nonmeta_state_manager& s, 
		const event_index_type ei) const {
	if (this->_set.size()) {
		const typename nonmeta_state_base<Tag>::pool_type&
			pool(s.template get_pool<Tag>());
		const event_subscribers_type&
			sub(pool[_set[0]].get_subscribers());
		return sub.find(ei) != sub.end();
	} else	return false;
}

//=============================================================================
// class DependenceSet method definitions

DependenceSet::DependenceSet() :
		dependence_set_base<bool_tag>(), 
		dependence_set_base<int_tag>(), 
		dependence_set_base<enum_tag>(), 
		dependence_set_base<channel_tag>() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DependenceSet::~DependenceSet() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
DependenceSet::dump(ostream& o) const {
	std::ostream_iterator<node_index_type> osi(o, " ");
	// local convenience aliases
	const instance_set_type&
		bool_set(dependence_set_base<bool_tag>::_set);
	const instance_set_type&
		int_set(dependence_set_base<int_tag>::_set);
	const instance_set_type&
		enum_set(dependence_set_base<enum_tag>::_set);
	const instance_set_type&
		channel_set(dependence_set_base<channel_tag>::_set);
if (bool_set.size()) {
	o << "bool deps: ";
	copy(begin(bool_set), end(bool_set), osi);
	o << endl;
}
if (int_set.size()) {
	o << "int deps: ";
	copy(begin(int_set), end(int_set), osi);
	o << endl;
}
if (enum_set.size()) {
	o << "enum deps: ";
	copy(begin(enum_set), end(enum_set), osi);
	o << endl;
}
if (channel_set.size()) {
	o << "channel deps: ";
	copy(begin(channel_set), end(channel_set), osi);
	o << endl;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSet::import(const DependenceSetCollector& d) {
	dependence_set_base<bool_tag>::__import(d);
	dependence_set_base<int_tag>::__import(d);
	dependence_set_base<enum_tag>::__import(d);
	dependence_set_base<channel_tag>::__import(d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DependenceSet&
DependenceSet::operator = (const this_type& d) {
	dependence_set_base<bool_tag>::operator = (d);
	dependence_set_base<int_tag>::operator = (d);
	dependence_set_base<enum_tag>::operator = (d);
	dependence_set_base<channel_tag>::operator = (d);
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSet::subscribe(const nonmeta_context_base& c,
		const event_index_type ei) const {
	dependence_set_base<bool_tag>::__subscribe(c, ei);
	dependence_set_base<int_tag>::__subscribe(c, ei);
	dependence_set_base<enum_tag>::__subscribe(c, ei);
	dependence_set_base<channel_tag>::__subscribe(c, ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSet::unsubscribe(const nonmeta_context_base& c,
		const event_index_type ei) const {
	dependence_set_base<bool_tag>::__unsubscribe(c, ei);
	dependence_set_base<int_tag>::__unsubscribe(c, ei);
	dependence_set_base<enum_tag>::__unsubscribe(c, ei);
	dependence_set_base<channel_tag>::__unsubscribe(c, ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	INVARIANT: event is either subscribed to ALL its dependencies or NONE.
	This fact makes the search FAST.  
 */
ostream&
DependenceSet::dump_subscribed_status(ostream& o,
		const nonmeta_state_manager& s, 
		const event_index_type ei) const {
	if (!dependence_set_base<bool_tag>::_set.size() && 
		!dependence_set_base<int_tag>::_set.size() && 
		!dependence_set_base<enum_tag>::_set.size() && 
		!dependence_set_base<channel_tag>::_set.size()) {
		return o << "(no dependencies)";
	} else if (dependence_set_base<bool_tag>::__subscribed_to_any(s, ei) ||
		dependence_set_base<int_tag>::__subscribed_to_any(s, ei) ||
		dependence_set_base<enum_tag>::__subscribed_to_any(s, ei) ||
		dependence_set_base<channel_tag>::__subscribed_to_any(s, ei)) {
		return o << "(blocked and subscribed to its dependencies)";
	} else {
		return o << "(currently not subscribed to its dependencies)";
	}
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

