// "list_of_ptr_template_methods.h"

// see "art_parser_template_methods.h" for dissertation on why files are
// organized so.  

#ifndef __LIST_OF_PTR_TEMPLATE_METHODS_H__
#define __LIST_OF_PTR_TEMPLATE_METHODS_H__

#include "list_of_ptr.h"

/// copy constructor with no transfer of ownership semantics
template <class T>
list_of_ptr<T>::list_of_ptr(const list_of_ptr<T>& l) : list<T*>(l), own(0) {
}

template <class T>
list_of_ptr<T>::~list_of_ptr() {
	if (own) {
		iterator i;
		for (i=begin(); i!=end(); i++)
			if (*i) delete (*i);
	}
	clear();		// may not be necessary
}

#endif	// __LIST_OF_PTR_TEMPLATE_METHODS_H__

