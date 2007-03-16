/**
	\file "guile/scm_reference.cc"
	$Id: scm_reference.h,v 1.1 2007/03/16 07:07:19 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
 */

#include "util/libguile.h"
#include "Object/ref/meta_reference_union.h"

//=============================================================================
// smob wrapped structures

namespace HAC {
namespace guile_wrap {
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

}	// end namespace guile_wrap
}	// end namespace HAC

