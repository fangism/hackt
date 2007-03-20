/**
	\file "guile/scm_reference.cc"
	$Id: scm_reference.h,v 1.1.2.1 2007/03/20 23:10:36 fang Exp $
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
void
raw_reference_smob_init(void);

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

