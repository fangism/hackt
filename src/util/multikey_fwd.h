/**
	\file "multikey_fwd.h"
	Forward declarations or multidimensional key class.  
	$Id: multikey_fwd.h,v 1.10 2005/01/28 19:58:46 fang Exp $
 */

#ifndef	__MULTIKEY_H_FWD__
#define	__MULTIKEY_H_FWD__

// overrideable namespace
#ifndef	MULTIKEY_NAMESPACE
#define	MULTIKEY_NAMESPACE	util
#endif

#include <iosfwd>

namespace MULTIKEY_NAMESPACE {
using std::ostream;

//=============================================================================
template <class K>
class multikey_base;

template <size_t D, class K = int>
class multikey;

template <class K>
class multikey_generic;

template <class K>
class multikey_generator_base;

template <size_t D, class K>
class multikey_generator;

template <class K>
class multikey_generator_generic;

//-----------------------------------------------------------------------------
template <class K>
ostream&
operator << (ostream& o, const multikey_base<K>& k); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k); 
#endif

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
template <class K>
bool
operator == (const multikey_base<K>& l, const multikey_base<K>& r);

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class K>
bool
operator != (const multikey_base<K>& l, const multikey_base<K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//-----------------------------------------------------------------------------

}	// end namespace MULTIKEY_NAMESPACE

//=============================================================================
#endif	//	__MULTIKEY_H_FWD__

