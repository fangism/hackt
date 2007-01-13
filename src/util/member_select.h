/**
	\file "util/member_select.h"
	Functional helper for referencing a member of an object.  
	Based on mem_fun and mem_fun_ref.  
	$Id: member_select.h,v 1.1.28.1 2007/01/13 02:08:31 fang Exp $
 */

#ifndef	__UTIL_MEMBER_SELECT_H__
#define	__UTIL_MEMBER_SELECT_H__

#include <functional>
#include "util/type_traits.h"	// for add_reference

namespace util {
using std::unary_function;

//=============================================================================
/**
	Member-of-object selector pointer-member adaptors, 
	for functional programming.  
	This family has 4 variations:
	(1) Call through pointer vs. call through reference.  
	(2) const-reference, non-const-reference
	TODO: what if result_type is already a reference?
		use type_traits' add_reference?
 */
template <class MT, class OT>
class member_select_t : public unary_function<OT*, MT> {
	typedef	unary_function<OT*, MT>	base_type;
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
	/**
		Argument type is already a pointer.  
	 */
	typedef	typename base_type::argument_type	argument_type;
	typedef	const OT*				const_argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	member_select_t(member_ptr_type _m) : _member(_m) { }

	result_type&
	operator () (const argument_type o) const {
		return o->*_member;
	}

	const result_type&
	operator () (const const_argument_type o) const {
		return o->*_member;
	}

};	// end struct member_select_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Const reference through pointer.  
 */
template <class MT, class OT>
class const_member_select_t : public unary_function<const OT*, MT> {
	typedef	unary_function<const OT*, MT>	base_type;
public:
	typedef	OT			object_type;
	typedef	MT			member_type;
	typedef	member_type		object_type::*member_ptr_type;
	typedef	typename base_type::result_type		result_type;
	typedef	typename base_type::argument_type	argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	const_member_select_t(member_ptr_type _m) : _member(_m) { }

	const result_type&
	operator () (const argument_type o) const {
		return o->*_member;
	}
};	// end struct const_member_select_t
#endif

//=============================================================================
/**
	Same thing, but through a reference interface.  
 */
template <class MT, class OT>
class member_select_ref_t : public unary_function<OT, MT> {
	typedef	unary_function<OT, MT>	base_type;
public:
	typedef	OT			object_type;
	typedef	MT			member_type;
	typedef	member_type		object_type::*member_ptr_type;
	typedef	typename base_type::result_type		result_type;
	typedef	typename base_type::argument_type	argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	member_select_ref_t(member_ptr_type _m) : _member(_m) { }

	result_type&
	operator () (argument_type& o) const {
		return o.*_member;
	}

	const result_type&
	operator () (const argument_type& o) const {
		return o.*_member;
	}
};	// end struct member_select_ref_t

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Same thing, but through a reference interface.  
 */
template <class MT, class OT>
class const_member_select_ref_t : public unary_function<OT, MT> {
	typedef	unary_function<OT, MT>	base_type;
public:
	typedef	OT			object_type;
	typedef	MT			member_type;
	typedef	member_type		object_type::*member_ptr_type;
	typedef	typename base_type::result_type		result_type;
	typedef	typename base_type::argument_type	argument_type;
private:
	member_ptr_type			_member;
public:
	explicit
	const_member_select_ref_t(member_ptr_type _m) : _member(_m) { }

	const result_type&
	operator () (const argument_type& o) const {
		return o.*_member;
	}
};	// end struct const_member_select_ref_t
#endif

//=============================================================================
// provide inline-helper functor generators
// Thank Bjarne for template argument deduction!

template <class MT, class OT>
inline
member_select_t<MT, OT>
member_select(MT OT::*_mem) {
	return member_select_t<MT, OT>(_mem);
}

#if 0
template <class MT, class OT>
inline
const_member_select_t<MT, OT>
const_member_select(MT OT::*_mem) {
	return const_member_select_t<MT, OT>(_mem);
}
#endif

template <class MT, class OT>
inline
member_select_ref_t<MT, OT>
member_select_ref(MT OT::*_mem) {
	return member_select_ref_t<MT, OT>(_mem);
}

#if 0
template <class MT, class OT>
inline
const_member_select_ref_t<MT, OT>
const_member_select_ref(MT OT::*_mem) {
	return member_select_ref_t<MT, OT>(_mem);
}
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MEMBER_SELECT_H__

