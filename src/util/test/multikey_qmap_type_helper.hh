/**
	\file "util/test/multikey_qmap_type_helper.hh"
	Helper template typedefs, just for testing purposes. 
	$Id: multikey_qmap_type_helper.hh,v 1.1 2006/04/18 18:42:48 fang Exp $
 */

#ifndef	__UTIL_TEST_MULTIKEY_QMAP_TYPE_HELPER_H__
#define	__UTIL_TEST_MULTIKEY_QMAP_TYPE_HELPER_H__

#include <string>
#include "util/multikey_map_fwd.hh"

namespace testing {

template <size_t D>
struct __helper_mk_map_type {
	typedef util::default_multikey_map<D, int, std::string>	__helper_type;
	typedef typename __helper_type::
		template rebind_default_map_type<util::default_qmap>::type
							__qmap_type;
	typedef util::multikey_map<D, int, std::string, __qmap_type>	type;
};

}	// end namespace testing

#endif	// __UTIL_TEST_MULTIKEY_QMAP_TYPE_HELPER_H__

