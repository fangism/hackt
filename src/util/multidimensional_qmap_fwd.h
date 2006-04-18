/**
	\file "util/multidimensional_qmap_fwd.h"
	Forward declarations for a fixed depth/dimension sparse tree.
	$Id: multidimensional_qmap_fwd.h,v 1.10 2006/04/18 18:42:43 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

#include "util/STL/list_fwd.h"
#include "util/size_t.h"

// full description in "multidimensional_qmap.h"
namespace util {
//=============================================================================
// forward declarations
// default to indexing by int, and containing bare char* strings

#define	MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE			\
	template <size_t D, class K, class T, class L>

#define	SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <class K, class T, class L>

MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap;

SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap<1,K,T,L>;

template <size_t D, class K, class T>
struct default_multidimensional_qmap {
	/**
		Note: definition expands ::type::type, so bare STL
		container class templates will not work.
		Use the util library's default_* wrapper policies instead.
		e.g. default_list;
	 */
	template <template <class> class L>
	struct rebind_default_list_type {
		typedef	L<K>				wrapper_type;
		typedef	typename wrapper_type::type	type;
	};

	typedef	typename rebind_default_list_type<std::default_list>::type
							default_list_type;
	typedef	multidimensional_qmap<D, K, T, default_list_type>
							type;

	template <size_t D2, class K2, class T2>
	struct rebind : public default_multidimensional_qmap<D2, K2, T2> { };
};	// end struct default_multidimensional_qmap

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

