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

//-----------------------------------------------------------------------------
template <class T>
list_of_ptr<T>::~list_of_ptr() {
	clear();
}

//-----------------------------------------------------------------------------
template <class T>
void
list_of_ptr<T>::pop_back(void) {
	if (own) {
		reverse_iterator e = rbegin();
		if (*e) {
			delete (*e);
			*e = NULL;
		}
	}
	parent::pop_back();
}

//-----------------------------------------------------------------------------
template <class T>
void
list_of_ptr<T>::pop_front(void) {
	if (own) {
		iterator e = begin();
		if (*e) {
			delete (*e);
			*e = NULL;
		}
	}
	parent::pop_front();
}

//-----------------------------------------------------------------------------
/**
	De-allocates memory pointed to by the pointer elements if
	ownership flag is true.  
 */
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

//-----------------------------------------------------------------------------
/**
	Releases responsibility for freeing list objects from
	this list, and transfers ownership to the destination list.  
	Also clears this list.  
	\param dest the destination list to which elements are transferred.  
		The destination list MUST be an owner, else the elements
		will never be de-allocated and memory will leak.  
 */
template <class T>
void
list_of_ptr<T>::release_append(list_of_ptr<T>& dest) {
	iterator i;
	for (i=begin(); i!=end(); i++)
		dest.push_back(*i);
	assert(dest.own);
	own = false;
	clear();
}

//=============================================================================
// class list_of_const_ptr method definitions

/**
	Copy constructor, from a list of non-const pointers.  
 */
template <class T>
list_of_const_ptr<T>::list_of_const_ptr(const list<T*>& l) :
		parent() {
	typename list<T*>::const_iterator i = l.begin();
	for ( ; i!=l.end(); i++)
		push_back(*i);
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

