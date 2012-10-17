/**
	\file "util/persistent_fwd.hh"
	Forward declaration of object-persistence-related classes.
	$Id: persistent_fwd.hh,v 1.1 2005/05/22 06:24:21 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_FWD_H__
#define	__UTIL_PERSISTENT_FWD_H__

namespace util {

class persistent;
class persistent_object_manager;

/**
	Traits-class for persistent objects.  
	The default definition is empty, intentionally --
	the user must provide a specialized definition per persistent class.
	We provide convenient macros for doing this.  

	Typically, these are referenced by both the specialized class
	and the persistent object manager template methods.
	To make the traits visible to outside users, the declaration
	of the specialization must be globally visible, in a header, 
	as opposed to in a source module.  
 */
template <class>
struct persistent_traits;	// end struct persistent_traits

}	// end namespace util

//=============================================================================
// macros

/***
	Standard set of prototypes for persistent object IO-related
	methods.  
	I got sick of typing and pasting them over and over...
	Should be in classes' public sections.  
	Note: this macro should be used only in final concrete classes, 
	because they are all non-virtual.  
	Don't stick a semicolon after this.  
***/

/**
	If constructor is private, use this to grant persistent_traits
	access to the private constructor.  
	NOTE: this macro requires "util/new_functor_fwd.h"
 */
#define	FRIEND_PERSISTENT_TRAITS					\
	friend struct util::new_functor<this_type,util::persistent>;	\
	friend struct util::persistent_traits<this_type>;

#define	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS		\
	void								\
	write_object(const util::persistent_object_manager&, 		\
		std::ostream&) const;					\
	void								\
	load_object(const util::persistent_object_manager&, std::istream&);

/**
	Same thing, but virtual functions.  
 */
#define	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS	\
virtual	void								\
	write_object(const util::persistent_object_manager&, 		\
		std::ostream&) const;					\
virtual	void								\
	load_object(const util::persistent_object_manager&, std::istream&);

#define	PERSISTENT_METHODS_DECLARATIONS_NO_POINTERS			\
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
#define	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_POINTERS		\
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS

#define	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC			\
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS		\
	void								\
	collect_transient_info(util::persistent_object_manager&) const;

#define	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC		\
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS	\
virtual	void								\
	collect_transient_info(util::persistent_object_manager&) const;

#define	PERSISTENT_METHODS_DECLARATIONS					\
	PERSISTENT_METHODS_DECLARATIONS_NO_POINTERS			\
	void								\
	collect_transient_info(util::persistent_object_manager&) const;

#define	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS				\
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS_NO_POINTERS		\
virtual	void								\
	collect_transient_info(util::persistent_object_manager&) const;

//=============================================================================

#endif	// __UTIL_PERSISTENT_FWD_H__

