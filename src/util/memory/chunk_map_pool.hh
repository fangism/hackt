/**
	\file "util/memory/chunk_map_pool.hh"
	Class definition for chunk-allocated mapped memory pool template.  
	$Id: chunk_map_pool.hh,v 1.17 2010/04/05 00:18:47 fang Exp $
 */

#ifndef	__UTIL_MEMORY_CHUNK_MAP_POOL_H__
#define	__UTIL_MEMORY_CHUNK_MAP_POOL_H__

#include <iosfwd>
#include <set>
#include <list>
#include <map>
#include "util/memory/chunk_map_pool_fwd.hh"
#include "util/numeric/inttype_traits.hh"
#include "util/macros.h"
#include "util/memory/fixed_pool_chunk.hh"
#include "util/cppcat.h"

//=============================================================================
/**
	Define the class-local static pool.
	\param TemplSpec optional template specification.
	\param _typename the typename keyword if needed.
	\param T the name of the pool-allocated class. 
 */
#define	__CHUNK_MAP_POOL_DEFAULT_STATIC_INIT(T)				\
T::pool_type T::pool;
#define	__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_STATIC_INIT(TemplSpec, T)	\
TemplSpec								\
typename T::pool_type T::pool;

/**
	General template macro for defining the new operator 
	for a (default) chunk_map_pool-allocated class.  
	\param TemplSpec optional template specification.
	\param T the name of the pool-allocated class. 
	NOTE: T may be a simple template-id without commas, due to
		proprocessor limitations.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(T)			\
void*									\
T::operator new (size_t s) {						\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}
#define	__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_OPERATOR_NEW(TemplSpec, T)	\
TemplSpec								\
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(T)

/**
	Template macro for defining the placement new operator
	for a (default) chunk_map_pool-allocated class.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(T)		\
void*									\
T::operator new (size_t s, void* p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p); return p;					\
}
#define	__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_OPERATOR_PLACEMENT_NEW(TemplSpec, T) \
TemplSpec								\
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(T)

/**
	Template macro for defining the delete operator
	for a (default) chunk_map_pool-allocated class.  
 */
#define	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(T)			\
void									\
T::operator delete (void* p) {						\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}
#define	__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_OPERATOR_DELETE(TemplSpec, T)	\
TemplSpec								\
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(T)

/**
	Template macro for defining class member methods for 
	chunk_map_pool-allocating that class.  
	Use this macro to conveniently define all the necessary
	class member functions for chunk_map_pool-allocating.  
	Normal definition of new and delete using chunk_map_pool as 
	the underlying allocator for a class.  
 */
#define	CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(T)			\
	__CHUNK_MAP_POOL_DEFAULT_STATIC_INIT(T)				\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(T)			\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(T)		\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(T)
/**
	Template version of the class member definition macro. 
 */
#define	TEMPLATE_CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(TemplSpec, T) \
	__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_STATIC_INIT(TemplSpec, T)	\
	TemplSpec							\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(T)			\
	TemplSpec							\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(T)		\
	TemplSpec							\
	__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(T)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Macro for explicitly requiring orderly memory pool static 
	initialization.  
	\param T any type that is not a template-id, damn preprocessor.
	Workaround: use preprocessor definition as substitute.  
 */
#define	__REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)			\
static const T::pool_ref_type						\
UNIQUIFY(__pool_ref__) (T::get_pool());
#define	__REQUIRES_CHUNK_MAP_POOL_TEMPLATE_STATIC_INIT(T)		\
static const typename T::pool_ref_type					\
UNIQUIFY(__pool_ref__) (T::get_pool());

/**
	This is the class-static initializer.  
 */
#define	__SELF_CHUNK_MAP_POOL_STATIC_INIT(T)				\
const T::pool_ref_type							\
T::__robust_init__ (T::get_pool());
#define	__SELF_CHUNK_MAP_POOL_TEMPLATE_STATIC_INIT(TemplSpec, T)	\
TemplSpec								\
const typename T::pool_ref_type						\
T::__robust_init__ (T::get_pool());

/**
	Declares and initializes an external anchor for properly
	initializing a dependent pool.  
	Use this macro for non-template types.  
 */
#define	REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)				\
	__REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)

/**
	Declares and initializes an external anchor for properly
	initializing a dependent pool.  
	Use this macro for template types.  
 */
#define	TEMPLATE_REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(T)			\
	__REQUIRES_CHUNK_MAP_POOL_TEMPLATE_STATIC_INIT(T)

/**
	Template macro for defining class static method get_pool().
 */
#define	__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(T)			\
T::pool_ref_ref_type							\
T::get_pool(void) {							\
	static pool_type*	pool = new pool_type();			\
	STATIC_RC_POOL_REF_INIT;					\
	static size_t*		count = NEW_SIZE_T;			\
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);	\
	return pool_ref_ref_type(pool, count);				\
}
#define	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_STATIC_GET_POOL(TemplSpec, T)	\
TemplSpec								\
typename __CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(T)

/**
	Template macro for defining class operator new, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(T)				\
void*									\
T::operator new (size_t s) {						\
	static pool_type& pool(*get_pool());				\
	INVARIANT(sizeof(T) == s);					\
	return pool.allocate();						\
}	/* implicitly calls default ctor thereafter */
#define	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_NEW(TemplSpec, T)	\
TemplSpec								\
__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(T)

