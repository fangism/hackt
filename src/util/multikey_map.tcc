/**
	\file "util/multikey_map.tcc"
	Template method definitions for multikey_map class.  
	$Id: multikey_map.tcc,v 1.6 2005/05/10 04:51:27 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_TCC__
#define	__UTIL_MULTIKEY_MAP_TCC__

#include <limits>
#include <functional>
#include <algorithm>
#include <iterator>

#include "util/multikey_map.h"
#include "util/multikey.tcc"
#include "util/multikey_assoc.tcc"
#include "util/IO_utils.tcc"

namespace util {
#include "util/using_ostream.h"
using std::istream;
using std::pair;
using std::numeric_limits;
USING_LIST
using util::write_value;
using util::read_value;
using util::write_map;
using util::read_map;
using util::multikey_generator;

//=============================================================================
// class multikey_map method definitions

MULTIKEY_MAP_TEMPLATE_SIGNATURE
multikey_map<D,K,T,M>::multikey_map() : map_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
multikey_map<D,K,T,M>::~multikey_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
T&
multikey_map<D,K,T,M>::operator [] (const list<K>& k) {
	INVARIANT(k.size() == D);
	multikey<D,K> dk(k);
	return map_type::operator[](dk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
T
multikey_map<D,K,T,M>::operator [] (const list<K>& k) const {
	INVARIANT(k.size() == D);
	key_type dk(k);
	return map_type::operator[](dk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<D,K,T,M>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
		o << i->first << " = " << i->second << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<D,K,T,M>::write(ostream& f) const {
	INVARIANT(f.good());
	write_value(f, this->population());
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++) {
		// ok to do this if key contains no pointers
		// and is of known fixed size
		// otherwise, use the partially specializable functors
		// value_writer and value_reader
		write_value(f, i->first);
		write_value(f, i->second);
	}
	return f;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_MAP_TEMPLATE_SIGNATURE
istream&
multikey_map<D,K,T,M>::read(istream& f) {
	INVARIANT(f.good());
	INVARIANT(this->empty());
	size_t size, i=0;
	read_value(f, size);
	for ( ; i<size; i++) {
		key_type key;
		mapped_type val;
		// ok to do this if key contains no pointers
		// and is of known fixed size
		read_value(f, key);
		read_value(f, val);
		(*this)[key] = val;
	}
	return f;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_TCC__

