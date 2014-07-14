/**
	\file "util/memory/deallocation_policy.hh"
	Definition of frequently used deallocation policies.  
	$Id: deallocation_policy.hh,v 1.4 2011/02/04 02:23:42 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_HH__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_HH__

#include <cassert>
#include <iosfwd>
#include "util/memory/deallocation_policy_fwd.hh"
#include "util/FILE_fwd.h"
#include "util/free.h"
#include "util/type_traits.hh"

BEGIN_C_DECLS
extern	int fclose(FILE*);
extern	int pclose(FILE*);
END_C_DECLS

#if 0
// forward declarations
BEGIN_NAMESPACE_STD
extern istream cin;
extern ostream cout;
extern ostream cerr;
END_NAMESPACE_STD
#endif

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
		const int err = fclose(t);
		assert(!err);
	}
};	// end struct FILE_tag

//-----------------------------------------------------------------------------
/**
	Close a stream opened by popen().
 */
struct pclose_tag {
	inline
	void
	operator () (FILE* t) const {
		const int err = pclose(t);
		assert(!err);
	}
};	// end struct FILE_tag

//-----------------------------------------------------------------------------
struct istream_tag {
	void
	operator () (std::istream*) const;
};

//-----------------------------------------------------------------------------
struct ostream_tag {
	void
	operator () (std::ostream*) const;
};

//-----------------------------------------------------------------------------
#if 0
// iostreams are already closed upon destruction
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
#endif

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

#endif	// __UTIL_MEMORY_DEALLOCATION_POLICY_HH__

