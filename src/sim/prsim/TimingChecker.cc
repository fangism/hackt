/**
	\file "sim/prsim/TimingChecker.cc"
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "sim/prsim/TimingChecker.hh"
#include "sim/prsim/State-prsim.hh"
#include "util/memory/array_pool.tcc"
#include "util/IO_utils.tcc"

#include "util/stacktrace.hh"
namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"
using util::read_value;
using util::write_value;
using std::make_pair;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
	\throw exception if there is an error
 */
TimingChecker::TimingChecker(const State& s) :
		state(s), 
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
		setup_violation_policy(E(SETUP_VIOLATION)),
		hold_violation_policy(E(HOLD_VIOLATION)),
#undef	E
		setup_check_map(),
		hold_check_map()
#if PRSIM_FWD_POST_TIMING_CHECKS
		,
		timing_check_pool(),
		timing_check_queue(),
		active_timing_check_map()
#endif
		{
}	// end TimingChecker::TimingChecker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: possibly run some checks?
	TODO: auto-checkpoint here if desired, even if state incoherent
 */
TimingChecker::~TimingChecker() {
#if PRSIM_FWD_POST_TIMING_CHECKS
	destroy_timing_checks();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
TimingChecker::reset(void) {
#define	E(e)	error_policy_enum(ERROR_DEFAULT_##e)
	setup_violation_policy = E(SETUP_VIOLATION);
	hold_violation_policy = E(HOLD_VIOLATION);
#undef	E
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
TimingChecker::set_mode_fatal(void) {
	setup_violation_policy = ERROR_FATAL;
	hold_violation_policy = ERROR_FATAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints information about setup/hold time violations.  
 */
error_policy_enum
TimingChecker::timing_exception::inspect(const State& s, ostream& o) const {
	typedef	State::node_type			node_type;
//	const node_type& tn(s.get_node(node_id));
	const node_type& rn(s.get_node(reference));
	const string tname(s.get_node_canonical_name(node_id));
	o << ((policy == ERROR_WARN) ? "Warning: " : "Error: ");
	if (tvalue == LOGIC_OTHER || rn.current_value() == LOGIC_OTHER)
		o << "possible ";
	o << (is_setup ? "setup" : "hold") << " time violation on node `"
		<< tname << "' -> " << node_type::value_to_char[size_t(tvalue)]
		<< " in process `";
	s.dump_process_canonical_name(o, pid) << "':\n";
	o << "\ttime( ";
	s.dump_node_canonical_name(o, reference);
	if (!is_setup) o << (dir ? '+' : '-');
	o << " -> " << tname;
	if (is_setup) o << (dir ? '+' : '-');
	o << " ) >= " << min_delay;
	// assume current_time has not advanced
	const time_type t1 = rn.get_last_transition_time();
	const time_type t2 = s.time();
	o << ", but got: (" << t2 << " - " << t1 << ") = " << t2-t1 << endl;
	return policy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_FWD_POST_TIMING_CHECKS
/**
	Registers a single timing exception, keeping related structures consistent.
 */
void
TimingChecker::register_timing_check(const timing_exception& tex, const time_type& ft) {
	const node_index_type gti = tex.node_id;
	const timing_check_index_type ti = timing_check_pool.allocate(tex);
	// schedule the new check ID at the trigger-node (map)
	active_timing_check_map[gti].insert(ti);
	// insert same ID into check-expiration-queue,
	//	on expiration, should dequeue, and remove from trigger-node
	timing_check_queue.insert(make_pair(ft, make_pair(ti, gti)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node ni is the reference node of a constraint.
	Post a constraint check on the trigger node that expires
	after an elapsed time dictated by the constraint.  
 */
void
TimingChecker::post_setup_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is setup check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(setup_check_map.find(ni));
if (f != setup_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with setup constraints" << endl);
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a setup constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(state.get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		const unique_process_subgraph::setup_constraint_key_type
			c = lni;
		const unique_process_subgraph::setup_constraint_set_type::const_iterator
			cf(pg.setup_constraints.find(c));
	if (cf != pg.setup_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local setup constraint" << endl);
		const vector<setup_constraint_entry>& sc(cf->second);
		vector<setup_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local trigger node "
				<< si->trig_node << endl);
			const node_index_type gti =
				state.translate_to_global_node(pid, si->trig_node);
			const time_type ft = state.time() +si->time;
			STACKTRACE_INDENT_PRINT("        should change no earlier than "
				<< ft << endl);
			// pool-allocate a check to post
			const timing_exception tex(ni, gti, LOGIC_OTHER, si->dir, true,
				pid, si->time, setup_violation_policy);
			register_timing_check(tex, ft);
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end for-all constraints with common reference node
}	// end for-all involved processes
}
}	// end post_setup_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node ni is the reference node of a constraint.
	Post a constraint check on the trigger node that expires
	after an elapsed time dictated by the constraint.  
 */
void
TimingChecker::post_hold_check(const node_index_type ni, const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is hold check on node" << endl);
	const timing_constraint_process_map_type::const_iterator
		f(hold_check_map.find(ni));
if (f != hold_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with hold constraints" << endl);
	bool rise_fall[2];
	rise_fall[1] = nv != LOGIC_LOW;	// maybe rising
	rise_fall[0] = nv != LOGIC_HIGH;	// maybe falling
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a hold constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(state.get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		bool dir = false;
	do {
		STACKTRACE_INDENT_PRINT("    check dir = " << dir << endl);
	if (rise_fall[dir]) {
		const unique_process_subgraph::hold_constraint_key_type
			c(lni, dir);
		const unique_process_subgraph::hold_constraint_set_type::const_iterator
			cf(pg.hold_constraints.find(c));
	if (cf != pg.hold_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local hold constraint" << endl);
		const vector<hold_constraint_entry>& sc(cf->second);
		vector<hold_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local trigger node "
				<< si->trig_node << endl);
			const node_index_type gti =
				state.translate_to_global_node(pid, si->trig_node);

			const time_type ft = state.time() +si->time;
			STACKTRACE_INDENT_PRINT("        should change no earlier than "
				<< ft << endl);
			// pool-allocate a check to post
			const timing_exception tex(ni, gti, LOGIC_OTHER, dir, false,
				pid, si->time, hold_violation_policy);
			const timing_check_index_type ti = timing_check_pool.allocate(tex);
			// schedule the new check ID at the trigger-node (map)
			active_timing_check_map[gti].insert(ti);
			// insert same ID into check-expiration-queue,
			//	on expiration, should dequeue, and remove from trigger-node
			timing_check_queue.insert(make_pair(ft, make_pair(ti, gti)));
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end if check in this direction of reference node
		dir = !dir;
	} while (dir);	// 2x: once per direction
	}	// end for-all constraints with common reference node
}	// end for-all involved processes
}
}	// end post_hold_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expire overdue timing checks (clear of violation window).
 */
void
TimingChecker::expire_timing_checks(void) {
	STACKTRACE_BRIEF;
	const timing_check_queue_type::const_iterator
		e(timing_check_queue.upper_bound(state.time()));
	timing_check_queue_type::iterator i(timing_check_queue.begin());
	for ( ; i!=e; ) {
		STACKTRACE_INDENT_PRINT("expiring timing check at " <<
			i->first << endl);
		const timing_check_index_type tci = i->second.first;
		const node_index_type tni = i->second.second;
		active_timing_check_map[tni].erase(tci);
		timing_check_pool.deallocate(tci);
		const timing_check_queue_type::iterator j(i);
		++i;
		timing_check_queue.erase(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructor-time, return resources.
 */
void
TimingChecker::destroy_timing_checks(void) {
	STACKTRACE_BRIEF;
	const timing_check_queue_type::const_iterator
		e(timing_check_queue.end());
	timing_check_queue_type::iterator i(timing_check_queue.begin());
	for ( ; i!=e; ) {
		const timing_check_index_type tci = i->second.first;
		const node_index_type tni = i->second.second;
		active_timing_check_map[tni].erase(tci);
		timing_check_pool.deallocate(tci);
		const timing_check_queue_type::iterator j(i);
		++i;
		timing_check_queue.erase(j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: it may be possible to reconstruct this information from 
	the rest of the state, by examining times of last edges
	on reference nodes.  
 */
void
TimingChecker::timing_exception::save(ostream& o) const {
	generic_exception::save(o);
	write_value(o, tvalue);
	write_value(o, reference);
	write_value(o, dir);
	write_value(o, is_setup);
	write_value(o, pid);
	write_value(o, min_delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
TimingChecker::timing_exception::load(istream& i) {
	generic_exception::load(i);
	read_value(i, tvalue);
	read_value(i, reference);
	read_value(i, dir);
	read_value(i, is_setup);
	read_value(i, pid);
	read_value(i, min_delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// for checkpointing
void
TimingChecker::save_active_timing_checks(ostream& o) const {
	// all we need is the timing_check_pool
	// timing_check_queue and active_timing_check_map can be reconstructed
// FIXME:
#if 0
	const size_t N = timing_check_queue.size();
	write_value(o, N);
	timing_check_queue_type::const_iterator
		qi(timing_check_queue.begin()), qe(timing_check_queue.end());
	for ( ; qi!=qe; ++qi) {
		write_value(o, qi->first);
//		timing_check_pool[qi->second.first].save(o);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// for checkpointing
void
TimingChecker::load_active_timing_checks(istream& i) {
// FIXME:
#if 0
	size_t N;
	read_value(i, N);
	size_t j = 0;
	for ( ; j<N; ++j) {
		time_type ft;
		read_value(i, ft);
#if 0
		timing_exception tex;
		tex.load(i);
		register_timing_check(tex, ft);		// reconstruct
#endif
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param tni trigger node to check for active constraints.
	\param nv new value of trigger node
 */
void
TimingChecker::check_active_timing_constraints(State& s, const node_index_type tni,
		const value_enum nv) {
	INVARIANT(&s == &state);
	timing_check_map_type::const_iterator
		f(active_timing_check_map.find(tni));
if (f != active_timing_check_map.end()) {
	const set<timing_check_index_type>& active(f->second);
	set<timing_check_index_type>::const_iterator
		ai(active.begin()), ae(active.end());
	for ( ; ai!=ae; ++ai) {
		timing_exception tex(timing_check_pool[*ai]);	// yes, copy
		tex.tvalue = nv;	// update trigger value
		if (tex.is_setup) {
			// is setup violation, trigger node is a clock
			// check for direction match on clock edge
			if (tex.dir ? (nv != LOGIC_LOW) : (nv != LOGIC_HIGH)) {
				s.handle_timing_exception(tex);
			}
		} else {
			// is hold time violation
			// direction of trigger node doesn't matter
			s.handle_timing_exception(tex);
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else	// !PRSIM_FWD_POST_TIMING_CHECKS
/**
	Node ni is the trigger node of a constraint.
	Look back at last time of transition of reference nodes.
 */
void
TimingChecker::do_setup_check(State& s, const node_index_type ni,
		const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is setup check on node" << endl);
	INVARIANT(&s == &state);
	const timing_constraint_process_map_type::const_iterator
		f(setup_check_map.find(ni));
if (f != setup_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with setup constraints" << endl);
	bool rise_fall[2];
	rise_fall[1] = nv != LOGIC_LOW;	// maybe rising
	rise_fall[0] = nv != LOGIC_HIGH;	// maybe falling
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a setup constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(state.get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		bool dir = false;
	do {
		STACKTRACE_INDENT_PRINT("    check dir = " << dir << endl);
	if (rise_fall[dir]) {
		const unique_process_subgraph::setup_constraint_key_type
			c(lni, dir);	// trigger node direction
		const unique_process_subgraph::setup_constraint_set_type::const_iterator
			cf(pg.setup_constraints.find(c));
	if (cf != pg.setup_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local setup constraint" << endl);
		const vector<setup_constraint_entry>& sc(cf->second);
		vector<setup_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local reference node "
				<< si->ref_node << endl);
			const node_index_type gri =
				state.translate_to_global_node(pid, si->ref_node);
			const time_type rt =
				state.get_node(gri).get_last_transition_time();
			STACKTRACE_INDENT_PRINT("        changed at time "
				<< rt << endl);
			const time_type d = state.time() -rt;
			if (rt >= 0.0 && (d < si->time)) {
#define	E	setup_violation_policy
			if (E > ERROR_IGNORE) {
				const timing_exception
					tex(gri, ni, nv, dir, true, pid, si->time, E);
				s.handle_timing_exception(tex);
			}
#undef	E
			}
		}	// end for local reference nodes
	}	// end if setup_constraints.find
	}	// end if rise_fall[dir]
		dir = !dir;
	} while (dir);	// 2x: once per direction
	}	// end for local node references
}	// end for processes
}	// end if check_setup_map
}	// end do_setup_check

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
TimingChecker::do_hold_check(State& s, const node_index_type ni,
		const value_enum nv) {
	STACKTRACE_INDENT_PRINT("there is hold check on node" << endl);
	INVARIANT(&s == &state);
	const timing_constraint_process_map_type::const_iterator
		f(hold_check_map.find(ni));
if (f != hold_check_map.end()) {
	STACKTRACE_INDENT_PRINT("found processes with hold constraints" << endl);
	const map<process_index_type, local_node_ids_type>& m(f->second);
	map<process_index_type, local_node_ids_type>::const_iterator
		i(m.begin()), e(m.end());
for ( ; i!=e; ++i) {
	// processes that own a hold constraint on this node
	const process_index_type pid = i->first;
	STACKTRACE_INDENT_PRINT("  process id " << pid << endl);
	const process_sim_state& ps(state.get_process_state(pid));
	const unique_process_subgraph& pg(ps.type());
	// possible multiple local occurrences
	local_node_ids_type::const_iterator
		li(i->second.begin()), le(i->second.end());
	for ( ; li!=le; ++li) {
		const node_index_type lni = *li;
		STACKTRACE_INDENT_PRINT("    local node id " << lni << endl);
		const unique_process_subgraph::hold_constraint_key_type
			c = lni;
		const unique_process_subgraph::hold_constraint_set_type::const_iterator
			cf(pg.hold_constraints.find(c));
	if (cf != pg.hold_constraints.end()) {
		STACKTRACE_INDENT_PRINT("      found local hold constraint" << endl);
		const vector<hold_constraint_entry>& sc(cf->second);
		vector<hold_constraint_entry>::const_iterator
			si(sc.begin()), se(sc.end());
		for ( ; si!=se; ++si) {
			// local nodes to check
			STACKTRACE_INDENT_PRINT("        local reference node "
				<< si->ref_node << endl);
			const node_index_type gri =
				state.translate_to_global_node(pid, si->ref_node);
			const time_type rt =
				state.get_node(gri).get_last_edge_time(
				si->dir ? LOGIC_HIGH : LOGIC_LOW);
			STACKTRACE_INDENT_PRINT("        changed at time "
				<< rt << endl);
			const time_type d = state.time() -rt;
			if (rt >= 0.0 && (d < si->time)) {
#define	E	hold_violation_policy
			if (E > ERROR_IGNORE) {
				const timing_exception
					tex(gri, ni, nv, si->dir, false, pid, si->time, E);
				s.handle_timing_exception(tex);
			}
#undef	E
			}
		}	// end for local reference nodes
	}	// end if hold_constraints.find
	}	// end for local node references
}	// end for processes
}	// end if check_hold_map
}	// end do_hold_check
#endif	// PRSIM_FWD_POST_TIMING_CHECKS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: need to check consistency with module.  
	Write out a header for safety checks.  
	TODO: save state only? without structure?
	\return true if to signal that an error occurred. 
	NOTE: we do not save the fact the a trace-file was being recorded!
 */
void
TimingChecker::save_checkpoint(ostream& o) const {
	write_value(o, setup_violation_policy);
	write_value(o, hold_violation_policy);
	// FIXME: finish me
}	// end TimingChecker::save_checkpoint

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has a ton of consistency checking.  
	TODO: need some sort of consistency check with module.  
	\pre the State is already allocated b/c no resizing is done
		during checkpoint loading.  
	\return true if to signal that an error occurred. 
 */
void
TimingChecker::load_checkpoint(istream& i) {
	read_value(i, setup_violation_policy);
	read_value(i, hold_violation_policy);
	// FIXME: finish me
}	// end TimingChecker::load_checkpoint

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Textual dump of checkpoint without loading it.  
	Keep this consistent with the save/load methods above.  
	\param i the input file stream for the checkpoint.
	\param o the output stream to dump to.  
 */
ostream&
TimingChecker::dump_checkpoint(ostream& o, istream& i) {
{
	error_policy_enum p;
        read_value(i, p);
        o << "setup-violation policy: " << error_policy_string(p) << endl;
        read_value(i, p);
        o << "hold-violation policy: " << error_policy_string(p) << endl;
}
	// FIXME: finish me
	return o;
}	// end TimingChecker::dump_checkpoint

//=============================================================================
// explicit class template instantiations
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

