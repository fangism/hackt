// "list_of_ptr.h"
// useful template for list<T*>
// the alternative is to instantiate list<auto_ptr<T>>...

// NOTE: assumes EXCLUSIVE ownwership of the pointers contained!
// any violation thereof will require something more powerful,
// such as reference counting.  

#ifndef __LIST_OF_PTR_H__
#define __LIST_OF_PTR_H__

#include <list>

using namespace std;

//=============================================================================
/// A list of exclusively owned pointers, based on std::list.  

/**
	The members of this class are pointers to objects of class T, 
	as the name suggests.  The pointers should be exclusive, i.e., 
	whereas other copies of the same pointer may read or write
	the object, only this list has responsibility for de-allocating
	the objects' memory.  This is generally useful for bottom-up 
	constructed objects such as syntax trees.  
 */
template <class T>
class list_of_ptr : public list<T*> {
private:
	// none
public:

/// The default constructor just creates an empty list.  
	list_of_ptr() : list<T*>() { }

/// The destructor frees memory to non-NULL pointers in the list.  
virtual	~list_of_ptr() {
        typename list<T*>::iterator i;
        for (i=begin(); i!=end(); i++) if (*i) delete (*i);
        clear();

// non-essential add-on methods
}


};

#endif	// __LIST_OF_PTR_H__