/**
	Template macro for defining class operator placement new, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(T)		\
void*									\
T::operator new (size_t s, void* p) {					\
	INVARIANT(sizeof(T) == s);					\
	NEVER_NULL(p);							\
	return p;							\
}	/* automatically calls copy-ctor thereafter */
#define	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_PLACEMENT_NEW(TemplSpec, T) \
TemplSpec								\
__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(T)

/**
	Template macro for defining class operator delete, 
	for chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(T)			\
void									\
T::operator delete (void* p) {						\
	static pool_type& pool(*get_pool());				\
	T* t = reinterpret_cast<T*>(p);					\
	NEVER_NULL(t);							\
	pool.deallocate(t);						\
}	/* implicitly calls dtor thereafter */
#define	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_DELETE(TemplSpec, T)	\
TemplSpec								\
__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(T)

/**
	Bundled template macro for declaring all necessary member functions
	for (robust) chunk_map_pool allocation.  
 */
#define	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(T) 			\
	__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(T) 			\
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(T)				\
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(T)		\
	__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(T)
#define	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_STATIC_DEFINITION(TemplSpec, T) \
	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_STATIC_GET_POOL(TemplSpec, T)	\
	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_NEW(TemplSpec, T)	\
	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_PLACEMENT_NEW(TemplSpec, T) \
	__CHUNK_MAP_POOL_TEMPLATE_ROBUST_OPERATOR_DELETE(TemplSpec, T)

/**
	Initialization-ordering-safe version fo chunk_map_pool-enabled
	overloads of new and delete.  
 */
#define	CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(T)			\
	__SELF_CHUNK_MAP_POOL_STATIC_INIT(T)				\
	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(T)

/**
	Template version of same definitions.  
 */
#define	TEMPLATE_CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(TemplSpec, T)	\
	__SELF_CHUNK_MAP_POOL_STATIC_INIT(TemplSpec, T)			\
	__CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(TemplSpec, T)


//=============================================================================
namespace util {
namespace memory {
using std::ostream;

//=============================================================================
// specialization
template <size_t C, bool Threaded>
class chunk_map_pool<void, C, Threaded> {
public:
	typedef	size_t		size_type;
	typedef	ptrdiff_t	difference_type;
	typedef	void*		pointer;
	typedef	const void*	const_pointer;
	typedef	void		value_type;
// no rebind yet...
};

//=============================================================================
/**
	Chunky allocator.  
	This allocator keeps tracks of memory in a map of chunks.  
	Allocation is performed one chunk at a time.  
	Each chunk maintains a free_mask.  
	When a chunk becomes "empty" its meory is eagerly reclaimed.  

	Tradeoff:
	The larger the chunk the fewer calls to the underlying
	allocator methods (allocate, deallocate), but the coarse-grain
	the memory fragmentation.  
 */
CHUNK_MAP_POOL_TEMPLATE_SIGNATURE
class chunk_map_pool {
	typedef	CHUNK_MAP_POOL_CLASS			this_type;
public:
	typedef	T					value_type;
	typedef	size_t					size_type;
	typedef	ptrdiff_t				difference_type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	/**
		Rebind allocator to another type.  
	 */
	template <class S>
	struct rebind {	typedef	chunk_map_pool<S,C>	other; };
protected:
	// or use typeless_memory_chunk?
	typedef	fixed_pool_chunk<T,C>		chunk_type;
	// possibly be able to pass in underlying allocator to chunk_set
	typedef	std::list<chunk_type>			chunk_set_type;
	typedef	typename chunk_set_type::iterator	chunk_set_iterator;
	typedef	std::map<const void*, chunk_set_iterator>
							alloc_map_type;
	typedef	typename alloc_map_type::iterator	alloc_map_iterator;
	typedef	typename alloc_map_type::const_iterator
						const_alloc_map_iterator;
	typedef	std::set<chunk_type*>			avail_set_type;
	typedef	typename avail_set_type::iterator	avail_set_iterator;
protected:
	/**
		The home of the memory chunks.  
	 */
	chunk_set_type					chunk_set;
	/**
		The map to all allocated chunks, indexed by address of value.  
	 */
	alloc_map_type					chunk_map;
	/**
		Set of chunks with any free entries.  
	 */
	avail_set_type					avail_set;
public:
	chunk_map_pool();

	~chunk_map_pool();

	bool
	contains(pointer p) const;

	pointer
	allocate(void) __ATTRIBUTE_MALLOC__ ;

#if 0
	// unimplemented
	pointer
	allocate(size_type n,
		chunk_map_pool<void,C,Threaded>::const_pointer hint = 0)
		__ATTRIBUTE_MALLOC__ ;
#endif

	void
	deallocate(pointer);

#if 0
	// unimplemented
	void
	deallocate(pointer p, size_type n);
#endif

	// placement constructor
	void
	construct(pointer __p, const_reference __val) const {
		::new(__p) value_type(__val);
	}

	// placement destructor
	void
	destroy(pointer __p) {
		__p->~value_type();
	}


	size_type
	max_size(void) const;

	ostream&
	status(ostream& o) const;

private:
	// non-copyable
	explicit
	chunk_map_pool(const this_type&);

	// non-assignable
	this_type&
	operator = (const this_type&);

};	// end class chunk_map_pool

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_CHUNK_MAP_POOL_H__

