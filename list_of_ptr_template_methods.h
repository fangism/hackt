// "list_of_ptr_template_methods.h"

// see "art_parser_template_methods.h" for dissertation on why files are
// organized so.  

#ifndef __LIST_OF_PTR_TEMPLATE_METHODS_H__
#define __LIST_OF_PTR_TEMPLATE_METHODS_H__

#include <stdlib.h>
#include "list_of_ptr.h"

//=============================================================================
// debug flags

#define	DEBUG_LIST_OF_PTR		0

#if DEBUG_LIST_OF_PTR
#include <iostream>
#endif

//=============================================================================
// class list_of_ptr method definitions

/// copy constructor with no transfer of ownership semantics
template <class T>
list_of_ptr<T>::list_of_ptr(const list_of_ptr<T>& l) : list<T*>(l), own(false) {
#if DEBUG_LIST_OF_PTR
	cerr << "list_of_ptr<T>::list_of_ptr(const list_of_ptr<T>&);" << endl;
#endif
	// f-cked up patch, copy constructor not always invoked?
	if (size() < l.size()) {
		const_iterator i = l.begin();
		for ( ; i!=l.end(); i++)
			push_back(*i);
	}
#if DEBUG_LIST_OF_PTR
	cerr << "\targument's size() = " << l.size() << endl;
	cerr << "\tthis size() = " << this->size() << endl;
#endif
}

template <class T>
list_of_ptr<T>::~list_of_ptr() {
	clear();
}

template <class T>
void
list_of_ptr<T>::pop_back(void) {
	if (own) {
		iterator e = end();
		if (*e) {
			delete (*e);
			*e = NULL;
		}
	}
	parent::pop_back();
}

template <class T>
void
list_of_ptr<T>::clear(void) {
	if (own) {
		iterator i;
		for (i=begin(); i!=end(); i++) {
			if (*i) {
				delete (*i);
				*i = NULL;
			}
		}
	}
	parent::clear();
}

//=============================================================================
// class list_of_const_ptr method definitions

/// copy constructor
template <class T>
list_of_const_ptr<T>::list_of_const_ptr(const list<T*>& l) :
		parent(l) {
}

/// copy constructor
template <class T>
list_of_const_ptr<T>::list_of_const_ptr(const parent& l) :
		parent(l) {
}

template <class T>
list_of_const_ptr<T>::~list_of_const_ptr() {
}

//=============================================================================

#endif	// __LIST_OF_PTR_TEMPLATE_METHODS_H__

