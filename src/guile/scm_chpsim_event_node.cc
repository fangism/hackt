/**
	\file "guile/scm_chpsim_event_node.cc"
	$Id: scm_chpsim_event_node.cc,v 1.1.2.4 2007/03/25 21:03:25 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "sim/chpsim/Event.h"
#include "Object/lang/CHP.h"	// for dynamic_cast on actions
#include "guile/scm_chpsim_event_node.h"
// #include <iostream>		// temporary
#include <sstream>
#include "util/guile_STL.h"

namespace HAC {
namespace guile_wrap {
using HAC::SIM::CHPSIM::EventNode;
using HAC::entity::CHP::action;
using HAC::entity::CHP::deterministic_selection;
using HAC::entity::CHP::nondeterministic_selection;
// using std::cout;		// temporary
using std::ostringstream;
using util::guile::make_scm;
using util::guile::scm_gsubr_type;
using util::guile::scm_c_define_gsubr_exported;

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
	Extracts event pointer from SMOB.  
	\return pointer to entry (never trivial).
	\throw SCM exception if type assertion fails.  
 */
static
scm_chpsim_event_node_ptr
scm_smob_to_chpsim_event_node_ptr(SCM obj) {
	scm_assert_smob_type(raw_chpsim_event_node_ptr_tag, obj);
	return reinterpret_cast<scm_chpsim_event_node_ptr>(SCM_SMOB_DATA(obj));
}

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
		scm_smob_to_chpsim_event_node_ptr(obj);
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
// guile primitive functions that operate on chpsim-event SMOBs

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is trivial type, which includes joins 
		end-of-selection, and concurrent forks, 
		but not condition waits.
	See EventNode::is_trivial.
 */
static
SCM
wrap_chpsim_event_trivial_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-trivial?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
//	const action* const a = ptr->get_chp_action();
//	const unsigned short t = ptr->get_event_type();
	return make_scm<bool>(ptr->is_trivial());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is an condition wait.
 */
static
SCM
wrap_chpsim_event_wait_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-wait?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_CONDITION_WAIT);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is an assignment.
 */
static
SCM
wrap_chpsim_event_assign_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-assign?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_ASSIGN);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a send.
 */
static
SCM
wrap_chpsim_event_send_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-send?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SEND);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a receive.
 */
static
SCM
wrap_chpsim_event_receive_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-send?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_RECEIVE);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a concurrent fork.
 */
static
SCM
wrap_chpsim_event_fork_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-fork?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_CONCURRENT_FORK);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a selection (deterministic or non).
 */
static
SCM
wrap_chpsim_event_select_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-select?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN);
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a deterministic selection.
 */
static
SCM
wrap_chpsim_event_select_det_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-select-det?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>((ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN) &&
		IS_A(const deterministic_selection*, ptr->get_chp_action()));
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a nondeterministic selection.
 */
static
SCM
wrap_chpsim_event_select_nondet_p(SCM obj) {
#define	FUNC_NAME "chpsim-event-select-nondet?"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>((ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN) &&
		IS_A(const nondeterministic_selection*, ptr->get_chp_action()));
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return process id of the allocated event, which may be 0
		to denote a top-level event.
 */
static
SCM
wrap_chpsim_event_process_index(SCM obj) {
#define	FUNC_NAME "chpsim-event-process-id"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_process_index());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return delay value of event.  
 */
static
SCM
wrap_chpsim_event_delay(SCM obj) {
#define	FUNC_NAME "chpsim-event-delay"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_delay());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return number of mandatory predecessors.  
 */
static
SCM
wrap_chpsim_event_num_predecessors(SCM obj) {
#define	FUNC_NAME "chpsim-event-num-predecessors"
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_predecessors());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register some basic event_node interface functions?
	This will be loaded into the current (active) module.  
	\pre chpsim-event-node SMOB tag already initialized.  
 */
void
import_chpsim_event_node_functions(void) {
	INVARIANT(raw_chpsim_event_node_ptr_tag);
	scm_c_define_gsubr_exported("chpsim-event-trivial?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_trivial_p));
	scm_c_define_gsubr_exported("chpsim-event-wait?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_wait_p));
	scm_c_define_gsubr_exported("chpsim-event-assign?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_assign_p));
	scm_c_define_gsubr_exported("chpsim-event-send?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_send_p));
	scm_c_define_gsubr_exported("chpsim-event-receive?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_receive_p));
	scm_c_define_gsubr_exported("chpsim-event-fork?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_fork_p));
	scm_c_define_gsubr_exported("chpsim-event-select?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_select_p));
	scm_c_define_gsubr_exported("chpsim-event-select-det?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_select_det_p));
	scm_c_define_gsubr_exported("chpsim-event-select-nondet?", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_select_nondet_p));
	scm_c_define_gsubr_exported("chpsim-event-process-id", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_process_index));
	scm_c_define_gsubr_exported("chpsim-event-delay", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_delay));
	scm_c_define_gsubr_exported("chpsim-event-num-predecessors", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_event_num_predecessors));
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

