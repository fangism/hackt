/**
	\file "sim/prsim/vpi-prsim.cc"
	$Id: vpi-prsim.cc,v 1.29 2011/01/11 01:13:25 fang Exp $
	Thanks to Rajit for figuring out how to do this and providing
	a reference implementation, which was yanked from:
 */
/*************************************************************************
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
#include <cstdarg>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>			// for remove_copy_if
#include "sim/prsim/State-prsim.h"
#include "sim/prsim/Command-prsim-export.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "sim/prsim/Command-prsim.h"
#include "parser/instref.h"
#include "main/main_funcs.h"
#include "main/prsim.h"
#include "main/prsim_options.h"
#include "util/numformat.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/string.h"
#include "util/tokenize.h"		// only for debugging
#include "util/inttypes.h"		// for 64b int type
#include "util/c_decl.h"
#include "util/stacktrace.h"
#include "util/utypes.h"

#ifndef	WIN32
#define	WIN32	0
#endif

// from either included sim/vpi_user.h or external: $(VPI_INCLUDE)
#include "vpi_user.h"

// use 64b time if there is 64b native integer available (some 32b platforms)
#ifdef	HAVE_UINT64_TYPE	// from "util/inttypes.h"
#define	TIME_64
#endif

/**
	Debugging switches
 */
#define	VERBOSE_DEBUG		(0 || DEBUG_MAX)
#define	TRACE_VCS_TIME		(0 || DEBUG_MAX)

#if TRACE_VCS_TIME
#define	SHOW_VCS_TIME(str, x)	format_time(cout << str) << x << endl
#else
#define	SHOW_VCS_TIME(str, x)
#endif

/**
	Define to 1 to call $finish instead of throwing an uncaught exception.
	Goal: 1
	Rationale: want clean exit to produce valid trace files, even on error.
	Status: well-tested, can perm
 */
#define	NICE_FINISH			1

/**
	Define to 1 to enforce a zero transport delay going from
	prsim to vcs, instead of calculating the time difference via
	floating-point subtraction.
	Rationale: floating-point subtraction of near-zero values
		sometimes yields garbage value?
	Status: tested, can perm
 */
#define	FORCE_ZERO_EXPORT_DELAY		1

/**
	Define to 1 to set nodes coming from vcs immediately, 
	without inserting into event queue.
	Rationale: simpler event handling
	Goal: 1
	Status: drafted, tested, including large test cases
		Fixed __advance_prsim to trigger multiple breakpoints, 
		not just the first one.
 */
#define	VPI_SET_NODE_IMMEDIATE		1

/**
	Define to 1 to set callback farther ahead than 1 time unit,
	to minimize the number of callbacks during idle times.
	Rationale: performance
	Goal: 1
	Status: tested, including large cosim,
		also enabled by __advance_prsim fix.
 */
#define	OPTIMISTIC_CALLBACK_SCHEDULING		1

