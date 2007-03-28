/**
	\file "guile/scm_reference.h"
	$Id: scm_reference.h,v 1.1.2.3 2007/03/28 19:36:58 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
 */

#include "util/libguile.h"
#include "Object/ref/meta_reference_union.h"

namespace HAC {
namespace guile_wrap {
//=============================================================================
// smob wrapped structures

typedef HAC::entity::meta_reference_union	scm_reference_union;

/**
	Tag identifier initialized by raw_reference_smob_init().
	This tag corresponds to entity::meta_reference_union* (naked pointer).  
 */
extern
const scm_t_bits& raw_reference_tag;

extern
const scm_reference_union*
scm_smob_to_raw_reference_ptr(const SCM&);

/// registers the smob type
extern
void
raw_reference_smob_init(void);

// loads collection of raw-reference related functions
// should this be extern "C"?
extern
void
load_raw_reference_functions(void);

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

