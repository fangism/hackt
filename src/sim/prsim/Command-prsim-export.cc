/**
	\file "sim/prsim/Command-prsim-export.cc"
	$Id: Command-prsim-export.cc,v 1.4 2010/08/25 18:53:44 fang Exp $
	Useful functions to be exported to elsewhere.  
 */

#include <iostream>
#include "sim/prsim/Command-prsim-export.hh"
#include "sim/prsim/Command-prsim.hh"
#include "sim/prsim/State-prsim.hh"
#include "sim/command_base.hh"		// for return values
#include "util/numformat.tcc"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"

// for convenient time formatting
static
inline
util::format_ostream_ref
format_time(ostream& o, const State& s) {
        return util::format_ostream_ref(o, s.time_fmt);
}

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
	Actions to print feedback after an event.  
	\param ni node that just changed in event.
	\param i number of steps remaining, or 0 if not applicable.
	\return true if node was a breakpoint.
 */
bool
post_event_messages(ostream& o, const State& s, 
		const step_return_type& ni, const size_t i) {
	const time_type ct(s.time());
	const node_type& n(s.get_node(GET_NODE(ni)));
	/***
		The following code should be consistent with
		Cycle::main() and Advance::main().
		tracing stuff here later...
	***/
	if (s.watching_all_nodes() || n.is_watchpoint()) {
		format_time(o << '\t', s) << ct << '\t';
		print_watched_node(o, s, ni);
	}
	if (s.have_atomic_updates()) {
		s.print_watched_atomic_updated_nodes(o);
	}
	if (n.is_breakpoint()) {
		const string nodename(s.get_node_canonical_name(GET_NODE(ni)));
		// node is plain breakpoint
		o << "\t*** break, ";
		if (i) {
			o << i << " steps left: ";
		}
		o << "`" << nodename << "\' became ";
		format_time(n.dump_value(o) << " at time ", s) << ct << endl;
		return true;
	}
	return false;
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
while (!s.stopped_or_fatal() && s.pending_events() &&
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
	if (s.watching_all_nodes() || n.is_watchpoint()) {
		format_time(cout << '\t', s) << s.time() << '\t';
		print_watched_node(cout, s, ni);
	}
	if (n.is_breakpoint()) {
		// node is plain breakpoint
		if (show_break) {
			const string nodename(s.get_node_canonical_name(
				GET_NODE(ni)));
			format_time(cout << "\t*** break, ", s)
				<< (stop_time -s.time()) <<
				" time left: `" << nodename << "\' became ";
			n.dump_value(cout) << endl;
		}
			return Command::NORMAL;
			// or Command::BREAK; ?
	}
}	// end while
	if (!s.stopped() && (s.time() < stop_time)) {
		s.update_time(stop_time);
	}
if (s.is_fatal()) {
	return error_policy_to_status(s.inspect_exceptions());
}
	// else leave the time at the time as of the last event
	return Command::NORMAL;
}	// end prsim_advance

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