//=============================================================================
namespace HAC {
namespace SIM {
namespace PRSIM {
using std::set;
using std::ostringstream;
#include "util/using_ostream.h"
using parser::parse_node_to_index;
using util::memory::count_ptr;
using util::strings::eat_whitespace;
// is current double-precision floating-point
typedef	State::time_type		Time_t;
typedef	State::node_type		node_type;

typedef	set<vpiHandle>			vpiHandleSetType;
/**
	Spare map for keeping track of vpiHandles per node.
	Could use a hash_map too, if we cared...
 */
typedef	std::map<node_index_type, set<vpiHandle> >	vpiHandleMapType;

//=============================================================================
static vpiHandleMapType			vpiHandleMap;

// simulator state
#if 0
static Prs *P = NULL;
#else
static count_ptr<module> HAC_module(NULL);
static count_ptr<State>	prsim_state(NULL);
static prsim_options prsim_opt;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only need to call this upon abrupt termination.
 */
static void __destroy_globals(void) {
	prsim_state = count_ptr<State>();	// destroy, auto-checkpoint
	HAC_module = count_ptr<module>();	// destroy
}

static void _run_prsim (const Time_t& vcstime, const int context);
static void __register_self_callback_have_event(Time_t);

// for time formatting
static
inline
util::format_ostream_ref
format_time(ostream& o) {
	return util::format_ostream_ref(o, prsim_state->time_fmt);
}

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
  const uint64 t = uint64(*tm);		// careful: double -> long conversion
  p->high = (t >> 32) & 0xffffffffUL;
  p->low = t & 0xffffffffUL;
#else
  p->high = 0;
  p->low = uint64(*tm) & 0xffffffffUL;
#endif
}

// whether or not callback is currently registered
static int scheduled = 0;
// callback scheduled time, revelant if scheduled == 1
static Time_t scheduled_time = 0;

/*
  last scheduled _run_prsim_callback. We need to remove it!!! Insane! 
*/
static vpiHandle last_registered_callback;
static
void
remove_callback(void) {
	vpi_remove_cb (last_registered_callback);
	scheduled = 0;
}

/**
@texinfo vpi/prsim_confirm_connections.texi
@deffn Function $prsim_confirm_connections
When this is called before any @t{$to_prsim}/@t{$from_prsim} connections
are made, each connection will be verbosely confirmed in the output.
@end deffn
@end texinfo
***/
static int _confirm_connections = 0;
static PLI_INT32 confirm_connections (PLI_BYTE8 *args) {
	_confirm_connections = 1;
	return 0;
}

/**
	Wrapper around vpi_printf that casts away constness
	for an incorrect prototype.
 */
static
PLI_INT32
vpi_puts_c(const char* str) {
	return vpi_printf(const_cast<PLI_BYTE8*>(str));
}

/**
	Wrapper around vpi_printf that casts away constness
	for an incorrect prototype.
 */
static
PLI_INT32
vpi_printf_c(const char* str, ...) {
	va_list a;
	va_start(a, str);
	return vpi_printf(const_cast<PLI_BYTE8*>(str), a);
	va_end(a);
}

static
PLI_INT32
prsim_sync(PLI_BYTE8*);

static
PLI_INT32
__no_op__(PLI_BYTE8*) {
	return 1;
}

static
PLI_INT32
_vpi_finish(void);

static
void
deprecation_warning(const char* m) {
	vpi_puts_c("Warning, [deprecated]: ");
	vpi_puts_c(m);
	vpi_puts_c("\n");
}

static
bool
react_to_node_event(const State::step_return_type nr, const bool init);

/**
	Wrapper around set_node, depending on implementation.
	\throw step_exception
 */
static
void
prsim_set_node(const node_index_type n, const value_enum val,
		const Time_t vcstime, const bool set_force,
		const bool init) {
	STACKTRACE_BRIEF;
#if VPI_SET_NODE_IMMEDIATE
	// what if vcstime is > prsimtime?
	// make sure there are no events before vcstime, and fast-forward
	prsim_state->safe_fast_forward(vcstime);
	const State::step_return_type
		nr(prsim_state->set_node_immediately(n, val, set_force));
	react_to_node_event(nr, init);	// may connect back out to vcs!
#else
	// this just enqueues node instead of changing it
	prsim_state->set_node_time(n, val, vcstime, set_force);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// equivalent current time on verilog-side of simulation (host)
static s_vpi_time vpi_current_time;
static Time_t vpi_current_time_prs;

/**
	Cache the current time from the host simulator side.
 */
static
void
sync_vpi_time(void) {
	STACKTRACE_BRIEF;
//	vpi_current_time.type = vpiSimTime;	// do once only during init
	vpi_get_time(NULL, &vpi_current_time);
	vcs_to_prstime(&vpi_current_time, &vpi_current_time_prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// print/debug time queues
// static
static
void
vpi_dump_queue(void) {
	STACKTRACE_BRIEF;
	vpiHandle tQi, tQh;
	s_vpi_time timeInfo;
	timeInfo.type = vpiSimTime;
	// static const
	vpi_puts_c("VPI event queue contains times (hi:lo):\n");
	tQi = vpi_iterate(vpiTimeQueue, NULL);
	while ((tQh = vpi_scan(tQi))) {
		vpi_get_time(tQh, &timeInfo);
		vpi_printf_c("\t%d:%d\n", timeInfo.high, timeInfo.low);
		vpi_free_object(tQh);
	}
	vpi_puts_c("end of VPI event queue\n");
}

static
PLI_INT32
vpi_dump_queue_cmd(PLI_BYTE8*) {
	vpi_dump_queue();
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// verbose debugging switch
/**
@texinfo vpi/prsim_verbose_transport.texi
@deffn Function $prsim_verbose_transport arg
If @var{arg} is 1 (or non-zero), then every value that is transported
between verilog and @command{prsim} will be verbosely reported.
@end deffn
@end texinfo
***/
static bool _verbose_transport = false;
static PLI_INT32 verbose_transport (PLI_BYTE8 *args) {
  STACKTRACE_BRIEF;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle fname;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_verbose_transport(0|1)\n";

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);
  _verbose_transport = bool(arg.value.integer);

  if (vpi_scan (h)) {
	// excess arguments
    vpi_puts_c (usage);
    return 0;
  }

  return 1;
}

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
  const node_index_type n = parse_node_to_index(prsim_name, *HAC_module).index;
  if (!n) {
//  cerr << "Node `" << prsim_name << "\' not found in .prs file!" << endl;
    cerr << "Node `" << prsim_name << "\' not found in HAC hierarchy!" << endl;
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
	\param vcs_name hierarchical verilog name of object.
	\return handle to the referenced object
	\throw exception if object not found
 */
static
vpiHandle
lookup_vcs_name(const char* vcs_name) {
  const vpiHandle net =
	vpi_handle_by_name (const_cast<PLI_BYTE8*>(vcs_name), NULL);
  if (!net) {
//  cerr << "Net name `" << vcs_name << "' not found in .v file." << endl;
    cerr << "Net name `" << vcs_name << "' not found in Verilog!" << endl;
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
	This callback is due to time-elapsed.
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
  STACKTRACE_BRIEF;
  Time_t curtime;	// max time to run before turning control to vcs.
	// p->time depends on the callback data reason
	// TODO: really want to know the time of next vcs event
  vcs_to_prstime (p->time, &curtime);
	// curtime is also the current VCS simulator time
	sync_vpi_time();	// always update sim time right away
#if VERBOSE_DEBUG
if (_verbose_transport) {
	format_time(cout << "vcs.current_time: ") << vpi_current_time_prs << endl;
	format_time(cout << "prsim.current_time: ") << prsim_state->time() << endl;
	format_time(cout << "_run_prsim_callback: until ") << curtime << endl;
}
#endif
  remove_callback();
  _run_prsim (curtime, 1);
#if VERBOSE_DEBUG
if (_verbose_transport) {
	format_time(cout << "_run_prsim_callback: return at prsim time ")
		<< prsim_state->time() << endl;
	vpi_dump_queue();	// always empty?
}
#endif
// TODO: return something, but what?
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register with VCS the longest time to advance until before
	calling back to prsim to handle its events.  
	May actually return earlier on events on registered nodes.  
	\param vcstime time at which to call back (at the latest)
	\pre this is called only if there are pending events.
 */
static void register_self_callback (Time_t vcstime)
{
  STACKTRACE_BRIEF;
  require_prsim_state(__FUNCTION__);
//  SHOW_VCS_TIME("callback vcstime: ", vcstime);
  
  if (scheduled == 1) {
#if VERBOSE_DEBUG
    if (_verbose_transport) {
	format_time(cout << "register_self_callback: vcstime: ") << vcstime;
	format_time(cout << ", scheduled: " ) << scheduled_time << endl;
    }
#endif
	if (vcstime < scheduled_time) {
#if VERBOSE_DEBUG
		if (_verbose_transport) {
			cout << "  replacing old callback." << endl;
		}
#endif
		// then newly scheduled time is earlier
		// replace previous callback
		remove_callback();
	} else {
#if VERBOSE_DEBUG
		if (_verbose_transport) {
			cout << "  keep previous callback." << endl;
		}
#endif
		// previous scheduled callback is already earlier, keep it
		return;
	}
  } else {
#if VERBOSE_DEBUG
	if (_verbose_transport) {
		cout << "  not yet scheduled." << endl;
	}
#endif
	STACKTRACE_INDENT_PRINT("not scheduled" << endl);
  }
#if 0 && VERBOSE_DEBUG
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
#if 0
  if (heap_peek_min (P->eventQueue) == NULL)
  {
    /* I have nothing to do, go away */
    return;
  }
#else
  // assured that this is called only if there is pending event at vcstime
#endif
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
	STACKTRACE_BRIEF;
/** Single instance of callback data passed to VCS and re-used.  */
static s_cb_data cb_data;
/* at most one callback pending, let's not malloc!  */
static s_vpi_time tm;
	INVARIANT(!scheduled);
  cb_data.reason = cbAfterDelay;	// relative to now, in future
  cb_data.cb_rtn = _run_prsim_callback;
  cb_data.obj = NULL;

  cb_data.time = &tm;
  scheduled_time = vcstime;
#if VERBOSE_DEBUG
if (_verbose_transport) {
	format_time(cout << "__register_self...: re-scheduled time: ")
		<< scheduled_time << endl;
}
#endif
#if OPTIMISTIC_CALLBACK_SCHEDULING
  vcstime -= vpi_current_time_prs;
#else
#if 0
  if (heap_peek_minkey (P->eventQueue) > vcstime)
#else
  const Time_t next_time = prsim_state->next_event_time();
	STACKTRACE_INDENT_PRINT("next_time: " << next_time << endl);
  INVARIANT(next_time == vcstime);
  if (next_time > vcstime)
#endif
  {
	STACKTRACE_INDENT_PRINT("next_time > vcstime" << endl);
#if 0
    vcstime = heap_peek_minkey (P->eventQueue) - vcstime;
#else
    vcstime = next_time - vcstime;
#endif
  }
  else {
	// always in this case!
	STACKTRACE_INDENT_PRINT("next_time <= vcstime" << endl);
    vcstime = 1;
	// polling every single time unit!?  expensive and unnecessary
  }
#endif
  SHOW_VCS_TIME("advance vcstime: ", vcstime);
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
static
// inline
value_enum
vpi_to_prsim_value(const PLI_INT32& v) {
  value_enum n;
  switch(v) {
	case vpi0:
	case vpiL:
		n = LOGIC_LOW;
		break;
	case vpi1:
	case vpiH:
		n = LOGIC_HIGH;
		break;
	default:	// vpiX, vpiZ
		n = LOGIC_OTHER;
		break;
  }
  return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
// inline
PLI_INT32
prsim_to_vpi_value(const value_enum n) {
	PLI_INT32 ret;
	switch (n) {
	case LOGIC_HIGH:
		ret = vpi1;
		break;
	case LOGIC_LOW:
		ret = vpi0;
		break;
	case LOGIC_OTHER:
		ret = vpiX;
		break;
	default:
		ret = vpiX;
		DIE;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the current value of the verilog node using prsim enum.
 */
static
value_enum
get_prsim_value(const vpiHandle& net) {
  s_vpi_value v;
  v.format = vpiScalarVal;
  vpi_get_value(net, &v);
  return vpi_to_prsim_value(v.value.scalar);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the current value of the node.
 */
static
void
report_transport_from_prsim(const node_index_type ni) {
	const string name(prsim_state->get_node_canonical_name(ni));
	format_time(cout << "[to vcs]   signal " << name << " changed @ time ")
		<< vpi_current_time_prs << ", val = ";
	prsim_state->get_node(ni).dump_value(cout) << '\n';
	// not endl, minimize flushing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the current value of the node.
	\param v new value of node.
 */
static
void
report_transport_to_prsim(const node_index_type ni, const value_enum v) {
	const string name(prsim_state->get_node_canonical_name(ni));
	format_time(cout << "[from vcs] signal " << name << " changed @ time ")
		<< vpi_current_time_prs << ", val = " <<
		node_type::translate_value_enum_to_char(v) << '\n';
//	prsim_state->get_node(ni).dump_value(cout) << '\n';
	// not endl, minimize flushing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Schedules a value update from prsim back to verilog-land.
 */
static
void
transport_value_from_prsim(const node_type& n, 
		const vpiHandle& net, 
		s_vpi_time& tm) {
	s_vpi_value v;
	v.format = vpiScalarVal;
	INVARIANT(net);
	const value_enum pval = n.current_value();
	v.value.scalar = prsim_to_vpi_value(pval);
	if (_verbose_transport) {
		report_transport_from_prsim(prsim_state->get_node_index(n));
	}
	// is temporary allocation really necessary? avoidable?
	// vpiPureTransportDelay: affects no other events
	// vpiNoDelay (used by Fang): take effect immediately, ignore time
	// vpiNoDelay seems to be needed when multiple breakpoint events
	// are processed before returning to VCS, as was introduced
	// by $prsim_sync.
	// vpi_free_object(vpi_put_value(net, &v, &tm, vpiPureTransportDelay));
	vpi_free_object (vpi_put_value (net, &v, &tm, vpiNoDelay));
	// Q: shouldn't control return immediately to VCS?
	// experimenting shows that this makes no difference!? both work
	// WHY?
}	// end transport_value_from_prsim

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like the above, but only conditionally schedules event if
	values do not match.
 */
static
void
initial_transport_value_from_prsim(const node_type& n, const vpiHandle& net) {
	s_vpi_value v;
	v.format = vpiScalarVal;
	INVARIANT(net);
	const value_enum cv(n.current_value());
	const value_enum vv(get_prsim_value(net));
if (cv != vv) {
	v.value.scalar = prsim_to_vpi_value(cv);
//	sync_vpi_time();	// is this really necessary? assume init time?
	if (_verbose_transport) {
		report_transport_from_prsim(prsim_state->get_node_index(n));
	}
	vpi_free_object (vpi_put_value (net, &v, &vpi_current_time, vpiNoDelay));
}
	// otherwise, don't bother -- values already match
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handles node transition and possible transport back to vcs
	Also handles printing of watched nodes.
	if it is a registered breakpoint.
	\return true if hit a breakpoint.
 */
bool
react_to_node_event(const State::step_return_type nr, const bool init) {
	const node_index_type ni = GET_NODE(nr);
	const node_type& n(prsim_state->get_node(ni));
	// const node_index_type m = GET_CAUSE(nr);
	const vpiHandleMapType::const_iterator
		n_space(vpiHandleMap.find(ni)),
		n_end(vpiHandleMap.end());
	// "n_space" in honor of the abuse of a certain void* PrsNode::*space
	const Time_t prsim_time = prsim_state->time();
	const value_enum val = n.current_value();
	if ((!init || (val != LOGIC_OTHER)) &&
		(prsim_state->watching_all_nodes()
#if USE_WATCHPOINT_FLAG
			|| prsim_state->is_watching_node(GET_NODE(nr))
#endif
			)) {
		format_time(cout << "prsim:\t") << prsim_time << '\t';
		print_watched_node(cout, *prsim_state, nr);
	}
    if (n.is_breakpoint()) {
#if !USE_WATCHPOINT_FLAG
	if (prsim_state->is_watching_node(GET_NODE(nr)) &&
			!prsim_state->watching_all_nodes()) {
		format_time(cout << "prsim:\t") << prsim_time << '\t';
		print_watched_node(cout, *prsim_state, nr);
	}
#endif
    if (n_space != n_end) {
	STACKTRACE("breakpt && registered");
      s_vpi_time tm;

      tm.type = vpiSimTime;
#if FORCE_ZERO_EXPORT_DELAY
      tm.high = 0;
      tm.low = 0;
#else
	// floating-point subtraction, sometimes problematic?
      const Time_t prsdiff = prsim_time - vcstime;
      prs_to_vcstime (&tm, &prsdiff);
#endif
      /* aha, schedule an event into the vcs queue */
      const vpiHandleSetType& net_set(n_space->second);
      vpiHandleSetType::const_iterator
	net_iter(net_set.begin()), net_end(net_set.end());
      INVARIANT(net_iter != net_end);	// must be at least 1
for ( ; net_iter != net_end; ++net_iter) {
      const vpiHandle& net(*net_iter);
	transport_value_from_prsim(n, net, tm);
}	// end for each fanout to VPI
	return true;
    }
	// is not registered $from_prsim
    }	// end if is_breakpoint
	return false;
}

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
  STACKTRACE_BRIEF;
  State::step_return_type nr;
  SHOW_VCS_TIME("limit vcstime: ", vcstime);

  /* run for at most 1ps */
#if 0
  while ((heap_peek_minkey (P->eventQueue) <= vcstime) 
	 && (n = prs_step_cause (P, &m, &seu)))
#else
  Time_t next_time;
  Time_t break_time = vcstime;		// may decrease only
  while (
#if PRSIM_AGGREGATE_EXCEPTIONS
	!prsim_state->is_fatal() &&
	// not is_stopped_or_fatal(), results in infinite loop...
#endif
	prsim_state->pending_events() &&
	(next_time = prsim_state->next_event_time(), 1) &&
	(next_time <= vcstime) &&
	(next_time <= break_time) && 
	GET_NODE((nr = prsim_state->step())))
#endif
  {
#if 0 && VERBOSE_DEBUG
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
#if TRACE_VCS_TIME
if (_verbose_transport) {
	if (prsim_state->pending_events()) {
	format_time(cout << "__advance_prsim: this event time: ")
		<< next_time << endl;
	}
	format_time(cout << "__advance_prsim: prsim time: ")
		<< prsim_state->time() << endl;
}
#endif
	// aggregate multiple break events (to vcs) that happen at same time
	if (react_to_node_event(nr, false)) {
		break_time = prsim_state->time();
	}
  }	// end while
}
// end __advance_prsim

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-throw version of the above.
	Caught exceptions wile nicely terminate the co-simulation.
	This is not really relevant, since we changed the
	error-handling mechanism to no longer throw.
	\param vcstime the maximum time to run before turning control to vcs.
 */
static void __advance_prsim_nothrow (const Time_t& vcstime, const int context)
{
	STACKTRACE_BRIEF;
#if VERBOSE_DEBUG
if (_verbose_transport) {
	format_time(cout << "Running prsim until ") << vcstime << endl;
#if 0
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
}
#endif
try {
	__advance_prsim(vcstime, context);	// may throw
#if PRSIM_AGGREGATE_EXCEPTIONS
	if (prsim_state->is_fatal()) {
		prsim_state->inspect_exceptions();
#else
} catch (const step_exception& exex) {
	exex.inspect(*prsim_state, cerr);	// ignore return code?
#endif
	// no need to translate error_policy_to_status
#if NICE_FINISH
	cerr << "Terminating simulation early due to hacprsim exception."
		<< endl;
	_vpi_finish();
#else
	__destroy_globals();
	THROW_EXIT;	// re-throw
#endif
#if PRSIM_AGGREGATE_EXCEPTIONS
	}	// end if is_fatal
#endif
} catch (...) {
	// catch all remaining exceptions here, destroy globals and rethrow
	cerr << "hacprsim (VPI) encountered error, terminating..." << endl;
#if NICE_FINISH
	_vpi_finish();
#else
	__destroy_globals();
	throw;		// re-throw
#endif
}
}

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
	STACKTRACE_BRIEF;
	if (prsim_state->pending_events()) {
		// re-schedule callback if there are events left
		const Time_t next = prsim_state->next_event_time();
//		format_time(cout << "next pending: ") << next << endl;
		if (next != SIM::delay_policy<Time_t>::max()) {
			register_self_callback(next);
			return;
		}
	}
// else
	{
		// otherwise, no events in queue, only wait for $to_prsim events
		// don't bother scheduling timed callback
		// it is safe to update prsim's current time 
		// if and only if there are no pending events
//		sync_vpi_time();	// is this really necessary?
		prsim_state->safe_fast_forward(vpi_current_time_prs);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PLI_INT32
_vpi_finish(void) {
	// print this finish timestamp out of convention
	// format_time(...) ?
	cerr << "$finish at simulation time (hacprsim) " <<
		prsim_state->time() << endl;
	__destroy_globals();
	return vpi_control(vpiFinish, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Runs prsim until a max time or first reached breakpoint, 
	and reschedules callback when done (if there are events remaining).
	\param vcstime the maximum time to run before turning control to vcs.
 */
static void _run_prsim (const Time_t& vcstime, const int context)
{
	STACKTRACE_BRIEF;
	__advance_prsim_nothrow(vcstime, context);
	reregister_next_callback();
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

Update: this command is now deprecated because now all @command{$prsim_cmd}
calls automatically re-synchronize the event queues, as a conservative measure.
@end deffn
@end texinfo
 */

static
void
prsim_catch_up(void) {
	STACKTRACE_BRIEF;
	require_prsim_state(__FUNCTION__);
	sync_vpi_time();
	const vpiHandle queue = vpi_handle(vpiTimeQueue, NULL);
	s_vpi_time queue_time;
	queue_time.type = vpiSimTime;
	vpi_get_time(queue, &queue_time);
	Time_t pq_time;
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
	vpi_free_object(queue);	// vpi_get_time() allocates a s_vpi_time object
	if (prsim_state->pending_events()) {
		if (scheduled) {
			// unschedule and reschedule
			remove_callback();
		}
		// "catch-up" prsim to VCS's time, 
		// even past breakpoints
		do {
			__advance_prsim_nothrow(pq_time, 0);	// context?
#if 0
			cout << "prsim advanced to time: " <<
				prsim_state->time() << endl;
#endif
		} while (prsim_state->pending_events() &&
			(prsim_state->next_event_time() <= vpi_current_time_prs));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PLI_INT32
prsim_sync(PLI_BYTE8*) {
	STACKTRACE_BRIEF;
	prsim_catch_up();
	// already calls sync_vpi_time()
	reregister_next_callback();
#if 0
	cout << "prsim caught up to time: " << prsim_state->time() << endl;
#endif
	// else no-op, leave alone
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called when a signal transition is transported from
	vcs to prsim, from $to_prsim();
	\param p callback data.
 */
static
PLI_INT32 prsim_callback (s_cb_data *p)
{
  STACKTRACE_BRIEF;
  sync_vpi_time();
  // PrsNode *n;
  Time_t vcstime;	// , prsdiff;
  // int bpcount = 0;
  // int gap;

  const value_enum val = vpi_to_prsim_value(p->value->value.scalar);
// TODO: use template type_traits on sizeof
#if (SIZEOF_SIZE_T == SIZEOF_VOIDP)
  const node_index_type n = reinterpret_cast<node_index_type>(p->user_data);
  // n = (PrsNode *)p->user_data;
#else
#error "node_index_type cannot fit inside a void* (size mismatch)!"
#endif

#if 0
  vpi_printf ("signal %s changed @ time %d, val = %d\n",
	      prs_nodename (n),
	      p->time->low,
	      p->value->value.scalar);
  /* convert my time to prsim time */
#ifdef TIME_64
  vcstime = ((unsigned long long)p->time->high) << 32 | ((unsigned long long)p->time->low);
#else
  vcstime = p->time->low;
#endif
#else
  vcs_to_prstime(p->time, &vcstime);
if (_verbose_transport) {
	report_transport_to_prsim(n, val);
}
#endif

/**
	Whether or not set events from VPI are considered forced.
 */
static const bool set_force = true;
try {
	prsim_set_node(n, val, vcstime, set_force, false);
	// optional: execute this most recent event now
	// bypass event queue!

	// is this reregister optional because _run_prsim also calls reregister?
	// this is required because new event may be vacuous,
	// but still needs to be registered.
	reregister_next_callback();
#if VERBOSE_DEBUG
    if (_verbose_transport) {
	const Time_t next_time = prsim_state->next_event_time();
	format_time(cout << "prsim_callback: next_time: ")
		<< next_time;	// could be time_type<Time_t>::max()
	format_time(cout << ", scheduled_time: ")
		<< scheduled_time << endl;
    }
#endif
} catch (...) {
	// possible exception with scheduling events in past
	_vpi_finish();
}
#if VERBOSE_DEBUG && 0
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif
#if VPI_SET_NODE_IMMEDIATE
// then node value change was executed immediately, bypassing event queue
// so no need to run
#else
// alternative: if receiving transport delay is zero, 
// could force-execute this event immediately.
// with any arbitrary transport delay, let event queue pick next event.
  _run_prsim (vcstime, 0);
#endif
// TODO: don't know what should be returned, was missing/void before
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This helps remove extraneous whitespace that may be the result
	of padded string concatenation in verilog;
	Helper function for stripping whitespaces anywhere in string, 
	not just leading whitespace. 
	NOTE: escaped identifiers need a trailing space preserved
 */
static
string
strip_spaces(const char* c) {
	STACKTRACE_BRIEF;
#if 0
	// insufficient
	return string(eat_whitespace(c));
#else
	const string temp(c);
	string ret;
//	ret.reserve(temp.length());		// optional
#if 0
	// too aggressive
	std::remove_copy_if(temp.begin(), temp.end(), back_inserter(ret), 
		isspace);
#else
	bool in_escape = false;
	string::const_iterator i(temp.begin()), e(temp.end());
	for ( ; i!=e; ++i) {
		switch (*i) {
		case ' ':
			if (in_escape) {
				// keep escape-terminating trailing space
				ret.push_back(*i);
				in_escape = false;
			} // else strip all other spaces
			break;
		case '\\': in_escape = true;	// fall-through
		default: ret.push_back(*i); break;
		}
	}
#endif
	STACKTRACE_INDENT_PRINT("original: \"" << c << "\"" << endl);
	STACKTRACE_INDENT_PRINT("stripped: \"" << ret << "\"" << endl);
	return ret;
#endif
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
This command should be invoked prior to any events in simulation.
@end deffn
@end texinfo
*/
static
void register_to_prsim (const char *vcs_name, const char *prsim_name)
{
  STACKTRACE_BRIEF;
  s_cb_data cb_data;
  // PrsNode *n;
  
  require_prsim_state(__FUNCTION__);

  const string VCS_name(strip_spaces(vcs_name));
  const string PRSIM_name(strip_spaces(prsim_name));
  // string vcs_name_stripped, prsim_name_stripped;

  cb_data.reason = cbValueChange;
  cb_data.cb_rtn = prsim_callback;
  const vpiHandle net = lookup_vcs_name(VCS_name.c_str());
  cb_data.obj = net;

	// TODO: who frees these mallocs???
	// these are setup one-time upon connection
	// ideally, free them upon finish
	// TODO: is this the right time type?
  cb_data.time = p_vpi_time(malloc (sizeof(s_vpi_time)));
  cb_data.time->type = vpiSimTime;

  cb_data.value = p_vpi_value(malloc (sizeof(s_vpi_value)));
  cb_data.value->format = vpiScalarVal;

#if 0
  n = prs_node (P, prsim_name);
#else
  const node_index_type ni = lookup_prsim_name(PRSIM_name.c_str());
#endif
  
  /* prsim net name */
  cb_data.user_data = reinterpret_cast<PLI_BYTE8*>(ni);	// YUCK, void*
  vpi_register_cb (&cb_data);
	// ignore return value of registered callback?
	// might be useful in future if we ever decide to 'disconnect'

  if (_confirm_connections) {
	cout << "$to_prsim: " << VCS_name << " -> " << PRSIM_name << endl;
  }
  if (prsim_state->node_is_driven(ni)) {
	cout << "$to_prsim: WARNING: node `" << PRSIM_name <<
		"' is driven from Verilog (" << VCS_name <<
		") and production rules!" << endl;
	// caveat: script might connect node to channel-source/env later
  }

  /* propagate the current value of the node to prsim */
  const value_enum v(get_prsim_value(net));
  // cout << "VALUE of " << prsim_name << " is " << size_t(n) << endl;
//  sync_vpi_time();	// is this really necessary? or assume init time?
  prsim_set_node(ni, v, vpi_current_time_prs, false, true);	// force?
	// may throw exception, b/c executes immediately
  prsim_sync(NULL);		// flush events
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
  STACKTRACE_BRIEF;
  // PrsNode *n;
  
  require_prsim_state(__FUNCTION__);

  const string VCS_name(strip_spaces(vcs_name));
  const string PRSIM_name(strip_spaces(prsim_name));

  /* this is the handle to the verilog net name */
  const vpiHandle net = lookup_vcs_name(VCS_name.c_str());
  const node_index_type ni = lookup_prsim_name(PRSIM_name.c_str());
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
	vpiHandleMap[ni].insert(net);
	// support multiple unique Handles
#endif

  if (_confirm_connections) {
	cout << "$from_prsim: " << PRSIM_name << " -> " << VCS_name << endl;
  }

/* propagate current prsim value to verilog, if different */
	initial_transport_value_from_prsim(prsim_state->get_node(ni), net);

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
  STACKTRACE_BRIEF;
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

/**
	This just processes the function arguments and passes them
	on to the underlying function.  
 */
static PLI_INT32 to_prsim (PLI_BYTE8 *args)
{
  STACKTRACE_BRIEF;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1, net2;
  s_vpi_value arg;
  string arg1;
  static const char usage[] = "Usage: $to_prsim(vcs-name, prsim-name)\n";

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);	// memory leak?
  arg1 = arg.value.str;

  net2 = vpi_scan (h);
  if (!net2) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net2, &arg);

#if VERBOSE_DEBUG
  vpi_printf_c ("setup %s (vcs) -> %s (prsim)\n", arg1.c_str(), arg.value.str);
#endif

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
    return 0;
  }

  register_to_prsim (arg1.c_str(), arg.value.str);
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static PLI_INT32 from_prsim (PLI_BYTE8 *args)
{
  STACKTRACE_BRIEF;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1, net2;
  s_vpi_value arg;
  string arg1;
  static const char usage[] = "Usage: $from_prsim(prsim-name, vcs-name)\n";

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);
  arg1 = arg.value.str;

  net2 = vpi_scan (h);
  if (!net2) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net2, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
  STACKTRACE_BRIEF;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_cmd(prsim-command)\n";
require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
    return 0;
  }

	prsim_sync(NULL);
  switch (CommandRegistry::interpret_line (*prsim_state, arg.value.str)) {
  case command_error_codes::SYNTAX:	// fall-through
  case command_error_codes::BADARG:
	cerr << "Treating command errors (syntax, bad-arg) as fatal."
		<< endl;
	// fall-through
  case command_error_codes::FATAL:
#if NICE_FINISH
	cerr << "Terminating simulation early due to hacprsim fatal error."
		<< endl;
	_vpi_finish();
#else
	__destroy_globals();
	THROW_EXIT;	// abort
#endif
  case command_error_codes::NORMAL:
  case command_error_codes::END:
	// conservatively, any command might alter event queue
	prsim_sync(NULL);
	return 1;
  default:
	// return on first error
	prsim_sync(NULL);
	return 0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is really superceded by $prsim_cmd("watch ...");
 */
static PLI_INT32 prsim_watch (PLI_BYTE8* args)
{
  STACKTRACE_BRIEF;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_watch(prsim-name)\n";

  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
    return 0;
  }

  register_prsim_watchpoint (arg.value.str);

  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_options.texi
@deffn Function $prsim_options optstring
Sets the command-line options to be used for @command{hacprsim} co-simulation.  
This should be done before the call to @command{$prsim()}.
@end deffn
@end texinfo
***/
static PLI_INT32 prsim_command_options (PLI_BYTE8* args)
{
  STACKTRACE_BRIEF;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_options(\"options\")\n";
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
    return 0;
  }
  // split up arg.value.str into argc,argv
  util::string_list toks;
  util::tokenize(arg.value.str, toks);
  toks.push_front("vpi-prsim");
  const int argc = toks.size();
  vector<const char*> argv;
  argv.reserve(argc);
  transform(toks.begin(), toks.end(), back_inserter(argv), 
	std::mem_fun_ref(&string::c_str));
  if (prsim::parse_command_options(argc, const_cast<char**>(&argv[0]),
	prsim_opt)) {
	cerr << "Error in command arguments." << endl;
	prsim::usage();
	return 0;
  }
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
  STACKTRACE_BRIEF;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim(filename)\n";
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
	if (!HAC_module->allocate_unique().good) {
		cerr << "ERROR in allocating.  Aborting." << endl;
		return 1;	// 0?
	}
	prsim_state = count_ptr<State>(
		new State(*HAC_module, prsim_opt.expr_alloc_flags));
	// grab paths from command-line options
	prsim_state->import_source_paths(prsim_opt.source_paths);
	prsim_state->initialize();
	// do not just run over vacuous events
	prsim_state->stop_on_vacuous_events();
	if (prsim_opt.autosave) {
		prsim_state->autosave(prsim_opt.autosave,
			prsim_opt.autosave_name);
	}
	if (prsim_opt.autotrace) {
		prsim_state->open_trace(prsim_opt.autotrace_name);
	}
	// forbid step/advance/cycle commands
	CommandRegistry::external_cosimulation = true;
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
  STACKTRACE_BRIEF;
  deprecation_warning(
	"$prsim_mkrandom() should be replaced with $prsim_cmd(\"timing random\")");
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_mkrandom(1 or 0)\n";

  require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
  STACKTRACE_BRIEF;
  deprecation_warning(
	"$prsim_resetmode() should be replaced with $prsim_cmd(\"mode reset\")");
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_resetmode(1 or 0)\n";

  require_prsim_state(__FUNCTION__);
  const vpiHandle task_call = vpi_handle (vpiSysTfCall, NULL);
  const vpiHandle h = vpi_iterate (vpiArgument, task_call);
  const vpiHandle fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
  STACKTRACE_BRIEF;
  deprecation_warning(
	"$prsim_status_x() should be replaced with $prsim_cmd(\"status X\")");
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle fname;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_status(1 or 0 or U)\n";

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
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
	prsim_state->print_status_nodes(cout, LOGIC_OTHER, false);
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
  STACKTRACE_BRIEF;
  deprecation_warning(
	"$prsim_set() should be replaced with $prsim_cmd(\"set ...\")");
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1;
  s_vpi_value arg;
  string arg1;
  vpiHandle fname;
  // PrsNode *n;
  value_enum val;
  static const char usage[] = "Usage: $prsim_set(prsim-name, val)\n";
  require_prsim_state(__FUNCTION__);

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }

  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);	// umm, memory leak, Hello???
  arg1 = arg.value.str;

  fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }

  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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
	// reregister_next_callback();
	prsim_sync(NULL);
	// because this command alters the event queue
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
  STACKTRACE_BRIEF;
  deprecation_warning(
	"$prsim_get() should be replaced with $prsim_cmd(\"get ...\")");
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1;
  s_vpi_value arg;
  string arg1;
  // vpiHandle fname;
  // PrsNode *n;
  // int val;
  static const char usage[] = "Usage: $prsim_get(prsim-name)\n";

  require_prsim_state(__FUNCTION__);
  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_puts_c (usage);
    return 0;
  }

  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  // arg1 = strdup (arg.value.str);
  arg1 = arg.value.str;

  if (vpi_scan (h)) {
    vpi_puts_c (usage);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo vpi/prsim_default_after.texi
@deffn Function $prsim_default_after time
Set the default delay for unspecified rules to @var{time}, 
in @command{hacprsim}'s time units (unitless).
This is analogous to the command-line @option{-D time} option.  
This command should be invoked @emph{before} loading the 
object file (@command{$prsim()}) that initializes the state of the simulation.  
@end deffn
@end texinfo
***/
static
PLI_INT32
prsim_default_after(PLI_BYTE8 *args) {
  STACKTRACE_BRIEF;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle fname;
  s_vpi_value arg;
  static const char usage[] = "Usage: $prsim_default_after(<Int>)\n";

  // require_prsim_state(__FUNCTION__);		// doesn't require state

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  fname = vpi_scan (h);
  if (!fname) {
    vpi_puts_c (usage);
    return 0;
  }
  arg.format = vpiIntVal;
  vpi_get_value (fname, &arg);
  if (arg.value.integer < 0) {
    vpi_puts_c ("Error: delay value must be non-negative!");
    return 0;
  }
  State::rule_type::default_unspecified_delay = arg.value.integer;

  if (vpi_scan (h)) {
	// excess arguments
    vpi_puts_c (usage);
    return 0;
  }

  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/***
@texinfo vpi/prsim_cycle.texi
@deffn Function $prsim_cycle
Execute prsim events until event queue is empty.
@end deffn
@end texinfo
***/
static
PLI_INT32
prsim_cycle(PLI_BYTE8 *args) {
  STACKTRACE_BRIEF;
}
#endif

//=============================================================================
struct funcs {
  const char *name;
  PLI_INT32 (*f) (PLI_BYTE8 *);
};

static struct funcs f[] = {
  { "$to_prsim", to_prsim },
  { "$from_prsim", from_prsim },
  { "$prsim_options", prsim_command_options },
  { "$prsim", prsim_file },
  { "$prsim_default_after", prsim_default_after },
  { "$prsim_verbose_transport", verbose_transport },
  { "$prsim_confirm_connections", confirm_connections },
  { "$prsim_cmd", prsim_cmd },		// one command to rule them all
  { "$prsim_sync", __no_op__ },		// deprecated, should be automatic now
	// these other commands are not needed, only for convenience
  { "$prsim_status_x", prsim_status_x },	// deprecated
  { "$prsim_set", prsim_set },			// deprecated
  { "$prsim_get", prsim_get },			// deprecated
  // { "$packprsim", prsim_packfile },
  { "$prsim_mkrandom", prsim_random },		// deprecated
  { "$prsim_resetmode", prsim_resetmode },	// deprecated
  { "$prsim_watch", prsim_watch },
  { "$vpi_dump_queue", vpi_dump_queue_cmd }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
  Register prsim tasks
*/
static
void register_prsim (void)
{
  STACKTRACE_BRIEF;
	// do some one-time initializations here
	vpi_current_time.type = vpiSimTime;	// do once only
	sync_vpi_time();		// should just be 0

  s_vpi_systf_data s;
  size_t i;

  /* register tasks */
  for (i=0; i < sizeof (f)/sizeof (f[0]); ++i) {
    s.type = vpiSysTask;
    s.tfname = const_cast<PLI_BYTE8*>(f[i].name);	// pffft...
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

