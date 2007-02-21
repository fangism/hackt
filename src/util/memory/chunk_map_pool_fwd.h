/**
	\file "util/memory/chunk_map_pool_fwd.h"
	Forward declarations for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool_fwd.h,v 1.8 2007/02/21 17:00:28 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

#include "util/size_t.h"
#include "util/STL/construct_fwd.h"

/**
	Declare this in a class to overload its new and delete operators.  
	\param C is the size of the chunks to be allocated.  
 */
#define	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(C)			\
	static void*	operator new (size_t);				\
	static void	operator delete (void*);			\
	static void*	operator new (size_t, void*);			\
	static void	operator delete (void*, void*) { }		\
private:								\
	typedef	util::memory::chunk_map_pool<this_type,C> pool_type;	\
	static	pool_type				pool;

/**
	The robust version is static-initialization-safe as long as
	one uses the REQUIRE macro appropriately.  
 */
#define CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(C)			\
	static void*	operator new (size_t);				\
	static void	operator delete (void*);			\
private:								\
	static void*	operator new (size_t, void*);			\
	static void	operator delete (void*, void*) { }		\
	typedef	util::memory::chunk_map_pool<this_type,C> pool_type;	\
	typedef	util::memory::raw_count_ptr<pool_type>			\
						pool_ref_ref_type;	\
public:									\
	typedef	util::memory::count_ptr<const pool_type> pool_ref_type;	\
private:								\
	static const pool_ref_type __robust_init__;			\
public:									\
	static								\
	pool_ref_ref_type						\
	get_pool(void);

/**
	Friend declarations needed if default constructor is private.
 */
#define	CHUNK_MAP_POOL_ESSENTIAL_FRIENDS(C)				\
	friend class util::memory::chunk_map_pool<this_type,C>;		\
	friend void _Construct<this_type>(this_type* __p);		\
	friend void _Construct<this_type, this_type>(			\
		this_type* __p, const this_type& __value);


namespace util {
namespace memory {


#define	CHUNK_MAP_POOL_TEMPLATE_SIGNATURE				\
template <class T, size_t C, bool Threaded>

template <class T, size_t C, bool Threaded = true>
class chunk_map_pool;

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_FWD_H__

