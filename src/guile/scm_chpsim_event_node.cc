/**
	\file "guile/scm_chpsim_event_node.cc"
	$Id: scm_chpsim_event_node.cc,v 1.1.2.6 2007/03/27 22:00:40 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "sim/chpsim/Event.h"
#include "sim/chpsim/Dependence.h"
#include "Object/lang/CHP.h"	// for dynamic_cast on actions
#include "Object/traits/classification_tags_fwd.h"
#include "guile/scm_chpsim_event_node.h"
#include "guile/hackt-documentation.h"
// #include <iostream>		// temporary
#include <sstream>
#include "util/guile_STL.h"
#include "util/for_all.h"
#include "util/caller.h"

namespace HAC {
namespace guile_wrap {
using HAC::SIM::CHPSIM::EventNode;
using HAC::entity::CHP::action;
using HAC::entity::CHP::deterministic_selection;
using HAC::entity::CHP::nondeterministic_selection;
using HAC::SIM::CHPSIM::DependenceSet;
using HAC::entity::bool_tag;
using HAC::entity::int_tag;
using HAC::entity::enum_tag;
using HAC::entity::channel_tag;
// using std::cout;		// temporary
using std::ostringstream;
using util::guile::make_scm;
using util::guile::make_scm_list;
using util::guile::scm_gsubr_type;
using util::guile::scm_c_define_gsubr_exported;

//=============================================================================
/**
	Writeable private tag.  Write once-only please!
 */
static
scm_t_bits __raw_chpsim_event_node_ptr_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
std::vector<scm_init_func_type>		local_registry;

// convenient macro for registration
#define HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, DOCSTRING) \
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT,			\
	VAR, ARGLIST, local_registry, DOCSTRING)

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
	scm_puts("<raw-chpsim-event-node: ", port);	// "#<"
	// TODO: print something about state?
	scm_assert_smob_type(raw_chpsim_event_node_ptr_tag, obj);
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	NEVER_NULL(ptr);
	ostringstream oss;
	ptr->dump_struct(oss);			// too verbose?
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
#define	FUNC_NAME "chpsim-event-trivial?"
HAC_GUILE_DEFINE(wrap_chpsim_event_trivial_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} (smob) trivial? (fork, join, end-select)") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
//	const action* const a = ptr->get_chp_action();
//	const unsigned short t = ptr->get_event_type();
	return make_scm<bool>(ptr->is_trivial());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is an condition wait.
 */
#define	FUNC_NAME "chpsim-event-wait?"
HAC_GUILE_DEFINE(wrap_chpsim_event_wait_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a condition-wait?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_CONDITION_WAIT);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is an assignment.
 */
#define	FUNC_NAME "chpsim-event-assign?"
HAC_GUILE_DEFINE(wrap_chpsim_event_assign_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} an assignment?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_ASSIGN);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a send.
 */
#define	FUNC_NAME "chpsim-event-send?"
HAC_GUILE_DEFINE(wrap_chpsim_event_send_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a channel send?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SEND);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a receive.
 */
#define	FUNC_NAME "chpsim-event-receive?"
HAC_GUILE_DEFINE(wrap_chpsim_event_receive_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a channel receive?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_RECEIVE);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a concurrent fork.
 */
#define	FUNC_NAME "chpsim-event-fork?"
HAC_GUILE_DEFINE(wrap_chpsim_event_fork_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a concurrent fork?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_CONCURRENT_FORK);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a selection (deterministic or non).
 */
