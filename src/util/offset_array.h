/**
	\file "util/offset_array.h"
	$Id: offset_array.h,v 1.2 2006/01/22 06:53:35 fang Exp $
 */

#ifndef	__UTIL_OFFSET_ARRAY_H__
#define	__UTIL_OFFSET_ARRAY_H__

namespace util {

/**
	A wrapper for automatically adjusting the offset of 
	an array indexed reference.  
	We use inheritance to grant access to all of the base
	class's public methods.  
	\param A the underlying array type, such as vector, list_vector.
	\param K the offset by which an index should be subtracted.  
 */
template <class A, int K>
class offset_array : public A {
public:
	typedef	A					parent_type;
	typedef	typename parent_type::size_type		size_type;
	typedef	typename parent_type::reference		reference;
	typedef	typename parent_type::const_reference	const_reference;
	enum {
		offset = K
	};

	reference
	operator [] (const size_type i) {
		return parent_type::operator[](i -K);
	}

	const_reference
	operator [] (const size_type i) const {
		return parent_type::operator[](i -K);
	}

};	// end class offset_array


}	// end namespace util

#endif	// __UTIL_OFFSET_ARRAY_H__

