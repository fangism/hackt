/**
	\file "sim/chpsim/Dependence.cc"
	$Id: Dependence.cc,v 1.1.2.4 2007/01/14 23:36:22 fang Exp $
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

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

