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

template <class T>
class list_of_ptr : public list<T*> {
private:
	// none
public:
// default constructor
	list_of_ptr() : list<T*>() { }

// destructor -- most critical modification
virtual	~list_of_ptr() {
        typename list<T*>::iterator i;
        for (i=begin(); i!=end(); i++) if (*i) delete (*i);
        clear();

// non-essential add-on methods
}


};

#endif	// __LIST_OF_PTR_H__

