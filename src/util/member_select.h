/**
	\file "util/member_select.h"
	Functional helper for referencing a member of an object.  
	Based on mem_fun and mem_fun_ref.  
	$Id: member_select.h,v 1.1 2006/07/13 22:21:39 fang Exp $
 */

#ifndef	__UTIL_MEMBER_SELECT_H__
#define	__UTIL_MEMBER_SELECT_H__

#include <functional>
#include "util/type_traits.h"

namespace util {
using std::unary_function;

//=============================================================================
/**
	Member-of-object selector pointer-member adaptors, 
	for functional programming.  
	This family has 2 variations:
	(1) Call through pointer vs. call through reference.  
	
 */
template <class MT, class OT>
class member_select_t :
		public unary_function<OT*, typename add_reference<MT>::type> {
	typedef	unary_function<OT*, typename add_reference<MT>::type>
					base_type;
public:
	typedef	OT			object_type;
	typedef	MT			member_type;
	typedef	member_type		object_type::*member_ptr_type;
	/**
		Whether or not we want to access the member by value (copy)
		or reference.  Type-traits avoids reference-to-reference.  
		Not sure if we want this to be automatic?
		Certainly when we want to mutate the member, a reference
		is required.  
	 */
	typedef	typename base_type::result_type		result_type;
	typedef	typename base_type::argument_type	argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	member_select_t(member_ptr_type _m) : _member(_m) { }

	result_type
	operator () (argument_type o) const {
		return o->*_member;
	}
};	// end struct member_select_t

//=============================================================================
/**
	Same thing, but through a reference interface.  
 */
template <class MT, class OT>
class member_select_ref_t :
		public unary_function<
			typename add_reference<OT>::type,
			typename add_reference<MT>::type> {
	typedef	unary_function<typename add_reference<OT>::type,
			typename add_reference<MT>::type>
					base_type;
public:
	typedef	OT			object_type;
	typedef	MT			member_type;
	typedef	member_type		object_type::*member_ptr_type;
	typedef	typename base_type::result_type		result_type;
	/**
		Now argument type is a reference.  
	 */
	typedef	typename base_type::argument_type	argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	member_select_ref_t(member_ptr_type _m) : _member(_m) { }

	result_type
	operator () (argument_type o) const {
		return o.*_member;
	}
};	// end struct member_select_ref_t

//=============================================================================
// provide inline-helper functor generators
// Thank Bjarne for template argument deduction!

template <class MT, class OT>
inline
member_select_t<MT, OT>
member_select(MT OT::*_mem) {
	return member_select_t<MT, OT>(_mem);
}

template <class MT, class OT>
inline
member_select_ref_t<MT, OT>
member_select_ref(MT OT::*_mem) {
	return member_select_ref_t<MT, OT>(_mem);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MEMBER_SELECT_H__

