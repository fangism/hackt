// "map_of_ptr_template_methods.h"

// see "art_parser_template_methods.h" for dissertation on why files are
// organized so.  

#ifndef __MAP_OF_PTR_TEMPLATE_METHODS_H__
#define __MAP_OF_PTR_TEMPLATE_METHODS_H__

#include "map_of_ptr.h"

/// copy constructor with no transfer of ownership semantics
template <class K, class T>
map_of_ptr<K,T>::map_of_ptr(const map_of_ptr<K,T>& l) : map<K,T*>(l), own(0) {
}

template <class K, class T>
map_of_ptr<K,T>::~map_of_ptr() {
	if (own) {
		iterator i;
		for (i=begin(); i!=end(); i++)
			if ((*i).second)
				delete (*i).second;
	}
	clear();		// may not be necessary
}

#endif	// __MAP_OF_PTR_TEMPLATE_METHODS_H__

