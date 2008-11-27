/**
	\file "sim/prsim/Command-prsim-export.cc"
	$Id: Command-prsim-export.cc,v 1.3 2008/11/27 11:09:35 fang Exp $
	Useful functions to be exported to elsewhere.  
 */

#include <iostream>
#include "sim/prsim/Command-prsim-export.h"
#include "sim/prsim/Command-prsim.h"
#include "sim/prsim/State-prsim.h"
#include "sim/command_base.h"		// for return values

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"

/**
	Yeah, I know looking up already looked up node, but we don't
	care because printing and diagnostics are not performance-critical.  
	\param nodename the name to use for reporting, which need not be
		the canonical name of the node, but some equivalent.  
 */
ostream&
print_watched_node(ostream& o, const State& s, 
		const step_return_type& r, const string& nodename) {
	const node_index_type ni = GET_NODE(r);
	// const string nodename(s.get_node_canonical_name(ni));
	const State::node_type& n(s.get_node(ni));
	n.dump_value(o << nodename << " : ");
	const node_index_type ci = GET_CAUSE(r);
	if (ci && s.show_cause()) {
		const string causename(s.get_node_canonical_name(ci));
		const State::node_type& c(s.get_node(ci));
		c.dump_value(o << "\t[by " << causename << ":=") << ']';
	}
	if (s.show_tcounts()) {
		o << "\t(" << n.tcount << " T)";
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This automatically uses the canonical name.  
 */
ostream&
print_watched_node(ostream& o, const State& s, 
		const step_return_type& r) {
	const node_index_type ni = GET_NODE(r);
	const string nodename(s.get_node_canonical_name(ni));
	return print_watched_node(o, s, r, nodename);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variation deduces the cause of the given node's last transition,
	the last arriving input to a firing rule.  
	\param nodename the name to use for reporting.  
 */
ostream&
print_watched_node(ostream& o, const State& s, 
		const node_index_type ni, const string& nodename) {
	return print_watched_node(o, s,
		step_return_type(ni, s.get_node(ni).get_cause_node()), 
		nodename);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s the simulator state
	\param stop_time the max time to stop and return
	\param show_break true to print extra breakpoint message
 */
int
prsim_advance(State& s, const time_type stop_time, bool show_break) {
	step_return_type ni;
	s.resume();
try {
while (!s.stopped() && s.pending_events() &&
		(s.next_event_time() < stop_time) &&
		GET_NODE((ni = s.step()))) {
	// honor breakpoints?
	// tracing stuff here later...
	const node_type& n(s.get_node(GET_NODE(ni)));
	/***
		The following code should be consistent with
		Cycle::main() and Step::main().
		TODO: factor this out for maintainability.  
	***/
	if (s.watching_all_nodes()) {
		print_watched_node(cout << '\t' << s.time() << '\t', s, ni);
	}
	if (n.is_breakpoint()) {
		// this includes watchpoints
		const bool w = s.is_watching_node(GET_NODE(ni));
		const string nodename(s.get_node_canonical_name(
			GET_NODE(ni)));
		if (w) {
		if (!s.watching_all_nodes()) {
			print_watched_node(cout << '\t' <<
				s.time() << '\t', s, ni);
		}	// else already have message from before
		}
		// channel support
		if (!w) {
			// node is plain breakpoint
		if (show_break) {
			cout << "\t*** break, " << stop_time -s.time() <<
				" time left: `" << nodename << "\' became ";
			n.dump_value(cout) << endl;
		}
			return Command::NORMAL;
			// or Command::BREAK; ?
		}
	}
}	// end while
} catch (const step_exception& exex) {
	return error_policy_to_status(exex.inspect(s, cerr));
}	// no other exceptions
	if (!s.stopped() && s.time() < stop_time) {
		s.update_time(stop_time);
	}
	// else leave the time at the time as of the last event
	return Command::NORMAL;
}	// end prsim_advance

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

