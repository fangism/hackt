/**
	\file "sim/prsim/TimingChecker.hh"
	Classes for timing constraint checking.
 */

#ifndef	__HAC_SIM_PRSIM_TIMING_CHECKER_HH____
#define	__HAC_SIM_PRSIM_TIMING_CHECKER_HH____

#include <vector>
#include <set>
#include <map>
#include "sim/time.hh"
#include "sim/prsim/Exception.hh"
#include "sim/prsim/enums.hh"
#include "util/memory/array_pool.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::vector;
using std::pair;
class State;
class ExprAlloc;

//=============================================================================
/**
	Structure responsible for maintaining timing constraint checking.
 */
class TimingChecker {
	typedef	real_time			time_type;
public:
	struct timing_exception : public generic_exception {
		/// value of trigger node
		value_enum			tvalue;
		/// reference node index
		node_index_type			reference;	// reference node
		/// direction of clock edge true:pos, false:neg
		bool				dir;
		/// type of timing violation (eventually enum)
		bool				is_setup;	// else hold
		/// process id that owns this constraint
		process_index_type		pid;
		/// min_delay value of constraint
		time_type			min_delay;

		timing_exception() :
			generic_exception(INVALID_NODE_INDEX, ERROR_FATAL) {
			/* uninitialized */
		}
		timing_exception(const node_index_type r,
			const node_index_type t, 
			const value_enum v,
			const bool d,
			const bool type, const process_index_type p,
			const time_type m,
			const error_policy_enum e) : 
			generic_exception(t, e),
			tvalue(v),
			reference(r), dir(d),
			is_setup(type), pid(p), min_delay(m) {
		}

		void
		save(ostream&) const;

		void
		load(istream&);

		error_policy_enum
		inspect(const State&, ostream&) const;
	};	// end timing_exception

private:
	const State&				state;	// back-reference
public:
	/// setup time violation policy
	error_policy_enum			setup_violation_policy;
	/// hold time violation policy
	error_policy_enum			hold_violation_policy;

private:
	enum {
		ERROR_DEFAULT_SETUP_VIOLATION = ERROR_WARN,
		ERROR_DEFAULT_HOLD_VIOLATION = ERROR_BREAK
	};

	/**
		key: index of node that triggers timing check(s)
		PRSIM_FWD_POST_TIMING_CHECKS:0 -- key is the trigger node
		PRSIM_FWD_POST_TIMING_CHECKS:1 -- key is the reference node
		value: set of processes that need to check the node
			These should be unique and sorted.
	 */
	typedef	std::set<node_index_type>	local_node_ids_type;
	typedef	std::map<node_index_type,
			std::map<process_index_type, local_node_ids_type> >
					timing_constraint_process_map_type;
	timing_constraint_process_map_type	setup_check_map;
	timing_constraint_process_map_type	hold_check_map;
#if PRSIM_FWD_POST_TIMING_CHECKS
	// can we use a timing_exception?
	typedef	size_t				timing_check_index_type;
	typedef	util::memory::array_pool<vector<timing_exception>,
			vector<timing_check_index_type> >
						timing_check_pool_type;
	typedef	pair<timing_check_index_type, node_index_type>
						timing_check_queue_entry;
	typedef	std::multimap<time_type, timing_check_queue_entry>
						timing_check_queue_type;
	typedef	std::map<node_index_type, std::set<timing_check_index_type> >
						timing_check_map_type;
	/// this owns the actual timing_exception objects
	timing_check_pool_type			timing_check_pool;
	/// time-ordered queue of pointers to active checks
	timing_check_queue_type			timing_check_queue;
	/// per-node set of active timing checks
	timing_check_map_type			active_timing_check_map;
#endif	// PRSIM_FWD_POST_TIMING_CHECKS

public:
	explicit
	TimingChecker(const State&);
	~TimingChecker();

	/**
		\param g global node index
		\param p global process index
		\param l local node index in that process
	 */
	void
	register_setup_check(const node_index_type g, 
		const process_index_type p, const node_index_type l) {
		setup_check_map[g][p].insert(l);
	}

	void
	register_hold_check(const node_index_type g, 
		const process_index_type p, const node_index_type l) {
		hold_check_map[g][p].insert(l);
	}

	void
	reset(void);

	void
	set_mode_fatal(void);

#if PRSIM_FWD_POST_TIMING_CHECKS
	void
	register_timing_check(const timing_exception&, const time_type&);

	void
	post_setup_check(const node_index_type, const value_enum);

	void
	post_hold_check(const node_index_type, const value_enum);

	void
	expire_timing_checks(void);

	void
	destroy_timing_checks(void);

	void
	check_active_timing_constraints(State&, const node_index_type, 
		const value_enum);

	// for checkpointing
	void
	save_active_timing_checks(ostream&) const;

	void
	load_active_timing_checks(istream&);
#else
	void
	do_setup_check(State&, const node_index_type, const value_enum);

	void
	do_hold_check(State&, const node_index_type, const value_enum);
#endif

	void
	save_checkpoint(ostream&) const;

	void
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

};	// end class TimingChecker

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_TIMING_CHECKER_HH____

