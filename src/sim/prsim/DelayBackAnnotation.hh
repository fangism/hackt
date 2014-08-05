/**
	\file "sim/prsim/DelayBackAnnotation.hh"
 */
#ifndef	__HAC_SIM_PRSIM_DELAYBACKANNOTATION_HH__
#define	__HAC_SIM_PRSIM_DELAYBACKANNOTATION_HH__

#include <map>
#include <set>
#include "util/memory/index_pool.hh"
#include "util/monotonic_vector.hh"
#include "sim/common.hh"
#include "sim/time.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::map;
using std::set;
using util::memory::index_pool;

// since min-delay constraints will be specified by type(scope)
// we want to store constraints in unique locations, 
// and then have node-pairs (reference-target) refer to the unique
// locations to access timing data (for sake of memory conservation).

// timing database interface:
// commands to modify delays
//	add
//	delete
//	change

struct delays {
	typedef	real_time			time_type;
	/**
		index[-+][-+] for all combinations of unate
	 */
	time_type				min_delay[2][2];

	delays() {
		min_delay[0][0] = 0;
		min_delay[0][1] = 0;
		min_delay[1][0] = 0;
		min_delay[1][1] = 0;
	}
};

// TODO: for memory/speed, setup using sparse structures
// then freeze into dense and compact structures
// like binary-searchable arrays
#if 0
// index_pool<delays> for unique delay value sets
typedef	size_t					delay_index_type;
typedef	node_index_type				pred_index_type;
#endif
// or use vector? like Node::process_fanin_type
// typedef	set<process_index_type>			process_timing_fanin_type;
typedef	util::monotonic_vector<process_index_type>
						process_timing_fanin_type;

// see TimingChecker for similar structures
// predicate node index? (ebool unique ID)

#if 0
/**
	predicate index cannot be part of delay structure because it
	references a global ebool/bool index.  
	first: predicate index (0 means unconditional)
	second: delay values
 */
typedef	std::pair<pred_index_type, delay_index_type>
						unique_delay_entry;
#endif

/**
	This class manages the global delay database.
	TODO: convert between sparse and dense maps for lookup efficiency
 */
class delay_back_annotation_manager {
#if 0
	/**
		key: reference node
		value: index into delay_pool
	 */
	typedef	map<node_index_type, unique_delay_entry>
						delay_reference_map_type;
	typedef	map<node_index_type, delay_reference_map_type>
						min_delay_map_type;
	/// delay database
	min_delay_map_type			delay_db;
#endif
	/**
		Each node (global) maintains a set of processes
		that own timing constraints that involve the key node
		as a target.  
		Since we don't expect to annotate the majority of
		nodes, this structure should remain sparse.
		Reminder that the global top-level process is a 
		special case and is indexed 0.
		key: target node index
		value: processes that can delay the target node
			with min-delay constraints.
		TODO: replace this with a vector<pair> because
			by construction this is grown monotonically,
			so we can use push_back instead of insert.
	 */
	typedef	map<node_index_type, process_timing_fanin_type>
						global_timing_fanin_map;
	global_timing_fanin_map			timing_fanin;
public:

	// two methods of adding delays:
	// global scope -- add a single delay constraint
	// type scope -- apply a delay constraint to *all*
	//	instances of the same type (everywhere in hierarchy).
	//	this results in adding multiple delay entries

	void
	add_timing_fanin(const node_index_type, const process_index_type);

	const process_timing_fanin_type*
	lookup_process_timing_fanin(const node_index_type) const;

	void
	reset_timing_fanin(void) {
		timing_fanin.clear();
	}

};	// end class delay_back_annotation_manager


}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_DELAYBACKANNOTATION_HH__
