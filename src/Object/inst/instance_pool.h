/**
	\file "Object/inst/instance_pool.h"
	Template class wrapper around list_vector.
	$Id: instance_pool.h,v 1.10.4.2 2006/03/14 22:16:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_H__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_H__

#include <iosfwd>
#include "Object/inst/instance_pool_fwd.h"
#include "util/list_vector.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/memory/index_pool.h"

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
	using parent_type::allocate;

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

