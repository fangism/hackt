/**
	\file "multikey_fwd.h"
	Forward declarations or multidimensional key class.  
	$Id: multikey_fwd.h,v 1.10.10.1 2005/02/06 16:23:44 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_H_FWD__
#define	__UTIL_MULTIKEY_H_FWD__

// overrideable namespace
#ifndef	MULTIKEY_NAMESPACE
#define	MULTIKEY_NAMESPACE	util
#endif

#ifndef	WANT_MULTIKEY_BASE
#define	WANT_MULTIKEY_BASE	0
#endif

#include <iosfwd>

namespace MULTIKEY_NAMESPACE {
using std::ostream;

//=============================================================================
#if WANT_MULTIKEY_BASE
template <class K>
class multikey_base;
#endif

template <size_t D, class K = int>
class multikey;

template <class K>
class multikey_generic;

#if WANT_MULTIKEY_BASE
template <class K>
class multikey_generator_base;
#endif

template <size_t D, class K>
class multikey_generator;

template <class K>
class multikey_generator_generic;

//-----------------------------------------------------------------------------
#if WANT_MULTIKEY_BASE
template <class K>
ostream&
operator << (ostream& o, const multikey_base<K>& k); 
#else
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k); 

template <class K>
ostream&
operator << (ostream& o, const multikey_generic<K>& k); 
#endif

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
#if WANT_MULTIKEY_BASE
template <class K>
bool
operator == (const multikey_base<K>& l, const multikey_base<K>& r);
#endif

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
#if WANT_MULTIKEY_BASE
template <class K>
bool
operator != (const multikey_base<K>& l, const multikey_base<K>& r);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//-----------------------------------------------------------------------------

}	// end namespace MULTIKEY_NAMESPACE

//=============================================================================
#endif	// __UTIL_MULTIKEY_H_FWD__

