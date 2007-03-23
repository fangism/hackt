/**
	\file "guile/scm_chpsim_event_node.cc"
	$Id: scm_chpsim_event_node.cc,v 1.1.2.1 2007/03/23 23:16:21 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "guile/scm_chpsim_event_node.h"
// #include <iostream>		// temporary
#include <sstream>

namespace HAC {
namespace guile_wrap {
using HAC::SIM::CHPSIM::EventNode;
// using std::cout;		// temporary
using std::ostringstream;

//=============================================================================
/**
	Writeable private tag.  Write once-only please!
 */
static
scm_t_bits __raw_chpsim_event_node_ptr_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public read-only reference/alias.
 */
const
scm_t_bits& raw_chpsim_event_node_ptr_tag(__raw_chpsim_event_node_ptr_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a pointer that should not be managed, as it points
	to the middle of a pool (array).  
	It should just be dropped.
	\return 0 always
 */
static
size_t
free_raw_chpsim_event_node_ptr(SCM obj) {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Probably prints too much information by default...
	\return non-zero to indicate success.
 */
static
int
print_raw_chpsim_event_node_ptr(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("<raw-chpsim-event-node: ", port);
	// TODO: print something about state?
	scm_assert_smob_type(raw_chpsim_event_node_ptr_tag, obj);
	const scm_chpsim_event_node_ptr ptr =
		reinterpret_cast<scm_chpsim_event_node_ptr>(SCM_SMOB_DATA(obj));
	NEVER_NULL(ptr);
	ostringstream oss;
	ptr->dump_struct(oss);
	scm_puts(oss.str().c_str(), port);
	scm_puts(">", port);
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
raw_chpsim_event_node_ptr_smob_init(void) {
if (!raw_chpsim_event_node_ptr_tag) {
	__raw_chpsim_event_node_ptr_tag = 
		scm_make_smob_type("raw-chpsim-event-node",
			sizeof(scm_chpsim_event_node_ptr));
	INVARIANT(raw_chpsim_event_node_ptr_tag);
//	scm_set_smob_mark(raw_chpsim_event_node_ptr_tag, ...);
	scm_set_smob_free(raw_chpsim_event_node_ptr_tag,
		free_raw_chpsim_event_node_ptr);
	scm_set_smob_print(raw_chpsim_event_node_ptr_tag,
		print_raw_chpsim_event_node_ptr);
//	scm_set_smob_equalp(raw_chpsim_event_node_ptr_tag, ...);
}
}

//=============================================================================

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register some basic event_node interface functions?
 */
void
import_chpsim_event_node_functions(void) {
	INVARIANT(raw_chpsim_event_node_ptr_tag);
	
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

