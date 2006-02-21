/**
	\file "Object/inst/instance_pool.h"
	Template class wrapper around list_vector.
	$Id: instance_pool.h,v 1.10 2006/02/21 04:48:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_H__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_H__

#include <iosfwd>
#include "Object/inst/instance_pool_fwd.h"
#include "util/list_vector.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/memory/index_pool.h"
#include "Object/devel_switches.h"


#if	INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
#include <queue>	// for std::priority_queue
#include "util/STL/queue_fwd.h"
#include <map>
#endif

namespace HAC {
namespace entity {
class footprint;
using std::istream;
using std::ostream;
using std::vector;
using util::good_bool;
using util::persistent_object_manager;
using util::memory::index_pool;

//=============================================================================
/**
	Wrapped interface to list_vector being used as an indexable pool, 
	intended for use of pseudo-allocating instances.  
	Consider adding this as an index_vector_pool to the util library.  

	(20060126: fangism)
		Due to a critical ICE in the create_unique index allocation, 
	we need to support deallocation for back-patching a botched
	algorithm.  (This is NOT the way I prefer to handle it, but
	delivery time is critical.)
	TODO: Go back and revisit the algorithm.  
 */
template <class T>
class instance_pool : private index_pool<util::list_vector<T> > {
	typedef	index_pool<util::list_vector<T> >	parent_type;
	typedef	instance_pool<T>		this_type;
	typedef	typename T::tag_type		tag_type;
public:
	typedef	typename T::traits_type			traits_type;
	typedef	typename parent_type::const_iterator	const_iterator;
	typedef	typename parent_type::size_type		size_type;
	typedef	typename parent_type::value_type	value_type;
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	typedef	typename parent_type::iterator		iterator;
private:
	/**
		Reverse sorting: lower numbers have higher priority.  
	 */
	typedef	typename std::default_priority_queue<size_type>::reverse_type
							free_list_type;
//	typedef	std::priority_queue<size_type>		free_list_type;
	free_list_type					free_list;

	typedef	std::map<size_type, size_type>		index_remap_type;
	index_remap_type				remap;
#endif
private:
	/**
		Default chunk size when not specified.  
	 */
	enum	{ default_chunk_size = 32 };
public:
	// custom default constructor
	instance_pool();

	explicit
	instance_pool(const size_type);

	// copy-construction policy
	instance_pool(const this_type&);

	~instance_pool();

	using parent_type::size;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::operator[];

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	size_type
	allocate(void);

	size_type
	allocate(const value_type&);

	void
	deallocate(const size_type);

	// dirty, dirty hack :( called by definition::create_complete...
	void
	compact(void);

	// dirty hack accomplice
	void
	truncate(void);

	size_t
	translate_remap(const size_t) const;
private:
	// as long as underlying type support this operation
	using parent_type::array_type::pop_back;

public:
#else
	using parent_type::allocate;
#endif

	// there is no deallocate

	ostream&
	dump(ostream&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class instance_pool

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_POOL_H__

