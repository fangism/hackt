/**
	\file "util/tagged_subtype.h"
	Tool for tagging types with additional type-information
	as a form of subtyping.  
	$Id: tagged_subtype.h,v 1.1 2006/07/13 22:21:40 fang Exp $
 */

#ifndef	__UTIL_TAGGED_SUBTYPE_H__
#define	__UTIL_TAGGED_SUBTYPE_H__

namespace util {

/**
	Class using template sugar to distinguish types by tag.  
	Useful to give additional semantic typing and meaning to 
	built-in types such as int.  Type-enforced documentation.  
	The additional hassle is in accessing the value as a member.  

	\param T the underlying base type to wrap around (via containership).  
		Note that this is NOT a subtype of T.  
		Most often, T will be some fundamental type.  
	\param Tag is not used internally, its sole purpose is
		type distinction.  

	Example use:
	struct pool_index_tag {};
	typedef	tagged_subtype<size_t, pool_index_tag>	pool_index_type;
	Now pool_index_type cannot be implicitly converted from other size_t's.
 */
template <typename T, typename Tag>
struct tagged_subtype {
public:
	typedef	T			value_type;
	/// the underlying value
	value_type			value;

	/**
		Could do: pass by call_traits.  
	 */
	explicit
	tagged_subtype(const value_type& v) : value(v) { }

	// default constructor
	// default copy-constructor
	// default destructor

	// no implicit conversion to T provided, because that defeats safety

};	// end struct tagged_subtype

}	// end namespace util

#endif	// __UTIL_TAGGED_SUBTYPE_H__

