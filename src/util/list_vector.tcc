/**
	file "list_vector.tcc"
	Template method definitions for list_vector class.  

	$Id: list_vector.tcc,v 1.3 2004/12/03 02:46:42 fang Exp $
 */

#ifndef	__LIST_VECTOR_TCC__
#define	__LIST_VECTOR_TCC__

#include <iostream>
#include <numeric>
#include "list_vector.h"

// should we include this automatically?
#include "qmap.tcc"

namespace util {
using namespace std;

//=============================================================================
// class list_vector method definitions


#if 0
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_TEMPLATE_SIGNATURE
list_vector<T,ValAlloc,VecAlloc>::iterator
list_vector<T,ValAlloc,VecAlloc>::insert(iterator pos, const value_type& v) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_TEMPLATE_SIGNATURE
void
list_vector<T,ValAlloc,VecAlloc>::insert(iterator pos, const size_type n, 
		const value_type& v) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_TEMPLATE_SIGNATURE
template <class InIter>
void
list_vector<T,ValAlloc,VecAlloc>::insert(iterator pos, InIter first, InIter last) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_TEMPLATE_SIGNATURE
list_vector<T,ValAlloc,VecAlloc>::iterator
list_vector<T,ValAlloc,VecAlloc>::erase(iterator first, iterator last) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_TEMPLATE_SIGNATURE
list_vector<T,ValAlloc,VecAlloc>::iterator
list_vector<T,ValAlloc,VecAlloc>::erase(iterator pos) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
LIST_VECTOR_TEMPLATE_SIGNATURE
inline
list_vector<T,ValAlloc,VecAlloc>::list_map_checker::list_map_checker(
		const size_type l, const size_type m) :
		list_size(l), map_size(m) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Debugging print.  Not maintained.  
 */
LIST_VECTOR_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o,
	const typename list_vector<T,ValAlloc,VecAlloc>::list_map_checker& l) {
	return o << '(' << l.list_size << ',' << l.map_size << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicitly checks the invariants maintained by the list_vector class. 
	If you think an object of this type is corrupt, call this to confirm.  
 */
LIST_VECTOR_TEMPLATE_SIGNATURE
void
list_vector<T,ValAlloc,VecAlloc>::check_invariants(void) const {
	const size_type this_size = this->size();
	assert(vec_list.size() >= 2);
	const size_type vdiff = vec_list.size() -vec_map.size() -2;
	const difference_type dist = distance(this->begin(), this->end());
	const difference_type rdist = distance(this->rbegin(), this->rend());
#if 1
	assert(dist == rdist);
	if (dist != difference_type(this_size)) {
		if (this_size) {
			cerr << "begin() at " << &*this->begin() <<
				" with value " << *this->begin() <<
				", --end() at " << &*(--this->end()) <<
				" with value " << *(--this->end()) << endl;
			cerr << "rbegin() at " << &*this->rbegin() <<
				" with value " << *this->rbegin() <<
				", --rend() at " << &*(--this->rend()) <<
				" with value " << *(--this->rend()) << endl;
			cerr << "front() at " << &this->front() <<
				" with value " << this->front() <<
				", back() at " << &this->back() <<
				" with value " << this->back() << endl;
		}
		cerr << "distance = " << dist <<
			", but size = " << this_size << endl;
	}
#endif
	assert(dist == difference_type(this_size));
	assert(!vdiff);
	if (vec_list_front()->empty()) {
		assert(vec_list.size() <= 3);
		assert(vec_list_back()->empty());
		assert(!this_size);
		assert(this->begin() == this->end());
		assert(this->rbegin() == this->rend());
	} else {
		assert(&this->front() == &*(this->begin()));
		assert(&this->back() == &*(--this->end()));
		assert(&this->front() == &*(--this->rend()));
		assert(&this->back() == &*(this->rbegin()));
		assert(!vec_map.empty());
		assert(vec_map.begin()->first == 0);
		// KEY INVARIANT:
		// the N+1'th map entry key == cumulative size of N chunks
		list_map_checker val(0,0);
		val = inner_product(++vec_map.begin(), vec_map.end(), 
			vec_list_front(), val, 
			ptr_fun(&list_map_checker::add), 
			ptr_fun(&list_map_checker::mul));
		// stops short of the sentinel in the vec_list
		assert(val.valid());
	}
	// check reference integrity and consistency
	size_t i = 0;
	const_iterator iter = this->begin();
	for ( ; i < this_size; i++, iter++) {
		assert(&this->at(i) == &*iter);
	}
	const_reverse_iterator riter = this->rbegin();
	for (i=1 ; i <= this_size; i++, riter++) {
		assert(&this->at(this_size -i) == &*riter);
	}
	// not necessarily true:
//	assert(vec_map.upper_bound(this_size) == vec_map.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the curious, this dumps internal representation
	of the list and map of vector chunks maintained.  
	\param o the output stream.
	\param o the same output stream.  
 */
LIST_VECTOR_TEMPLATE_SIGNATURE
ostream&
list_vector<T,ValAlloc,VecAlloc>::dump_details(ostream& o) const {
	o << "In list_vector<T,ValAlloc> at " << this << ": {" << endl;
//	o << "\tlist<vector<T> >, map<size_t,vector<T*>: {" << endl;
	o << "\tvec_list_front at " << &*vec_list_front() << 
		", vec_list_back at " << &*vec_list_back() << endl;
	o << "\tvec_list_rfront at " << &*vec_list_rfront() << 
		", vec_list_rback at " << &*vec_list_rback() << endl;
	// start map one-ahead, skip the last vector chunk, 
	// because it may not be full.
	assert(vec_list.size() == vec_map.size() +2);
	const_vec_map_iterator mi = vec_map.begin();
	const_list_iterator li = vec_list.begin();
	const const_list_iterator le = --vec_list.end();
//	const const_vec_map_iterator me = vec_map.end();
	// the head sentinel
	o << "\tat " << &*li << "\t" <<
		li->size() << "/" << li->capacity() << endl;
	for (li++ ; li != le; li++, mi++) {
		o << "\tat " << &*li << "\t" <<
			li->size() << "/" << li->capacity() <<
			"\t" << mi->first << "\t" << mi->second << endl;
	}
	// and the end sentinel
	o << "\tat " << &*li << "\t" <<
		li->size() << "/" << li->capacity() << endl;
//	o << "\t}" << endl;
	return o << "}" << endl;
}

//=============================================================================
}	// end namespace util

#endif	// __LIST_VECTOR_TCC__

