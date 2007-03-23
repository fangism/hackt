/**
	\file "guile/scm_chpsim_event_node.h"
	$Id: scm_chpsim_event_node.h,v 1.1.2.1 2007/03/23 23:16:21 fang Exp $
 */

#ifndef	__HAC_GUILE_SCM_CHPSIM_EVENT_NODE_H__
#define	__HAC_GUILE_SCM_CHPSIM_EVENT_NODE_H__

#include "util/libguile.h"
#include "sim/chpsim/Event.h"

namespace HAC {
namespace guile_wrap {
//=============================================================================
/**
	We store a pointer that should not be memory managed, 
	as it points to a static element of an array.  
	Invariant: the pointer shall remain valid through the lifetime
	of the guile interpreter.  
	No reallocation allowed!
 */
typedef	HAC::SIM::CHPSIM::EventNode*	scm_chpsim_event_node_ptr;

/**
	Run-time type identifier set upon guile's SMOB registration.  
 */
extern
const scm_t_bits& raw_chpsim_event_node_ptr_tag;

extern
void
raw_chpsim_event_node_ptr_smob_init(void);

extern
void
import_chpsim_event_node_functions(void);

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

#endif	// __HAC_GUILE_SCM_CHPSIM_EVENT_NODE_H__

