/**
	\file "sim/prsim/vpi-prsim.cc"
	$Id: vpi-prsim.cc,v 1.1.2.2 2007/12/20 18:35:51 fang Exp $
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
using std::cout;
using std::cerr;
using std::endl;
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

static void _run_prsim (const Time_t& vcstime, const int context);

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
static
void
require_prsim_state(const char* caller) {
	if (!prsim_state) {
		cerr << "Call " << caller <<
			" only after a HAC module has been loaded." << endl;
		THROW_EXIT;
	}
}

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

//=============================================================================
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


static void register_self_callback (Time_t vcstime)
{
  STACKTRACE_VERBOSE;
  static s_cb_data cb_data;
  static s_vpi_time tm; /* at most one callback pending, let's not malloc! */
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

	STACKTRACE_INDENT_PRINT("have event to do" << endl);

  cb_data.reason = cbAfterDelay;
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

  cb_data.time->type = vpiSimTime;
  prs_to_vcstime (cb_data.time, &vcstime);

  cb_data.value = NULL;
  cb_data.user_data = NULL;
  last_registered_callback = vpi_register_cb (&cb_data);
  scheduled = 1;
}

/**
	What is context for?
	Should be like "advance-until" behavior.  
 */
static void _run_prsim (const Time_t& vcstime, const int context)
{
  STACKTRACE_VERBOSE;
  State::step_return_type nr;
  Time_t prsdiff;
  SHOW_VCS_TIME(vcstime);

#if VERBOSE_DEBUG
  cout << "Running prsim @ time " << vcstime << endl;
	prsim_state->dump_event_queue(cout);
	cout << "end of event queue." << endl;
#endif

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

      prsdiff = prsim_time - vcstime;

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
      case node_type::LOGIC_HIGH:
#if VERBOSE_DEBUG
	vpi_printf ("Set net %s (%x) to TRUE\n", nodename, net);
#endif
	v.value.scalar = vpi1;
	break;
      case node_type::LOGIC_LOW:
#if VERBOSE_DEBUG
	vpi_printf ("Set net %s (%x) to FALSE\n", nodename, net);
#endif
	v.value.scalar = vpi0;
	break;
      case node_type::LOGIC_OTHER:
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
      vpi_free_object (vpi_put_value (net, &v, &tm, vpiPureTransportDelay));
    }
    else if (n.is_breakpoint() && (n_space == n_end)) {
	STACKTRACE("breakpt && unregistered");
	const string name(prsim_state->
		get_node_canonical_name(ni));
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
  register_self_callback (vcstime);
}


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
static const bool force = true;
  switch (p->value->value.scalar) {
// NOTE: for now, set_node_time is not "forced", hence false argument
  case vpi0:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_F, vcstime);
#else
    prsim_state->set_node_time(n, node_type::LOGIC_LOW, vcstime, force);
#endif
    break;
  case vpi1:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_T, vcstime);
#else
    prsim_state->set_node_time(n, node_type::LOGIC_HIGH, vcstime, force);
#endif
    break;
  case vpiZ:
    /* nothing */
    break;
  case vpiX:
#if 0
    prs_set_nodetime (P, n, PRS_VAL_X, vcstime);
#else
    prsim_state->set_node_time(n, node_type::LOGIC_OTHER, vcstime, force);
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

/*
  Register a VCS-driven node with VCS and prsim
*/
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


/*
  Register a prsim-driven node with VCS and prsim
*/
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

/*
  Register a prsim-driven node watch-point
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
  char *arg1;

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  arg1 = strdup (arg.value.str);

  net2 = vpi_scan (h);
  if (!net2) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net2, &arg);

#if VERBOSE_DEBUG
  vpi_printf ("setup %s (vcs) -> %s (prsim)\n", arg1,
	      arg.value.str);
#endif

  if (vpi_scan (h)) {
    vpi_printf ("Usage: $to_prsim(vcs-name, prsim-name)\n");
    return 0;
  }

  register_to_prsim (arg1, arg.value.str);
  return 1;
}

static PLI_INT32 from_prsim (PLI_BYTE8 *args)
{
  STACKTRACE_VERBOSE;
  vpiHandle task_call;
  vpiHandle h;
  vpiHandle net1, net2;
  s_vpi_value arg;
  char *arg1;

  task_call = vpi_handle (vpiSysTfCall, NULL);
  h = vpi_iterate (vpiArgument, task_call);
  net1 = vpi_scan (h);
  if (!net1) {
    vpi_printf ("Usage: $from_prsim(prsim-name,vcs-name)\n");
    return 0;
  }
  arg.format = vpiStringVal;
  vpi_get_value (net1, &arg);
  arg1 = strdup (arg.value.str);

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

  register_from_prsim (arg.value.str, arg1);
  return 1;
}

/**
	One command to rule them all, 
	and in the simulator bind them.  
	This passes strings to prsim's command interpreter.  
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
  HAC_module = load_module(arg.value.str);
if (HAC_module) {
	if (!HAC_module->is_allocated()) {
		if (!HAC_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
	}
	prsim_state = count_ptr<State>(
		new State(*HAC_module, ExprAllocFlags()));
	prsim_state->initialize();
}
#endif

  return prsim_state ? 1 : 0;
}

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


struct funcs {
  char *name;
  PLI_INT32 (*f) (PLI_BYTE8 *);
};

static struct funcs f[] = {
  { "$to_prsim", to_prsim },
  { "$from_prsim", from_prsim },
  { "$prsim", prsim_file },
  { "$prsim_cmd", prsim_cmd },
  // { "$packprsim", prsim_packfile },
  { "$prsim_mkrandom", prsim_random },
  { "$prsim_resetmode", prsim_resetmode },
  { "$prsim_watch", prsim_watch }
};

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

