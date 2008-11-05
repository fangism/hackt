/**
	\file "sim/prsim/Node.cc"
	Implementation of PRS node.  
	$Id: Node.cc,v 1.13 2008/11/05 23:03:53 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include "sim/prsim/Node.h"
#include "util/macros.h"
#include "util/stacktrace.h"
#include "util/IO_utils.tcc"
#if PRSIM_INDIRECT_EXPRESSION_MAP
#include "util/STL/valarray_iterator.h"
#include "sim/prsim/State-prsim.h"	// for faninout_struct_type
#endif

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::string;
using util::write_value;
using util::read_value;

/**
	For debugging only.  Clearly see node boundaries.  
 */
#define	NODE_ALIGN_MARKERS				0

//=============================================================================
// class Node method definitions

Node::Node() : 
#if PRSIM_INDIRECT_EXPRESSION_MAP
	fanin(), 
#else
#if PRSIM_WEAK_RULES
//	The next standard of C++ better have aggregate initializers...
//	pull_up_index({INVALID_EXPR_INDEX}),
//	pull_dn_index({INVALID_EXPR_INDEX}),
#else
	pull_up_index(INVALID_EXPR_INDEX),
	pull_dn_index(INVALID_EXPR_INDEX), 
#endif
#endif
	fanout(),
	struct_flags(NODE_DEFAULT_STRUCT_FLAGS) {
	INVARIANT(!fanout.size());
#if !PRSIM_INDIRECT_EXPRESSION_MAP
#if PRSIM_WEAK_RULES
	pull_up_index[0] = pull_up_index[1] = INVALID_EXPR_INDEX;
	pull_dn_index[0] = pull_dn_index[1] = INVALID_EXPR_INDEX;
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::~Node() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PRSIM_INDIRECT_EXPRESSION_MAP
/**
	TODO: add ONLY if not already found in fanout list?
	Realloc-ing on every push_back could be slow... consider vector.  
 */
void
Node::push_back_fanout(const expr_index_type ei) {
	STACKTRACE("Node::push_back_fanout()");
#if 0
	fanout.resize(fanout.size() +1);
	const size_t i = fanout.size() -1;
	fanout[i] = ei;
#else
	fanout.push_back(ei);	// automatically resizes and reallocs
#endif
	STACKTRACE_INDENT_PRINT("fanout[" << fanout.size() -1
		<< "]=" << fanout.back() << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
Node::contains_fanout(const expr_index_type i) const {
	return (std::find(fanout.begin(), fanout.end(), i) != fanout.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by ExprAlloc when moving expressions around.  
	This occurs during compaction of optimized expressions.  
 */
void
Node::replace_pull_index(const bool dir, const expr_index_type _new
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) {
	if (dir) {
		pull_up_index STR_INDEX(w) = _new;
	} else {
		pull_dn_index STR_INDEX(w) = _new;
	}
}
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps structural information about the Node only.
	Of each up/dn pair, the first is the strong rule, 
		the second is weak (may not be printed).
 */
ostream&
Node::dump_struct(ostream& o) const {
	ostream_iterator<expr_index_type> osi(o, ", ");
#if PRSIM_INDIRECT_EXPRESSION_MAP
	o << "fanin-processes: ";
#if VECTOR_NODE_FANIN
	std::copy(fanin.begin(), fanin.end(), osi);
#else
	std::copy(begin(fanin), end(fanin), osi);
#endif
	// o << endl;
	// TODO: expand process fanins to rule expresions?
#else
	o << "up: ";
	if (pull_up_index STR_INDEX(NORMAL_RULE))
		o << pull_up_index STR_INDEX(NORMAL_RULE);
	else	o << '-';	// irrelevant
#if PRSIM_WEAK_RULES
	if (pull_up_index STR_INDEX(WEAK_RULE))
		o << '<' << pull_up_index STR_INDEX(WEAK_RULE);
	// else just omit
#endif
	o << ", dn: ";
	if (pull_dn_index STR_INDEX(NORMAL_RULE))
		o << pull_dn_index STR_INDEX(NORMAL_RULE);
	else	o << '-';	// irrelevant
#if PRSIM_WEAK_RULES
	if (pull_dn_index STR_INDEX(WEAK_RULE))
		o << '<' << pull_dn_index STR_INDEX(WEAK_RULE);
	// else just omit
#endif
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP
	o << " fanout: ";
#if 1
//	o << '<' << fanout.size() << "> ";
	std::copy(fanout.begin(), fanout.end(), osi);
	// std::copy(&fanout[0], &fanout[fanout.size()], osi);
#else
	size_t i = 0;
	for ( ; i<fanout.size(); ++i)
		o << fanout[i] << ", ";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a dot-edge from this node to each fanout expression.  
	\param s is the name of the node corresponding to the tail 
	of the edge.  
 */
ostream&
Node::dump_fanout_dot(ostream& o, const string& s) const {
	const_fanout_iterator i(fanout.begin());
	const const_fanout_iterator e(fanout.end());
	for ( ; i!=e; ++i) {
		o << s << " -> EXPR_" << *i << ';' << endl;
	}
	return o;
}

//=============================================================================
// class fanin_state_type method definitions

#if PRSIM_INDIRECT_EXPRESSION_MAP
/**
	Ripped from ExprState::dump_state()
 */
ostream&
fanin_state_type::dump_state(ostream& o) const {
	return o << "ctdn: " << countdown << " X: " << unknowns << "(/" <<
		size << ')' << " pull: " << size_t(pull());
}
#endif

//=============================================================================
// class NodeState method definitions

const uchar
NodeState::value_to_char[3] = { '0', '1', 'X' };

const value_enum
NodeState::invert_value[3] = { LOGIC_HIGH, LOGIC_LOW, LOGIC_OTHER };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This just initializes all nodes with unknown values.  
	This preserves the watchpoint and breakpoint state of all nodes.  
 */
void
NodeState::initialize(void) {
	event_index = INVALID_EVENT_INDEX;
	// placement destruct for good measure?
	new (&causes) LastCause;	// placement construct to initialize
	value = LOGIC_OTHER;
	tcount = 0;
	state_flags |= NODE_INITIALIZE_SET_MASK;
	state_flags &= ~NODE_INITIALIZE_CLEAR_MASK;
#if PRSIM_INDIRECT_EXPRESSION_MAP
	pull_up_state STR_INDEX(NORMAL_RULE).initialize();
	pull_dn_state STR_INDEX(NORMAL_RULE).initialize();
#if PRSIM_WEAK_RULES
	pull_up_state STR_INDEX(WEAK_RULE).initialize();
	pull_dn_state STR_INDEX(WEAK_RULE).initialize();
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INDIRECT_EXPRESSION_MAP
static
void
__count_fanins(fanin_state_type& s, const fanin_array_type& v) {
	s.size += v.size();
}

void
NodeState::count_fanins(const faninout_struct_type& f) {
	__count_fanins(pull_up_state STR_INDEX(NORMAL_RULE), 
		f.pull_up STR_INDEX(NORMAL_RULE));
	__count_fanins(pull_dn_state STR_INDEX(NORMAL_RULE), 
		f.pull_dn STR_INDEX(NORMAL_RULE));
#if PRSIM_WEAK_RULES
	__count_fanins(pull_up_state STR_INDEX(WEAK_RULE), 
		f.pull_up STR_INDEX(WEAK_RULE));
	__count_fanins(pull_dn_state STR_INDEX(WEAK_RULE), 
		f.pull_dn STR_INDEX(WEAK_RULE));
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This initializes the state and also wipes the watch/break flags.  
 */
void
NodeState::reset(void) {
	event_index = INVALID_EVENT_INDEX;
	// placement destruct for good measure?
	new (&causes) LastCause;	// placement construct to initialize
	value = LOGIC_OTHER;
	tcount = 0;
	state_flags = NODE_INITIAL_STATE_FLAGS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
NodeState::dump_value(ostream& o) const {
	return o << value_to_char[value & LOGIC_MASK];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps stateful information about the Node only.  
 */
ostream&
NodeState::dump_state(ostream& o) const {
	o << "val = ";
	return dump_value(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes char to node value.  
	NOTE: reserving H/L for weak logic levels.  
	\return 0, 1, 2 (X), or -1 on error.  
 */
value_enum
NodeState::char_to_value(const char v) {
	switch (v) {
	case 'f':	// fall-through
	case 'F':	// fall-through
	case '0': return LOGIC_LOW;

	case 't':	// fall-through
	case 'T':	// fall-through
	case '1': return LOGIC_HIGH;

	case 'X':	// fall-through
	case 'x':	// fall-through
	case 'U':	// fall-through
	case 'u':	// fall-through
		return LOGIC_OTHER;
	default:
		return value_enum(-1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lexes string to node value.  
	TODO: add synonymous character mappings.  
	\return 0, 1, 2 (X), or -1 on error.  
 */
value_enum
NodeState::string_to_value(const string& v) {
	if (v.length() != 1) {
		return value_enum(-1);
	} else {
		return char_to_value(v[0]);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: we don't save the event index because it may change upon 
	reconstruction due to event allocation.  
	It is reconstructed by load_event, which is called during
	event queue reconstruction.  
	Otherwise, the event_index is left as INVALID_EVENT_INDEX
 */
void
NodeState::save_state(ostream& o) const {
#if NODE_ALIGN_MARKERS
	static const char cc = 0xCC;
	write_value(o, cc);
#endif
	write_value(o, value);
	write_value(o, state_flags);
//	omit event index, which is reconstructed
	causes.save_state(o);
	write_value(o, tcount);
#if NODE_ALIGN_MARKERS
	static const char dd = 0xDD;
	write_value(o, dd);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The node state must be reeconstructed *before* the event queue
	is reconstructed, because it overwrites the event_index field.  
	\pre node is reset to unknown value to be able to reconstruct
		intermediate expression state through propagation.  
 */
void
NodeState::load_state(istream& i) {
	INVARIANT(value == LOGIC_OTHER);
#if NODE_ALIGN_MARKERS
	char dd;
	read_value(i, dd);
#endif
	read_value(i, value);
	read_value(i, state_flags);
//	omit event index, which is reconstructed
	INVARIANT(event_index == INVALID_EVENT_INDEX);
	causes.load_state(i);
	read_value(i, tcount);
#if NODE_ALIGN_MARKERS
	read_value(i, dd);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state_header(ostream& o) {
	return o << "value\tflags\tcause\ttcount";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
NodeState::dump_checkpoint_state(ostream& o, istream& i) {
	this_type temp;
	temp.load_state(i);
	temp.dump_value(o) << '\t' << size_t(temp.state_flags) <<
		'\t';
	temp.causes.dump_checkpoint_state(o);
	return o << '\t' << temp.tcount;
}

//=============================================================================
#if 0
/**
	Hmmm... need canonical name.  
 */
void
NodeState::status_dumper::operator () (const NodeState& n) {
	if (n.current_value() == match_val) {
		os << ...;
	}
}
#endif

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

