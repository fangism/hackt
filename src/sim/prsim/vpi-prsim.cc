/**
	\file "sim/prsim/vpi-prsim.cc"
	$Id: vpi-prsim.cc,v 1.6 2008/11/24 20:49:54 fang Exp $
	Thanks to Rajit for figuring out how to do this and providing
	a reference implementation, which was yanked from:
 */
/*************************************************************************
 *  Copyright (c) 2007 Achronix Semiconductor
 *  All Rights Reserved
 *  Id: vpi_prsim.c,v 1.3 2007/11/26 19:20:02 rajit Exp
 **************************************************************************
 */
/**
	Thanks to Rajit for doing the hard work!
	Many names of symbols were kept the same to honor their origin.  
	Do we need to worry about static global initializers ordering?
 */

// for desparate times...
#define	DEBUG_MAX				0

#define	ENABLE_STACKTRACE			(0 || DEBUG_MAX)
#define	ENABLE_STATIC_TRACE			(0 || DEBUG_MAX)
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <cstdio>
#include <cctype>
#include <map>
#include <iostream>
#include <sstream>
#include "sim/prsim/State-prsim.h"
#include "sim/prsim/Command-prsim-export.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "sim/prsim/Command-prsim.h"
#include "parser/instref.h"
#include "main/main_funcs.h"
#include "util/memory/count_ptr.tcc"
#include "util/string.h"
#include "util/tokenize.h"		// only for debugging
#include "util/inttypes.h"
#include "util/c_decl.h"
#include "util/stacktrace.h"

#ifndef	WIN32
#define	WIN32	0
#endif

#include "vpi_user.h"

/**
	Debugging switches
 */
#define	VERBOSE_DEBUG		(0 || DEBUG_MAX)
#define	TRACE_VCS_TIME		(0 || DEBUG_MAX)

#if TRACE_VCS_TIME
#define	SHOW_VCS_TIME(x)	cout << "vcstime: " << x << endl
#else
#define	SHOW_VCS_TIME(x)
#endif

//=============================================================================
namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostringstream;
#include "util/using_ostream.h"
using parser::parse_node_to_index;
using util::memory::count_ptr;
using util::strings::eat_whitespace;
// is current double-precision floating-point
typedef	State::time_type		Time_t;
typedef	State::node_type		node_type;

/**
	Spare map for keeping track of vpiHandles per node.
	Could use a hash_map too, if we cared...
 */
typedef	std::map<node_index_type, vpiHandle>	vpiHandleMapType;

//=============================================================================
static vpiHandleMapType			vpiHandleMap;

// simulator state
#if 0
static Prs *P = NULL;
#else
static count_ptr<module> HAC_module(NULL);
static count_ptr<State>	prsim_state(NULL);
#endif

/**
	Only need to call this upon abrupt termination.
 */
static void __destroy_globals(void) {
	prsim_state = count_ptr<State>();	// destroy, auto-checkpoint
	HAC_module = count_ptr<module>();	// destroy
}

static void _run_prsim (const Time_t& vcstime, const int context);
static void __register_self_callback_have_event(Time_t);

#if 0
static char *skip_white (char *s)
{
  while (*s && isspace (*s)) {
    s++;
  }
  return s;
}
#endif

// TODO: figure out different time types from "vpi_user.h"
// prsim's native time is floating point
static void vcs_to_prstime (const s_vpi_time *p, Time_t *tm)
{
#ifdef TIME_64
  *tm = (uint64(p->high) << 32) | uint64(p->low);
#else
  *tm = p->low;
#endif
}

/**
	TODO: error on overflow?
 */
static void prs_to_vcstime (s_vpi_time *p, const Time_t *tm)
{
#ifdef TIME_64
  const uint64 t = *tm;
  p->high = (t >> 32) & 0xffffffffUL;
  p->low = t & 0xffffffffUL;
#else
  p->high = 0;
  p->low = uint64(*tm) & 0xffffffffUL;
#endif
}

// whether or not callback is currently registered
static int scheduled = 0;

/*
  last scheduled _run_prsim_callback. We need to remove it!!! Insane! 
*/
static vpiHandle last_registered_callback;

//=============================================================================
// common error routines
/**
	\param caller can be compiler-generated string like
		__FUNCTION__ or __PRETTY_FUNCTION__.
 */
