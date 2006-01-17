/**
	\file "util/memory/deallocation_policy.h"
	Definition of frequently used deallocation policies.  
	$Id: deallocation_policy.h,v 1.1.2.1 2006/01/17 20:55:28 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_H__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_H__

#include "util/memory/deallocation_policy_fwd.h"
#include "util/FILE_fwd.h"
extern	void free(void*);
extern	int fclose(FILE*);

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
	Deallocate something allocated by new, single object allocation.
	This is the default assumed by all pointer classes.  
	\sa delete_array_tag.  
 */
struct delete_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
		delete t;
	}
};	// end struct delete_tag

//-----------------------------------------------------------------------------
/**
	Deallocate something allocated by new [], object array allocations.
	\sa delete_tag.
 */
struct delete_array_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
		delete [] t;
	}
};	// end struct delete_array_tag

//-----------------------------------------------------------------------------
/**
	Deallocate something allocated by malloc.
	For the record, this is equivalent to:
	custom_functor_tag<>
 */
struct free_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
		free(t);
	}
};	// end struct free_tag

//-----------------------------------------------------------------------------
/**
	Close a stream opened by fopen().
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
 */
template <class T, void (*f)(T*)>
struct custom_ptr_fun_tag {
	inline
	void
	operator () (T*) {
		(*f)(t);
	}
};	// end class custom_ptr_fun_tag

//-----------------------------------------------------------------------------
/**
	Roll your own deallocation policy.
	Works well with allocator-based methods.  
 */
template <class T, void (f)(T*)>
struct custom_fun_ref_tag {
	inline
	void
	operator () (T*) {
		f(t);
	}
};	// end class custom_fun_ref_tag

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DEALLOCATION_POLICY_H__

