/**
	\file "Object/inst/instance_pool.h"
	Template class wrapper around list_vector.
	$Id: instance_pool.h,v 1.3 2005/09/04 21:14:50 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_POOL_H__
#define	__OBJECT_INST_INSTANCE_POOL_H__

#include <iosfwd>
#include "Object/inst/instance_pool_fwd.h"
#include "util/list_vector.h"
#include "util/persistent_fwd.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;
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
	typedef	typename T::tag_type		tag_type;
public:
	typedef	typename parent_type::const_iterator	const_iterator;
//	typedef	typename parent_type::iterator		iterator;
private:
	/**
		Default chunk size when not specified.  
	 */
	enum	{ default_chunk_size = 32 };
public:
	// custom default constructor
	instance_pool();

	explicit
	instance_pool(const size_t);

	// copy-construction policy
	instance_pool(const this_type&);

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
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_POOL_H__

