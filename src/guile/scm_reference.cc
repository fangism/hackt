/**
	\file "main/libhackt-wrap.cc"
	$Id: scm_reference.cc,v 1.1 2007/03/16 07:07:19 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
 */

#define	ENABLE_STACKTRACE			0

#include "guile/scm_reference.h"
#include <iostream>
#include <sstream>
// #include "util/libguile.h"
// #include "util/guile_STL.h"

//=============================================================================
// smob wrapped structures

namespace HAC {
namespace guile_wrap {
using HAC::entity::meta_reference_union;
using std::ostringstream;

/**
	Tag identifier initialized by raw_reference_smob_init().
	This tag corresponds to entity::meta_reference_union* (naked pointer).  
	This will be set by the first call to raw_reference_smob_init().
 */
static
scm_t_bits __raw_reference_tag;

const
scm_t_bits& raw_reference_tag(__raw_reference_tag);


#if 0
/**
	Nothing to recursively mark.  
 */
static
SCM
mark_raw_reference(SCM obj) {
	return SCM_UNSPECIFIED;
}
#endif

/**
	Not bothering to use the scm_gc_malloc/free.  
	\return 0 always.
 */
static
size_t
free_raw_reference(SCM obj) {
	// pointer must be heap-allocated
//	std::cerr << "freeing raw-reference." << std::endl;
	meta_reference_union* ptr = 
		reinterpret_cast<meta_reference_union*>(SCM_SMOB_DATA(obj));
	if (ptr) {
		delete ptr;
		ptr = NULL;	// and STAY dead!
	}
	return 0;
}

/**
	\return non-zero to indicate success.
 */
static
int
print_raw_reference(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-reference ", port);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(obj));
	if (ptr && ptr->inst_ref()) {
		ostringstream oss;
		ptr->inst_ref()->what(oss) << " ";
		ptr->inst_ref()->dump_type_size(oss);
		scm_puts(oss.str().c_str(), port);
	} else {
		scm_puts("null", port);
	}
	scm_puts(" >", port);
	return 1;
}

#if 0
static
SCM
equalp_raw_reference(SCM o1, SCM o2) {
}
#endif

/**
	Registers our own raw-reference type as a smob to guile.  
	Direct use of this type should be avoided where possible.  
	This registers the type exactly once.
 */
void
raw_reference_smob_init(void) {
//	std::cout << raw_reference_tag << std::endl;	// is 0
if (!raw_reference_tag) {	// first time reads 0-initialized
	__raw_reference_tag =
		scm_make_smob_type("raw-reference",
			sizeof(HAC::entity::meta_reference_union));
#if 0
	// experiment: what happens?  VERY VERY BAD
	std::cout << raw_reference_tag << std::endl;
	raw_reference_tag = scm_make_smob_type("raw-reference",
		sizeof(HAC::entity::meta_reference_union));
	std::cout << raw_reference_tag << std::endl;
#endif
//	scm_set_smob_mark(raw_reference_tag, mark_raw_reference);
	scm_set_smob_free(raw_reference_tag, free_raw_reference);
	scm_set_smob_print(raw_reference_tag, print_raw_reference);
//	scm_set_smob_equalp(raw_reference_tag, equalp_raw_reference);
}
}	// end raw_reference_smob_init

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

