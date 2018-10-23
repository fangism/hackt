/**
	\file "util/multidimensional_sparse_set_fwd.hh"
	Forward declarations for a fixed depth/dimension tree-set.
	$Id: multidimensional_sparse_set_fwd.hh,v 1.11 2006/04/18 18:42:43 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include <list>
#include "util/STL/pair_fwd.hh"
#include "util/size_t.h"

#define MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE			\
template <size_t D, class T, class R, class L>

#define SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE	\
template <class T, class R, class L>

// full description in "multidimensional_sparse_set.hh"
namespace util {
//=============================================================================
// forward declarations

// should param R be a template <class, class>?
// or just any class that satisfies the pair interface, 
// not necessarily template?

MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
class multidimensional_sparse_set;

SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
class multidimensional_sparse_set<1,T,R,L>;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

