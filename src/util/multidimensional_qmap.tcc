/**
	\file "multidimensional_qmap.tcc"
	Template method definitions for multidimensional_qmap class.
	$Id: multidimensional_qmap.tcc,v 1.1.24.2 2005/02/07 06:02:32 fang Exp $
 */

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_TCC__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_TCC__

#include <iostream>
#include <numeric>		// for accumulate

#include "sstream.h"
#include "multidimensional_qmap.h"

#include "qmap.tcc"

namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
using std::accumulate;
#include "using_ostream.h"

//=============================================================================
// class multidimensional_qmap method definitions

MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<D,K,T,L>::multidimensional_qmap() : index_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<D,K,T,L>::multidimensional_qmap(const this_type& s) :
		index_map(s.index_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<D,K,T,L>::~multidimensional_qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
void
multidimensional_qmap<D,K,T,L>::clean(void) {
#if 0
	for_each(index_map.begin(), index_map.end(), 
		unary_compose(
			mem_fun_ref(&child_type::clean), 
			_Select2nd<typename map_type::value_type>()
		)
	);
	remove_if(index_map.begin(), index_map.end(), 
		unary_compose(
			mem_fun_ref(&child_type::empty), 
			_Select2nd<typename map_type::value_type>()
		)
	);
#elif 1
	map_iterator i = index_map.begin();
	const const_map_iterator e = index_map.end();
	for ( ; i!=e; ) {
		i->second.clean();
		if (i->second.empty()) {
			map_iterator j = i;
			j++;
			index_map.erase(i);
			i = j;
		} else {
			i++;
		}
	}
#else
	for_each(index_map.begin(), index_map.end(),
		MULTIDIMENSIONAL_QMAP_NAMESPACE::clean<D,K,T,L>);
	// problems with assignment = of pairs with const first
	remove_if(index_map.begin(), index_map.end(),
		MULTIDIMENSIONAL_QMAP_NAMESPACE::empty<D,K,T,L>);
#endif
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
void
multidimensional_qmap<D,K,T,L>::clear(void) {
	index_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
bool
multidimensional_qmap<D,K,T,L>::erase(const index_arg_type& l) {
	const_list_iterator h = l.first;
	const_list_iterator t = l.second;
	map_iterator probe = index_map.find(*h);
	if (probe == index_map.end()) {
		return false;
	} else if (h == t) {
		// erase entire subrange
		index_map.erase(probe);
		return true;
	} else {
		index_arg_type sub(h, t);
		sub.first++;
		return probe->second.erase(sub);
	}
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
typename multidimensional_qmap<D,K,T,L>::size_type
multidimensional_qmap<D,K,T,L>::population(void) const {
	return accumulate(index_map.begin(), index_map.end(), 0,
		MULTIDIMENSIONAL_QMAP_NAMESPACE::population<D,K,T,L>);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
T&
multidimensional_qmap<D,K,T,L>::operator [] (const index_arg_type& i) {
	INVARIANT(i.first != i.second);
	K k = *(i.first);
	index_arg_type sub(i.first, i.second);
	sub.first++;
	return index_map[k][sub];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
T
multidimensional_qmap<D,K,T,L>::operator [] (const index_arg_type& i) const {
	INVARIANT(i.first != i.second);
	K k = *(i.first);
	index_arg_type sub(i.first, i.second);
	sub.first++;
	return AS_A(const map_type&, index_map)[k][sub];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
ostream&
multidimensional_qmap<D,K,T,L>::dump(ostream& o, const string& pre) const {
	const_map_iterator i = index_map.begin();
	const const_map_iterator e = index_map.end();
	for ( ; i!=e; i++) {
		ostringstream p;
		p << pre << '[' << i->first << ']';
		i->second.dump(o, p.str());
	}
	return o;
}

//=============================================================================
// class multidimensional_qmap method definitions (specialized)

SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<1,K,T,L>::multidimensional_qmap() : index_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<1,K,T,L>::multidimensional_qmap(const this_type& s) :
		index_map(s.index_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
multidimensional_qmap<1,K,T,L>::~multidimensional_qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
bool
multidimensional_qmap<1,K,T,L>::erase(const K i) {
	map_iterator probe = index_map.find(i);
	if (probe != index_map.end()) {
		index_map.erase(probe);
		return true;
	} else  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
bool
multidimensional_qmap<1,K,T,L>::erase(const index_arg_type& l) {
	INVARIANT(l.first == l.second);
	return erase(*(l.first));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
ostream&
multidimensional_qmap<1,K,T,L>::dump(ostream& o, const string& pre) const {
	const_map_iterator i = index_map.begin();
	const const_map_iterator e = index_map.end();
	for ( ; i!=e; i++) {
		o << pre << '[' << i->first << "] = "
			<< i->second << endl;
	}
	return o;
}

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_QMAP_NAMESPACE

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_TCC__

