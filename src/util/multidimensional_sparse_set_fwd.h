/**
	\file "util/multidimensional_sparse_set_fwd.h"
	Forward declarations for a fixed depth/dimension tree-set.
	$Id: multidimensional_sparse_set_fwd.h,v 1.11 2006/04/18 18:42:43 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include "util/STL/pair_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/size_t.h"

#define MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE			\
template <size_t D, class T, class R, class L>

#define SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE	\
template <class T, class R, class L>

// full description in "multidimensional_sparse_set.h"
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

template <size_t D, class T>
struct default_multidimensional_sparse_set {
	typedef	std::pair<T, T>				default_range_type;

	template <template <class> class L, class R>
	struct rebind_default_list_type {
		typedef L<R>				wrapper_type;
		typedef typename wrapper_type::type	type;
	};

	typedef	typename rebind_default_list_type<std::default_list, 
			default_range_type>::type
							default_list_type;
	typedef	multidimensional_sparse_set<D, T,
			default_range_type, default_list_type>
							type;

	template <size_t D2, class T2>
	struct rebind : public default_multidimensional_sparse_set<D2, T2> { };
};	// end struct default_multidimensional_sparse_set

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

