/**
	\file "util/memory/deallocation_policy.h"
	Definition of frequently used deallocation policies.  
	$Id: deallocation_policy.h,v 1.1.2.3 2006/01/18 07:01:06 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_H__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_H__

#include "util/memory/deallocation_policy_fwd.h"
#include "util/FILE_fwd.h"
#include "util/free.h"
#include "util/type_traits.h"

BEGIN_C_DECLS
extern	int fclose(FILE*);
END_C_DECLS

namespace util {
namespace memory {
//=============================================================================
/**
	General purpose customizable functor deallocation policy.  
	Recommend using <functional> with this.  
	mem_fun, ptr_fun, mem_fun_ref, etc...
	TODO: helper inline functions to simplify use.  
	\param unary functor.  
 */
template <class F>
struct custom_functor_tag {
	typedef	typename	F::argument_type	argument_type;
	// no return_type necessary
	inline
	void
	operator () (argument_type t) const {
		F()(t);
	}
};	// end struct custom_functor_tag

#if 0
/// helper function to leverage template argument deduction
template <class F>
inline
F
custom_functor_tag_t(F f) {
	return custom_functor_tag<F>();
}
#endif

//-----------------------------------------------------------------------------
/**
	Deallocate something allocated by malloc.
	For the record, this is equivalent to:
	custom_functor_tag<>
 */
struct free_tag {
	/**
		Since free() takes a void*, we need to cast-away
		any constness in type T.  
	 */
	template <class T>
	inline
	void
	operator () (T* t) const {
		free(const_cast<typename remove_const<T>::type*>(t));
	}
};	// end struct free_tag

//-----------------------------------------------------------------------------
/**
	Close a stream opened by fopen().
	A FILE pointer class should really never use const FILE*.  
 */
struct fclose_tag {
	inline
	void
	operator () (FILE* t) const {
		fclose(t);
	}
};	// end struct FILE_tag

//-----------------------------------------------------------------------------
/**
	Close a stream opened by .open().
	Can be anything with a close() member function.
 */
struct iostream_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
		t->close();
	}
};	//  end iostream_tag

//-----------------------------------------------------------------------------
/**
	Roll your own deallocation policy.
	Works well with allocator-based methods.  
	TODO: helper function to deduce template argument.
 */
template <class T, void (*f)(T*)>
struct custom_ptr_fun_tag {
	inline
	void
	operator () (T* t) {
		(*f)(t);
	}
};	// end class custom_ptr_fun_tag

//-----------------------------------------------------------------------------
/**
	Roll your own deallocation policy.
	Works well with allocator-based methods.  
	TODO: helper function to deduce template argument.
 */
template <class T, void (f)(T*)>
struct custom_fun_ref_tag {
	inline
	void
	operator () (T* t) {
		f(t);
	}
};	// end class custom_fun_ref_tag

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DEALLOCATION_POLICY_H__

