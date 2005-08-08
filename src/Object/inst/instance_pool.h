/**
	\file "Object/inst/instance_pool.h"
	Template class wrapper around list_vector.
	$Id: instance_pool.h,v 1.2 2005/08/08 16:51:09 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_POOL_H__
#define	__OBJECT_INST_INSTANCE_POOL_H__

#include "Object/inst/instance_pool_fwd.h"
#include "util/list_vector.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Wrapped interface to list_vector being used as an indexable pool, 
	intended for use of pseudo-allocating instances.  
	Consider adding this as an index_vector_pool to the util library.  
 */
template <class T>
class instance_pool : private util::list_vector<T> {
	typedef	util::list_vector<T>		parent_type;
	typedef	instance_pool<T>		this_type;
public:
	typedef	typename parent_type::const_iterator	const_iterator;
//	typedef	typename parent_type::iterator		iterator;
public:
	explicit
	instance_pool(const size_t);

	~instance_pool();

	using parent_type::size;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::operator[];

	size_t
	allocate(void);

	size_t
	allocate(const T&);

	// there is no deallocate

};	// end class instance_pool

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_POOL_H__