static
void
require_prsim_state(const char* caller) {
	if (!prsim_state) {
		cerr << "Call " << caller <<
			" only after a HAC module has been loaded." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to node-name parse and lookup.
	\throw exception if not found.
 */
static
node_index_type
lookup_prsim_name(const char* prsim_name) {
  require_prsim_state(__FUNCTION__);
  const node_index_type n = parse_node_to_index(prsim_name, *HAC_module);
  if (!n) {
    cerr << "Node `" << prsim_name << "\' not found in .prs file!" << endl;
	THROW_EXIT;
  }
  return n;
}

static
node_index_type
lookup_prsim_name(const string& s) {
	return lookup_prsim_name(s.c_str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param vcs_name should really just be a const char*.
 */
static
vpiHandle
lookup_vcs_name(const char* vcs_name) {
  const vpiHandle net =
	vpi_handle_by_name (const_cast<PLI_BYTE8*>(vcs_name), NULL);
  if (!net) {
    cerr << "Net name `" << vcs_name << "' not found in .v file." << endl;
	THROW_EXIT;
  }
  return net;
}

#if 0
static
vpiHandle
lookup_vcs_name(const string& s) {
	return lookup_vcs_name(s.c_str());
}
#endif

//=============================================================================
/**
	\param p callback data.
	p->time is the time-limit to return back to VCS, in VCS's time units
		and time scale.  
	\return no clue
 */
static
PLI_INT32
// was: void
_run_prsim_callback (const p_cb_data p)
{
  STACKTRACE_VERBOSE;
  Time_t curtime;
  scheduled = 0;
  vcs_to_prstime (p->time, &curtime);
  vpi_remove_cb (last_registered_callback);
  _run_prsim (curtime, 1);
// TODO: return something, but what?
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register with VCS the longest time to advance until before
	calling back to prsim to handle its events.  
	May actually return earlier on events on registered nodes.  
	\param vcstime time at which to call back (at the latest)
 */
static void register_self_callback (Time_t vcstime)
{
  STACKTRACE_VERBOSE;
  require_prsim_state(__FUNCTION__);
  SHOW_VCS_TIME(vcstime);
  
  if (scheduled == 1) return;

	STACKTRACE_INDENT_PRINT("not scheduled" << endl);
#if VERBOSE_DEBUG
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
#if 0
  if (heap_peek_min (P->eventQueue) == NULL)
#else
  if (!prsim_state->pending_events())
#endif
  {
    /* I have nothing to do, go away */
    return;
  }
	__register_self_callback_have_event(vcstime);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does the real work in scheduling a callback.  
	\param vcstime time at which to call back (at the latest)
	\pre callback must not already be scheduled.  
	\pre there is event to schedule from prsim.  
	\post callback event to prsim is scheduled.  
 */
static void
__register_self_callback_have_event(Time_t vcstime) {
	STACKTRACE_VERBOSE;
  static s_cb_data cb_data;
  static s_vpi_time tm; /* at most one callback pending, let's not malloc! */
	INVARIANT(!scheduled);
  cb_data.reason = cbAfterDelay;	// relative to now, in future
  cb_data.cb_rtn = _run_prsim_callback;
  cb_data.obj = NULL;

  cb_data.time = &tm;
#if 0
  if (heap_peek_minkey (P->eventQueue) > vcstime)
#else
  const Time_t next_time = prsim_state->next_event_time();
	STACKTRACE_INDENT_PRINT("next_time: " << next_time << endl);
  if (next_time > vcstime)
#endif
  {
	STACKTRACE_INDENT_PRINT("next_time > vcstime" << endl);
#if 0
    vcstime = heap_peek_minkey (P->eventQueue) - vcstime;
#else
    vcstime = next_time - vcstime;
    SHOW_VCS_TIME(vcstime);
#endif
  }
  else {
	STACKTRACE_INDENT_PRINT("next_time <= vcstime" << endl);
    vcstime = 1;
    SHOW_VCS_TIME(vcstime);
  }
	// here, vcstime is the relative time in the future (from now)
  cb_data.time->type = vpiSimTime;
  prs_to_vcstime (cb_data.time, &vcstime);

  cb_data.value = NULL;
  cb_data.user_data = NULL;
  last_registered_callback = vpi_register_cb (&cb_data);
  scheduled = 1;
}
// end __register_self_callback_have_event

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	While prsim has events that are before VCS's next event time, 
	run (unless breakpoint encountered that defers back to VCS).  
	\param vcstime the longest that prsim should run before returning
		control back to VCS (absolute simulation time).  
	What is context for?
	Should be like "advance-until" behavior.  
 */
static void __advance_prsim (const Time_t& vcstime, const int context)
{
  STACKTRACE_VERBOSE;
  State::step_return_type nr;
  SHOW_VCS_TIME(vcstime);

  /* run for at most 1ps */
#if 0
  while ((heap_peek_minkey (P->eventQueue) <= vcstime) 
	 && (n = prs_step_cause (P, &m, &seu)))
#else
  while (prsim_state->pending_events() &&
	(prsim_state->next_event_time() <= vcstime) 
	 && GET_NODE((nr = prsim_state->step())))
#endif
  {
    const node_index_type ni = GET_NODE(nr);
    const node_type& n(prsim_state->get_node(ni));
    const node_index_type m = GET_CAUSE(nr);
    const Time_t prsim_time = prsim_state->time();
    const vpiHandleMapType::iterator
	n_space(vpiHandleMap.find(ni)),
	n_end(vpiHandleMap.end());
    // "n_space" in honor of the abuse of a certain void* PrsNode::*space
#if VERBOSE_DEBUG
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
#if TRACE_VCS_TIME
	cout << "prsim time: " << prsim_time << endl;
#endif
	if (prsim_state->watching_all_nodes()) {
		print_watched_node(cout << "prsim: \t" << prsim_time << '\t', 
			*prsim_state, nr);
	}
    if (n.is_breakpoint() && (n_space != n_end)) {
	STACKTRACE("breakpt && registered");
      s_vpi_value v;
      s_vpi_time tm;

      const Time_t prsdiff = prsim_time - vcstime;

      tm.type = vpiSimTime;
      prs_to_vcstime (&tm, &prsdiff);
      /* aha, schedule an event into the vcs queue */
      const vpiHandle& net = n_space->second;
      v.format = vpiScalarVal;
      INVARIANT(net);
#if VERBOSE_DEBUG
	const string name(prsim_state->get_node_canonical_name(ni));
	const char* const nodename = name.c_str();
#endif
      switch (n.current_value()) {
      case LOGIC_HIGH:
#if VERBOSE_DEBUG
	vpi_printf ("Set net %s (%x) to TRUE\n", nodename, net);
#endif
	v.value.scalar = vpi1;
	break;
      case LOGIC_LOW:
#if VERBOSE_DEBUG
	vpi_printf ("Set net %s (%x) to FALSE\n", nodename, net);
#endif
	v.value.scalar = vpi0;
	break;
      case LOGIC_OTHER:
#if VERBOSE_DEBUG
	vpi_printf ("Set net %s (%x) to X\n", nodename, net);
#endif
	v.value.scalar = vpiX;
	break;
      default:
	DIE;
      }
#if VERBOSE_DEBUG
	ostringstream oss;
	oss << "[tovcs] signal " << name << " changed @ time " <<
		  prsim_time << ", val = " << v.value.scalar;
	vpi_printf("%s\n", oss.str().c_str());
#endif
	// is temporary allocation really necessary? avoidable?
	// vpiPureTransportDelay: affects no other events
	// vpiNoDelay (used by Fang): take effect immediately, ignore time
	// vpiNoDelay seems to be needed when multiple breakpoint events
	// are processed before returning to VCS, as was introduced
	// by $prsim_sync.
      // vpi_free_object (vpi_put_value (net, &v, &tm, vpiPureTransportDelay));
      vpi_free_object (vpi_put_value (net, &v, &tm, vpiNoDelay));
	// Q: shouldn't control return immediately to VCS?
	break;
	// experimenting shows that this makes no difference!? both work
	// WHY?
    }
    else if (n.is_breakpoint() && (n_space == n_end)) {
	STACKTRACE("breakpt && unregistered");
	const string name(prsim_state->get_node_canonical_name(ni));
	ostringstream oss;
	oss << "\t" << prsim_time << " " << name << " : ";
	n.dump_value(oss);
	vpi_printf("%s", oss.str().c_str());
      if (m) {
	ostringstream oss2;
	oss2 << "  [by " << prsim_state->get_node_canonical_name(m) << ":=";
	prsim_state->get_node(m).dump_value(oss2);
	oss2 << "]";
	vpi_printf("%s", oss2.str().c_str());
      }
      vpi_printf ("\n");
    }
  }
}
// end _run_prsim

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does one of two things:
	if events are remaining in prsim, schedule a callback, 
	otherwise synchronize prsim's simulation time to VCS's sim time.  
 */
static
// inline
void
reregister_next_callback(void) {
	if (prsim_state->pending_events()) {
		// re-schedule callback if there are events left
		register_self_callback(prsim_state->next_event_time());
	} else {
		// otherwise, no events in queue, only wait for $to_prsim events
		// don't bother scheduling timed callback
		// it is safe to update prsim's current time 
		// if and only if there are no pending events
		s_vpi_time current_time;
		current_time.type = vpiSimTime;
		vpi_get_time(NULL, &current_time);
		Time_t pcur_time;
		vcs_to_prstime(&current_time, &pcur_time);
		prsim_state->update_time(pcur_time);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Runs prsim until a max time or first reached breakpoint, 
	and reschedules callback when done (if there are events remaining).
 */
static void _run_prsim (const Time_t& vcstime, const int context)
{
	STACKTRACE_VERBOSE;
#if VERBOSE_DEBUG
  cout << "Running prsim @ time " << vcstime << endl;
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
try {
	__advance_prsim(vcstime, context);
} catch (const step_exception& exex) {
	prsim_state->inspect_exception(exex, cerr);
#if PRSIM_NEW_ERROR_POLICIES
	// *ignoring* return value
	// NOTE: we are treating all such exceptions as fatal errors
	// ignoring the per-class error-handling policies set by the user
#endif
	__destroy_globals();
	THROW_EXIT;	// re-throw
} catch (...) {
	// catch all remaining exceptions here, destroy globals and rethrow
	cerr << "hacprsim (VPI) encountered error, terminating..." << endl;
	__destroy_globals();
	throw;		// re-throw
}
#if 0
	if (!prsim_state->pending_events()) {
		// catch up to vcs time?
		prsim_state->update_time(vcstime);
	}
#endif
#if 0
  register_self_callback (vcstime);
#else
#if 0
	if (prsim_state->pending_events()) {
		register_self_callback(prsim_state->next_event_time());
	}
#else
	reregister_next_callback();
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
Expects no parameters (void).  
@texinfo vpi/prsim_sync.texi
@deffn Function $prsim_sync
Synchronize callbacks with current @command{hacprsim} event queue.
This is needed because some @command{$prsim_cmd} commands may 
introduce new events into the event queue, 
which requires re-registration of the callback
function with updated times.  
@end deffn
@end texinfo
 */
static
PLI_INT32
prsim_sync(PLI_BYTE8*) {
	STACKTRACE_VERBOSE;
	require_prsim_state(__FUNCTION__);
	const vpiHandle queue = vpi_handle(vpiTimeQueue, NULL);
	s_vpi_time current_time, queue_time;
	current_time.type = vpiSimTime;
	queue_time.type = vpiSimTime;
	vpi_get_time(NULL, &current_time);
	vpi_get_time(queue, &queue_time);
	Time_t pq_time, pcur_time;
	vcs_to_prstime(&current_time, &pcur_time);
	vcs_to_prstime(&queue_time, &pq_time);
#if 0
	cout << "VCS current time: " << current_time.low << endl;
	cout << "VCS next event time: " << queue_time.low << endl;
	cout << "prsim current time: " << prsim_state->time() << endl;
	cout << "prsim next event time: ";
	if (prsim_state->pending_events()) {
		cout << prsim_state->next_event_time() << endl;
		prsim_state->dump_event_queue(cout);
	} else	cout << "(none)" << endl;
#endif
	if (prsim_state->pending_events()) {
		if (scheduled) {
			// unschedule and reschedule
			vpi_remove_cb (last_registered_callback);
			scheduled = 0;
		}
		// "catch-up" prsim to VCS's time, 
		// even past breakpoints
		do {
			__advance_prsim(pq_time, 0);	// context?
#if 0
			cout << "prsim advanced to time: " <<
				prsim_state->time() << endl;
#endif
		} while (prsim_state->pending_events() &&
			(prsim_state->next_event_time() <= pcur_time));
	}
	reregister_next_callback();
#if 0
	cout << "prsim caught up to time: " << prsim_state->time() << endl;
#endif
	// else no-op, leave alone
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p callback data.
 */
static
PLI_INT32 prsim_callback (s_cb_data *p)
{
  STACKTRACE_VERBOSE;
  // PrsNode *n;
  Time_t vcstime;	// , prsdiff;
  // int bpcount = 0;
  // int gap;

// TODO: use template type_traits on sizeof
#if (SIZEOF_SIZE_T == SIZEOF_VOIDP)
  const node_index_type n = reinterpret_cast<node_index_type>(p->user_data);
  // n = (PrsNode *)p->user_data;
#else
#error "node_index_type cannot fit inside a void* (size mismatch)!"
#endif

#if VERBOSE_DEBUG
#if 0
  vpi_printf ("signal %s changed @ time %d, val = %d\n",
	      prs_nodename (n),
	      p->time->low,
	      p->value->value.scalar);
#else
  cout << "signal " << prsim_state->get_node_canonical_name(n) <<
	" changed @ time " << p->time->low << ", val = " <<
	p->value->value.scalar << endl;
#endif
#endif

  /* convert my time to prsim time */
#if 0
#ifdef TIME_64
  vcstime = ((unsigned long long)p->time->high) << 32 | ((unsigned long long)p->time->low);
#else
  vcstime = p->time->low;
#endif
#else
  vcs_to_prstime(p->time, &vcstime);
  SHOW_VCS_TIME(vcstime);
#endif
/**
	Whether or not set events from VPI are considered forced.
 */
static const bool set_force = true;
  switch (p->value->value.scalar) {
// NOTE: for now, set_node_time is not "forced", hence false argument
  case vpi0:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_F, vcstime);
#else
    prsim_state->set_node_time(n, LOGIC_LOW, vcstime, set_force);
#endif
    break;
  case vpi1:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_T, vcstime);
#else
    prsim_state->set_node_time(n, LOGIC_HIGH, vcstime, set_force);
#endif
    break;
  case vpiZ:
    /* nothing */
    break;
  case vpiX:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_X, vcstime);
#else
    prsim_state->set_node_time(n, LOGIC_OTHER, vcstime, set_force);
#endif
    break;
  default:
    DIE;
    break;
  }
#if VERBOSE_DEBUG
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
  _run_prsim (vcstime, 0);
// TODO: don't know what should be returned, was missing/void before
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
  Register a VCS-driven node with VCS and prsim

@texinfo vpi/to_prsim.texi
@deffn Function $to_prsim vname pname
Establish a connection from Verilog signal @var{vname}
to @command{hacprsim} signal @var{pname}.
When @var{vname} changes value, @command{hacprsim} will be updated
accordingly.  
@end deffn
@end texinfo
*/
static
void register_to_prsim (const char *vcs_name, const char *prsim_name)
{
  STACKTRACE_VERBOSE;
  s_cb_data cb_data;
  // PrsNode *n;
  
  require_prsim_state(__FUNCTION__);

  vcs_name = eat_whitespace (vcs_name);
  prsim_name = eat_whitespace (prsim_name);

  cb_data.reason = cbValueChange;
  cb_data.cb_rtn = prsim_callback;
  const vpiHandle net = lookup_vcs_name(vcs_name);
  cb_data.obj = net;

	// TODO: who frees these mallocs???
	// TODO: is this the right time type?
  cb_data.time = p_vpi_time(malloc (sizeof(s_vpi_time)));
  cb_data.time->type = vpiSimTime;

  cb_data.value = p_vpi_value(malloc (sizeof(s_vpi_value)));
  cb_data.value->format = vpiScalarVal;

#if 0
  n = prs_node (P, prsim_name);
#else
  const node_index_type ni = lookup_prsim_name(prsim_name);
#endif
  
  /* prsim net name */
  cb_data.user_data = reinterpret_cast<PLI_BYTE8*>(ni);	// YUCK, void*
  vpi_register_cb (&cb_data);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
  Register a prsim-driven node with VCS and prsim

@texinfo vpi/from_prsim.texi
@deffn Function $from_prsim pname vname
Establish a connection from @command{hacprsim} signal @var{pname} to
Verilog signal @var{vname}.
When @var{pname} changes value in @command{hacprsim},
the Verilog simulator will be notified accordingly.  
@end deffn
@end texinfo
*/
static
void register_from_prsim (const char *vcs_name, const char *prsim_name)
{
  STACKTRACE_VERBOSE;
  // PrsNode *n;
  
  require_prsim_state(__FUNCTION__);

  vcs_name = eat_whitespace (vcs_name);
  prsim_name = eat_whitespace (prsim_name);

  /* this is the handle to the verilog net name */
  const vpiHandle net = lookup_vcs_name(vcs_name);
  const node_index_type ni = lookup_prsim_name(prsim_name);
  /* set a breakpoint! */
#if 0
  prs_set_bp (P, n);
#else
  prsim_state->set_node_breakpoint(ni);
#endif

#if VERBOSE_DEBUG
#if 0
  vpi_printf ("Net %s is %x\n", prs_nodename (n), net);
#else
  const string name(prsim_state->get_node_canonical_name(ni));
  cout << "Net " <<  name << " is " << static_cast<void*>(net) << endl;
#endif
#endif

  /* set up correspondence */
#if 0
  n->space = (void *)net;
#else
	// TODO: check for existence first?
	vpiHandleMap[ni] = net;
#endif
}

#if 0
/// wrapper.
static
void register_to_prsim(const string& s, const string& p) {
	register_to_prsim(s.c_str(), p.c_str());
}

/// wrapper.
static
void register_from_prsim(const string& s, const string& p) {
	register_from_prsim(s.c_str(), p.c_str());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
@texinfo vpi/prsim_watch.texi
@deffn Function $prsim_watch node
Register a prsim-driven node watch-point.
Synonymous with @command{$prsim_cmd("watch node");}.
@end deffn
@end texinfo
*/
void register_prsim_watchpoint (const char *prsim_name)
{
  STACKTRACE_VERBOSE;
  // vpiHandle net;
  // PrsNode *n;
  
  require_prsim_state(__FUNCTION__);
  prsim_name = eat_whitespace (prsim_name);

  const node_index_type ni = lookup_prsim_name (prsim_name);
  const node_type& n(prsim_state->get_node(ni));
  if (n.is_breakpoint() && (vpiHandleMap.find(ni) != vpiHandleMap.end())) {
    cerr << "Cannot watch node `" << prsim_name <<
		"\'; it already exists in vcs" << endl;
	THROW_EXIT;
  }

  /* set a breakpoint! */
#if 0
  prs_set_bp (P, n);
#else
  prsim_state->set_node_breakpoint(ni);
#endif

#if 0
  vpi_printf ("Net %s is %x\n", prs_nodename (n), net);
#endif

#if 0
  n->space = NULL;
#else
	vpiHandleMap.erase(ni);
#endif
}

//=============================================================================
// exported commands to VPI

static PLI_INT32 to_prsim (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1, net2;
  s_vpi_value arg;
  string arg1;

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);	// memory leak?
  arg1 = arg.value.str;

  net2 = vpi_scan (h);
  if (!net2) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net2, &arg);

#if VERBOSE_DEBUG
  vpi_printf ("setup %s (vcs) -> %s (prsim)\n", arg1.c_str(), arg.value.str);
#endif

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }

  register_to_prsim (arg1.c_str(), arg.value.str);
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static PLI_INT32 from_prsim (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1, net2;
  s_vpi_value arg;
  string arg1;

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $from_prsim(prsim-name,vcs-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);
  arg1 = arg.value.str;

  net2 = vpi_scan (h);
  if (!net2) {
    vpi_printf ("Usage: $from_prsim(prsim-name,vcs-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net2, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $from_prsim(prsim-name, vcs-name)\n");
    return 0;
  }

  register_from_prsim (arg.value.str, arg1.c_str());
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	One command to rule them all, 
	and in the simulator bind them.  
	This passes strings to prsim's command interpreter.  
@texinfo vpi/prsim_cmd.texi
@deffn Function $prsim_cmd cmd
Runs an arbitrary command @var{cmd} (string) that would normally 
be interpreted by @command{hacprsim}.  
This is the one command to rule them all, 
the last command you will ever need.  
@end deffn
@end texinfo
 */
static PLI_INT32 prsim_cmd (PLI_BYTE8* args)
{
  STACKTRACE_VERBOSE;
  s_vpi_value arg;
require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $prsim_cmd(prsim-command)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_cmd(prsim-command)\n");
    return 0;
  }

  if (CommandRegistry::interpret_line (*prsim_state, arg.value.str)) {
	// return on first error
	return 0;
  } else {
	return 1;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static PLI_INT32 prsim_watch (PLI_BYTE8* args)
{
  STACKTRACE_VERBOSE;
  s_vpi_value arg;

  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $prsim_watch(prsim-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_watch(prsim-name)\n");
    return 0;
  }

  register_prsim_watchpoint (arg.value.str);

  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim.texi
@deffn Function $prsim obj
Loads HAC object file @var{obj} for @command{hacprsim} co-simulation.  
The object file need not be compiled through the allocation phase, 
the library will automatically compile it through the allocation phase 
for you if needed.  
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_file (PLI_BYTE8* args)
{
  STACKTRACE_VERBOSE;
  s_vpi_value arg;
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_printf ("Usage: $prsim(filename)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim(filename)\n");
    return 0;
  }
#if 0
  P = prs_fopen (arg.value.str);
#else
if (!check_object_loadable(arg.value.str).good) {
	// already have error message
	THROW_EXIT;	// abort
}
  HAC_module = load_module(arg.value.str);
if (HAC_module) {
	if (!HAC_module->is_allocated()) {
		if (!HAC_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
	}
	HAC_module->populate_top_footprint_frame();
	prsim_state = count_ptr<State>(
		new State(*HAC_module, ExprAllocFlags()));
	prsim_state->initialize();
}
#endif

  return prsim_state ? 1 : 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_mkrandom.texi
@deffn Function $prsim_mkrandom v
For @var{v} 1, synonymous with @command{$prsim_cmd("timing random");}.
For @var{v} 0, synonymous with @command{$prsim_cmd("timing after");}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_random (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  s_vpi_value arg;

  require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_printf ("Usage: $prsim_mkrandom(1 or 0)\n");
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_mkrandom(1 or 0)\n");
    return 0;
  }

  if (arg.value.integer == 0) {
#if 0
    P->flags &= ~PRS_RANDOM_TIMING;
#else
	prsim_state->norandom();
#endif
  }
  else {
#if 0
    P->flags |= PRS_RANDOM_TIMING;
#else
	prsim_state->randomize();
#endif
  }
  return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_resetmode.texi
@deffn Function $prsim_resetmode v
For @var{v} 1, synonymous with @command{$prsim_cmd("mode reset");}.
For @var{v} 0, synonymous with @command{$prsim_cmd("mode run");}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_resetmode (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  s_vpi_value arg;

  require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_printf ("Usage: $prsim_resetmode(1 or 0)\n");
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_resetmode(1 or 0)\n");
    return 0;
  }

  if (arg.value.integer == 0) {
#if 0
    P->flags &= ~PRS_NO_WEAK_INTERFERENCE;
#else
    prsim_state->set_mode_run();
#endif
  }
  else {
#if 0
    P->flags |= PRS_NO_WEAK_INTERFERENCE;
#else
    prsim_state->set_mode_reset();
#endif
  }
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_status_x.texi
@deffn Function $prsim_status_x
Synonymous with @command{$prsim_cmd("status X");}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_status_x (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle fname;
  s_vpi_value arg;

#if 0
  if (!prsim_state) {
    fatal_error ("Call prsim_status only after an object file has been loaded");
  }
#else
  require_prsim_state(__FUNCTION__);
#endif

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  fname = vpi_scan (h);
  if (!fname) {
    vpi_printf ("Usage: $prsim_status(1 or 0 or U)\n");
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_status(1 or 0 or U)\n");
    return 0;
  }

#if 0
  int v = PRS_VAL_X;
  //printf("U nodes:\n");
  prs_apply (P, (void*)v, check_nodeval);
  prs_apply (P, (void*)NULL, clear_nodeflag);
  vpi_printf ("\n");

  //if (arg.value.integer == 0) {
    //P->flags &= ~PRS_NO_WEAK_INTERFERENCE;
  //}
  //else {
    //P->flags |= PRS_NO_WEAK_INTERFERENCE;
  //}
#else
	// arg.value.char? .str[0]?
	prsim_state->status_nodes(cout, LOGIC_OTHER, false);
#endif
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_set.texi
@deffn Function $prsim_set node val
Sets a @var{node} in @command{hacprsim} to @var{val}.
Synonymous with @command{$prsim_cmd("set node val");}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_set (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1;
  s_vpi_value arg;
  string arg1;
  vpiHandle fname;
  // PrsNode *n;
  value_enum val;
  require_prsim_state(__FUNCTION__);

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $prsim_set(prsim-name, val)\n");
    return 0;
  }

  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);	// umm, memory leak, Hello???
  arg1 = arg.value.str;

  fname = vpi_scan (h);
  if (!fname) {
    vpi_printf ("Usage: $prsim_set(prsim-name, val)\n");
    return 0;
  }

  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_set(prsim-name, val)\n");
    return 0;
  }

#if 0
  vpi_printf ("prsim set %s %d\n", arg1, arg.value.integer);
#endif
	// can't set X, huh?
  if (arg.value.integer == 0) {
          val = LOGIC_LOW;
  } else {
          val = LOGIC_HIGH;
  }
#if 0
  n = prs_node (P, arg1);
  prs_set_node (P, n, val);
#else
  // Time_t vcstime;
  // vcs_to_prstime(p->time, &vcstime);
	const node_index_type ni = lookup_prsim_name(arg1);
	prsim_state->set_node(ni, val, false);
	// error status?
#endif

  //if (vpi_scan (h)) {
    //vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    //return 0;
  //}

  //register_to_prsim (arg1, arg.value.str);

  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_get.texi
@deffn Function $prsim_get node
Prints value of @var{node} in @command{hacprsim}.  
Synonymous with @command{$prsim_cmd("get node");}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_get (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1;
  s_vpi_value arg;
  string arg1;
  // vpiHandle fname;
  // PrsNode *n;
  // int val;

  require_prsim_state(__FUNCTION__);
  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $prsim_get(prsim-name)\n");
    return 0;
  }

  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);
  arg1 = arg.value.str;

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $prsim_set(prsim-name, val)\n");
    return 0;
  }

#if 0
  vpi_printf ("prsim set %s %d\n", arg1, arg.value.integer);
#endif

#if 0
  n = prs_node (P, arg1);
  vpi_printf ("%s: %c\n", arg1.c_str(), prs_nodechar (prs_nodeval (n)));
#else
	const node_index_type ni = lookup_prsim_name(arg1);
	print_watched_node(cout, *prsim_state, ni, arg1);
#endif

  //if (vpi_scan (h)) {
    //vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    //return 0;
  //}

  //register_to_prsim (arg1, arg.value.str);

  return 1;
}

//=============================================================================
struct funcs {
  char *name;
  PLI_INT32 (*f) (PLI_BYTE8 *);
};

static struct funcs f[] = {
  { "$to_prsim", to_prsim },
  { "$from_prsim", from_prsim },
  { "$prsim", prsim_file },
  { "$prsim_cmd", prsim_cmd },		// one command to rule them all
  { "$prsim_sync", prsim_sync },
	// these other commands are not needed, only for convenience
  { "$prsim_status_x", prsim_status_x },
  { "$prsim_set", prsim_set },
  { "$prsim_get", prsim_get },
  // { "$packprsim", prsim_packfile },
  { "$prsim_mkrandom", prsim_random },
  { "$prsim_resetmode", prsim_resetmode },
  { "$prsim_watch", prsim_watch }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
  Register prsim tasks
*/
static
void register_prsim (void)
{
  STACKTRACE_VERBOSE;
  s_vpi_systf_data s;
  size_t i;

  /* register tasks */
  for (i=0; i < sizeof (f)/sizeof (f[0]); ++i) {
    s.type = vpiSysTask;
    s.tfname = f[i].name;
    s.calltf = f[i].f;
    s.compiletf = NULL;
    s.sizetf = NULL;
    s.user_data = NULL;
    vpi_register_systf (&s);
  }
}

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

//=============================================================================
BEGIN_C_DECLS
extern void (*vlog_startup_routines[]) (void);

void (*vlog_startup_routines[]) (void) =
{
  &HAC::SIM::PRSIM::register_prsim,
  NULL
};

END_C_DECLS

DEFAULT_STATIC_TRACE_END

