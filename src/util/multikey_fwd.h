/**
	\file "multikey_fwd.h"
	Forward declarations or multidimensional key class.  
	$Id: multikey_fwd.h,v 1.10.2.2 2005/02/17 00:10:20 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_H_FWD__
#define	__UTIL_MULTIKEY_H_FWD__

#include <iosfwd>

namespace util {
using std::ostream;

//=============================================================================

template <size_t D, class K = int>
class multikey;

template <class K>
class multikey_generic;

template <size_t D, class K>
class multikey_generator;

template <class K>
class multikey_generator_generic;

//-----------------------------------------------------------------------------
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k); 

template <class K>
ostream&
operator << (ostream& o, const multikey_generic<K>& k); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator > (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator == (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator >= (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const multikey<D,K>& l, const multikey<D,K>& r);

//-----------------------------------------------------------------------------
}	// end namespace util

//=============================================================================
#endif	// __UTIL_MULTIKEY_H_FWD__

