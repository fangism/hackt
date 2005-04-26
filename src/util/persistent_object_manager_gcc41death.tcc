/**
	\file "persistent_object_manager_gcc41death.tcc"
	Template methods for persistent_object_manager class.
	$Id: persistent_object_manager_gcc41death.tcc,v 1.1.2.2 2005/04/26 00:13:10 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__
#define	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

#include "persistent_object_manager_gcc41death.h"
// #include "memory/pointer_manipulator.h"
// #include "IO_utils.h"
// #include <cassert>
extern "C" {
extern void __eprintf (const char *, const char *, unsigned, const char *)
    __attribute__ ((noreturn));
}


#define	WELCOME_TO_TYPE_REGISTRATION			0

namespace util {
//=============================================================================
using namespace util::memory;

//=============================================================================
// class persistent_object_manager template method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
template <class P>
inline		// ICE!!!
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const raw_pointer_tag) const {
#if 1		// ICE
	typedef	persistent_object_manager::visit_info*	return_type;
	typedef typename pointer_traits<P>::pointer	pointer_type;
	unsigned long i = 0;
//	read_value(f, i);	// not needed to ICE!
	check_reconstruction_table_range(i);	// was INVARIANT
	const std::pair<persistent*, visit_info*> pv(lookup_ptr_visit_info(i));
	persistent* const o(pv.first);
	// for this to work, pointer_type must be a raw_pointer
	const_cast<P&>(ptr) = dynamic_cast<pointer_type>(o);
//	if (o) assert(ptr);		// ICE!!!
	if (o)
		((void) ((ptr) ? 0 :
			(__eprintf ("%s:%u: failed assertion `%s'\n",
			"util/persistent_object_manager_gcc41death.tcc",
			46, "ptr"), 0)));
	return pv.second;
#else
	return NULL;
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <class P>
inline		// ICE!!!
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const single_owner_pointer_tag) const {
#if 0
	const typename pointer_traits<P>::pointer&
		p = pointer_manipulator::get_pointer(ptr);
	return __read_pointer(f, p, __pointer_category(p));
#else
	return NULL;
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a pointer in reference-count mode.  
 */
template <class P>
// inline		// ICE!!!
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const shared_owner_pointer_tag) const {
#if 1
	typedef	persistent_object_manager::visit_info*	return_type;
	typedef typename pointer_traits<P>::pointer	pointer_type;
	// not reference here, use a local copy first!
#if 0
	const pointer_type p = pointer_manipulator::get_pointer(ptr);
#else
	const pointer_type p(NULL);
#endif
	const return_type ret = __read_pointer(f, p, __pointer_category(p));
	if (p) {
		size_t* c = lookup_ref_count(p);
//		NEVER_NULL(c);
		// uses the unsafe constructor
		const_cast<P&>(ptr) = P(p, c);
		// the reference-count pointer is responsible for
		// deleting the size_t*
	} else {
		const_cast<P&>(ptr) = P(NULL);
	}
	return ret;
#else
	return NULL;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ALERT: this intentially and coercively discards const-ness!
	Need to specialize for reference counter pointers!

	Marking and classifying visits is the manager's way of
	knowing which addresses need to be deleted upon
	termination of the reconstruction table.  
 */
template <class P>
void
persistent_object_manager::read_pointer(istream& f, const P& ptr) const {
//	STACKTRACE("pom::read_pointer()");
	visit_info* v = __read_pointer(f, ptr, __pointer_category(ptr));
//	NEVER_NULL(v);
	// really the value of the pointer is irrelevant, just the ownership
	v->mark_visit(ptr);
}

//=============================================================================
}	// end namespace util

#endif	//	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

