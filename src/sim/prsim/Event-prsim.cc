/**
	\file "sim/prsim/Event-prsim.cc"
	Implementation of prsim event structures.  
	$Id: Event-prsim.cc,v 1.1.40.1 2007/12/28 18:44:54 fang Exp $

	NOTE: file was renamed from:
	Id: Event.cc,v 1.8 2007/01/21 06:00:58 fang Exp
	to avoid dyld object base name conflict with chpsim's Event.  
	Sadly, this breaks continuity in revision history.
 */

#include <iostream>
#include <iterator>
#include <numeric>
#include "sim/prsim/Event-prsim.h"
#include "sim/time.h"
#include "util/memory/index_pool.tcc"
#include "util/IO_utils.tcc"

// debug switch defined in this corresponding header file
#if DEBUG_EVENT_POOL_ALLOC
#define	ENABLE_STACKTRACE			1
#endif

#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
// class Event static initializations

/**
	First index is the guard's pulling state (F = OFF, T = ON, X = WEAK),
	second index is the pending event state.
	NOTE: the ordering of these tables must be kept consistent
		with the enumeration values for node_type::value and
		expr_type::pull_enum.
 */
const uchar
Event::upguard[3][3] = {
	{	EVENT_VACUOUS, 		// guard F, event F: vacuous
		EVENT_UNSTABLE,		// guard F, event T: unstable
		EVENT_VACUOUS		// guard F, event X: vacuous
	},
	{	EVENT_INTERFERENCE,	// guard T, event F: interference
		EVENT_VACUOUS,		// guard T, event T: vacuous
		EVENT_VACUOUS		// guard T, event X: vacuous
	},
	{	EVENT_WEAK_INTERFERENCE,// guard X, event F:
		EVENT_WEAK_UNSTABLE,	// guard X, event T:
		EVENT_VACUOUS		// guard X, event X: vacuous
	}
};

const uchar
Event::dnguard[3][3] = {
	{	EVENT_UNSTABLE,		// guard F, event F: vacuous
		EVENT_VACUOUS,		// guard F, event T: unstable
		EVENT_VACUOUS		// guard F, event X: vacuous
	},
	{	EVENT_VACUOUS,		// guard T, event F: interference
		EVENT_INTERFERENCE,	// guard T, event T: vacuous
		EVENT_VACUOUS		// guard T, event X: vacuous
	},
	{	EVENT_WEAK_UNSTABLE,	// guard X, event F:
		EVENT_WEAK_INTERFERENCE,// guard X, event T:
		EVENT_VACUOUS		// guard X, event X: vacuous
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
Event::save_state(ostream& o) const {
	write_value(o, node);
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	cause.save_state(o);
#else
	write_value(o, cause_node);
#endif
#if PRSIM_CHECKPOINT_CAUSE_RULE
	// rule/expr IDs will vary with optimization level!
	// don't really need: rule ID ise only used for get_delay_up/dn
	write_value(o, cause_rule);
#endif
	write_value(o, val);
	// is this necessary, or is there an invariant we can rely on?
	write_value(o, flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
Event::load_state(istream& i) {
	read_value(i, node);
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
	cause.load_state(i);
#else
	read_value(i, cause_node);
#endif
#if PRSIM_CHECKPOINT_CAUSE_RULE
	read_value(i, cause_rule);
#endif
	read_value(i, val);
	read_value(i, flags);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
Event::dump_checkpoint_state_header(ostream& o) {
	return o << "node\tcause-node\tcause-rule\tvalue";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update to dump flags
 */
ostream&
Event::dump_checkpoint_state(ostream& o, istream& i) {
	this_type temp;
	temp.load_state(i);
	return o << temp.node << "\t\t" << 
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
		'(' << temp.cause.node << ',' << size_t(temp.cause.val) << ')'
		// could use Node::value_to_char[]...
#else
		temp.cause_node
#endif
		<< "\t\t" <<
		temp.cause_rule << '\t' << size_t(temp.val);
}

//=============================================================================
// class EventPool method definitions
/**
	This always reserves the 0th entry as an invalid entry.  
	Thus, 0 should never be in the freelist.  
 */
EventPool::EventPool() : event_pool(), free_indices() {
	const event_index_type zero __ATTRIBUTE_UNUSED__ =
		event_pool.allocate();
	INVARIANT(!zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventPool::~EventPool() {
	INVARIANT(check_valid_empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strict paranoia checking.  
	All resources returned properly to the pool, free-list
		must be as big as the pool (minus sentinel).  
 */
bool
EventPool::check_valid_empty(void) const {
	bool die = false;
	const size_t p = event_pool.size() -1;
	const size_t m = free_indices.size();
	if (p != m) {
		cerr << "FATAL: event pool size is " << p <<
			" while free-list size is " << m << endl;
		die = true;
	} else {
		const size_t s = std::accumulate(
			free_indices.begin(), free_indices.end(), 0);
		if (s != m*(m+1)/2) {	// triangular sum
			cerr << "FATAL: event pool free list sum "
				"is not what\'s expected!" << endl;
			die = true;
		}
	}
	if (die) {
		std::ostream_iterator<size_t> osi(cerr, ",");
		cerr << "free-list: ";
		copy(free_indices.begin(), free_indices.end(), osi);
		cerr << endl;
	}
	return !die;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
EventPool::clear(void) {
	free_indices.clear();
	event_pool.clear();
	const event_index_type zero __ATTRIBUTE_UNUSED__ =
		event_pool.allocate();
	INVARIANT(!zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if DEBUG_EVENT_POOL_ALLOC
/**
	Keep this consistent with the inline definition.  
 */
event_index_type
EventPool::allocate(const event_type& e) {
	STACKTRACE_VERBOSE;
	if (UNLIKELY(free_indices.empty())) {   // UNLIKELY
		const event_index_type ret = event_pool.size();
		event_pool.allocate(e); // will realloc
		INVARIANT(ret);
		STACKTRACE_INDENT_PRINT("allocating entry: " << ret << endl);
		return ret;
	} else {                        // LIKELY
		const event_index_type ret = free_list_acquire(free_indices);
		event_pool[ret] = e;
		STACKTRACE_INDENT_PRINT("allocating entry: " << ret << endl);
		return ret;
	}
}
#endif

#if DEBUG_EVENT_POOL_ALLOC
void
EventPool::deallocate(const event_index_type i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("deallocating entry: " << i << endl);
	INVARIANT(i);
	free_list_release(free_indices, i);
}
#endif  

}	// end namespace PRSIM

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