#define	FUNC_NAME "chpsim-event-select?"
HAC_GUILE_DEFINE(wrap_chpsim_event_select_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a selection? (deterministic or non-deterministic)") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>(ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a deterministic selection.
 */
#define	FUNC_NAME "chpsim-event-select-det?"
HAC_GUILE_DEFINE(wrap_chpsim_event_select_det_p, FUNC_NAME, 1, 0, 0, (SCM obj),
"Is the event @var{obj} a deterministic selection?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>((ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN) &&
		IS_A(const deterministic_selection*, ptr->get_chp_action()));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\param SMOB of the scm chpsim-event.
	\return #t if event is a nondeterministic selection.
 */
#define	FUNC_NAME "chpsim-event-select-nondet?"
HAC_GUILE_DEFINE(wrap_chpsim_event_select_nondet_p, FUNC_NAME, 1, 0, 0, 
	(SCM obj),
"Is the event @var{obj} a nondeterministic selection?") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm<bool>((ptr->get_event_type() ==
		HAC::SIM::CHPSIM::EVENT_SELECTION_BEGIN) &&
		IS_A(const nondeterministic_selection*, ptr->get_chp_action()));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return process id of the allocated event, which may be 0
		to denote a top-level event.
 */
#define	FUNC_NAME "chpsim-event-process-id"
HAC_GUILE_DEFINE(wrap_chpsim_event_process_index, FUNC_NAME, 1, 0, 0, 
	(SCM obj),
"Return the parent process index of the indexed event @var{obj} (smob).") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_process_index());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return delay value of event.  
 */
#define	FUNC_NAME "chpsim-event-delay"
HAC_GUILE_DEFINE(wrap_chpsim_event_delay, FUNC_NAME, 1, 0, 0, (SCM obj),
"Return the delay assigned to event @var{obj} (smob).") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_delay());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return number of *mandatory* predecessors, the number of events
		that must arrive (in graph) before this event fires.  
 */
#define	FUNC_NAME "chpsim-event-num-predecessors"
HAC_GUILE_DEFINE(wrap_chpsim_event_num_predecessors, FUNC_NAME, 1, 0, 0, 
	(SCM obj),
"Return the number of predecessors required to arrive before this event "
"may fire.") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->get_predecessors());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unary function.
	\param SMOB of the scm chpsim-event.
	\return number of successors.  
 */
#define	FUNC_NAME "chpsim-event-num-successors"
HAC_GUILE_DEFINE(wrap_chpsim_event_num_successors, FUNC_NAME, 1, 0, 0, 
	(SCM obj),
"Return the number of successors events that follow this event @var{obj}.") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm(ptr->successor_events.size());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param SMOB of the scm chpsim-event.
	\return number list of successors.  
 */
#define	FUNC_NAME "chpsim-event-successors"
HAC_GUILE_DEFINE(wrap_chpsim_event_successors, FUNC_NAME, 1, 0, 0, (SCM obj),
"Return a list of the successor events of event @var{obj}.") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	return make_scm_list(ptr->successor_events);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Internal function that needs to be post-processed
	by "hackt/chpsim.scm": chpsim-event-may-block-deps,
		which adds the appropriate tags.  
	Make sure this is used consistently, by construction.  
 */
#define	FUNC_NAME "chpsim-event-may-block-deps-internal"
HAC_GUILE_DEFINE(wrap_chpsim_event_may_block_deps_internal, FUNC_NAME, 1, 0, 0,
	(SCM obj),
"Return a set of instances what this event @var{obj} *may* be blocked "
"waiting on.") {
	const scm_chpsim_event_node_ptr ptr =
		scm_smob_to_chpsim_event_node_ptr(obj);
	const DependenceSet bds(ptr->get_block_dependencies());
	SCM b = make_scm_list(bds.get_instance_set<bool_tag>());
	SCM i = make_scm_list(bds.get_instance_set<int_tag>());
	SCM e = make_scm_list(bds.get_instance_set<enum_tag>());
	SCM c = make_scm_list(bds.get_instance_set<channel_tag>());
	// make_scm_list?
	return scm_cons(b, scm_cons(i, scm_cons(e, scm_cons(c, SCM_EOL))));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register some basic event_node interface functions?
	This will be loaded into the current (active) module.  
	\pre chpsim-event-node SMOB tag already initialized.  
 */
void
import_chpsim_event_node_functions(void) {
	INVARIANT(raw_chpsim_event_node_ptr_tag);
	util::for_all(local_registry, util::caller());
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

