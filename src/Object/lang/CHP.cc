/**
	\file "Object/lang/CHP.cc"
	Class implementations of CHP objects.  
	$Id: CHP.cc,v 1.18 2007/01/23 02:43:16 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	ENABLE_STACKTRACE_CHPSIM		(0 && ENABLE_STACKTRACE)

/**
	Various levels of chpsim event generation optimizations, 
	such as fusion.  
	TODO: control in execute-time switch.  
 */
#define	OPTIMIZE_CHPSIM_EVENTS			1

#include <iterator>
#include <algorithm>
#include <exception>
#include "Object/lang/CHP.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/pbool_const.h"
#include "Object/def/footprint.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference_base.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/value_scalar.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/connection_policy.h"
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/def/template_formals_manager.h"
#include "Object/nonmeta_context.h"
#include "Object/state_manager.h"
#include "Object/global_channel_entry.h"
#include "Object/nonmeta_channel_manipulator.h"

// chpsim headers
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/DependenceCollector.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/nonmeta_context.h"

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/visitor_functor.h"
#include "util/value_saver.h"
#include "util/indent.h"
#include "util/IO_utils.tcc"
#include "util/STL/valarray_iterator.h"
#include "util/reference_wrapper.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/numeric/random.h"	// for rand48

#if ENABLE_STACKTRACE_CHPSIM
#define	STACKTRACE_CHPSIM_VERBOSE	STACKTRACE_VERBOSE
#else
#define	STACKTRACE_CHPSIM_VERBOSE
#endif

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::action_sequence,
		"CHP-action-sequence")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::concurrent_actions,
		"CHP-concurrent-actions")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::guarded_action,
		"CHP-guarded-action")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::deterministic_selection,
		"CHP-deterministic-selection")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::nondeterministic_selection,
		"CHP-nondeterministic-selection")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::metaloop_selection,
		"CHP-metaloop-selection")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::metaloop_statement,
		"CHP-metaloop-statement")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::assignment,
		"CHP-assignment")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::condition_wait,
		"CHP-condition-wait")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::channel_send,
		"CHP-channel-send")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::channel_receive,
		"CHP-channel-receive")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::do_forever_loop, 
		"CHP-forever-loop")
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::do_while_loop, 
		"CHP-do-while")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::action_sequence, CHP_SEQUENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::concurrent_actions, CHP_CONCURRENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::guarded_action, CHP_GUARDED_ACTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::deterministic_selection, CHP_DET_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::nondeterministic_selection, CHP_NONDET_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::metaloop_selection, CHP_SELECT_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::metaloop_statement, CHP_ACTION_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::assignment, CHP_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::condition_wait, CHP_WAIT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::channel_send, CHP_SEND_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::channel_receive, CHP_RECEIVE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::do_forever_loop, CHP_FOREVER_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::do_while_loop, CHP_DO_WHILE_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
namespace CHP {
using std::equal;
using std::find;
using std::transform;
using std::back_inserter;
using std::for_each;
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using SIM::CHPSIM::EventNode;
using SIM::CHPSIM::RECHECK_NEVER_BLOCKED;
using SIM::CHPSIM::RECHECK_BLOCKED_THIS;
using SIM::CHPSIM::RECHECK_UNBLOCKED_THIS;
using SIM::CHPSIM::RECHECK_DEFERRED_TO_SUCCESSOR;
using util::reference_wrapper;
using util::set_inserter;
using util::numeric::rand48;

//=============================================================================
/// helper routines
static
good_bool
unroll_resolve_selection_list(const selection_list_type&,
	const unroll_context&, selection_list_type&);

static
good_bool
set_channel_alias_directions(const simple_channel_nonmeta_instance_reference&, 
	const unroll_context&, const bool);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For all non-selection events that execute, schedule all successor
	events for recheck.
	Also decrement those events' countdown upon execution of this event.  
	Don't forget to decrement the selected event's counter for selections!
	This should be kept consistent with CHPSIM::EventNode::execute().
 */
static
inline
void
recheck_all_successor_events(const nonmeta_context& c) {
	typedef	EventNode	event_type;
	typedef	size_t		event_index_type;
	STACKTRACE_CHPSIM_VERBOSE;
	const event_type::successor_list_type&
		succ(c.get_event().successor_events);
	copy(std::begin(succ), std::end(succ), set_inserter(c.rechecks));
	for_each(std::begin(succ), std::end(succ), 
		event_type::countdown_decrementer(c.event_pool));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print outgoing edges adorned with guard expressions as labels. 
 */
static
ostream&
dump_selection_successor_edges(const selection_list_type& l, ostream& o, 
		const EventNode& e, const size_t i, 
		const expr_dump_context& c) {
	typedef	selection_list_type::const_iterator const_iterator;
	const EventNode::successor_list_type& succ(e.successor_events);
	const size_t* si = std::begin(succ);
	const size_t* se = std::end(succ);
	const_iterator li(l.begin()), le(l.end());
	for ( ; li!=le; ++li, ++si) {
		const guarded_action::guard_ptr_type&
			g((*li)->get_guard());
		o << EventNode::node_prefix << i << " -> " <<
			EventNode::node_prefix << *si << "\t[label=\"";
		if (g) {
			g->dump(o, c);
		} else {
			o << "else";
		}
		o << "\"];" << endl;
	}
	// guard list may have ONE less than successor list
	// if there is an implicit else-clause
	if (si != se) {
		o << EventNode::node_prefix << i << " -> " <<
			EventNode::node_prefix << *si <<
			"\t[label=\"else\"];" << endl;
		++si;
		INVARIANT(si == se);
		
	}
	// check for else clause
	return o;
}

//=============================================================================
// class action method definitions

/**
	Unroll-copies the action pointer, using shallow copy if
	nothing changed, deep-copy if something internal changed.  
	\throw exception if there is an error.  
 */
action_ptr_type
action::transformer::operator () (const action_ptr_type& a) const {
	NEVER_NULL(a);
	return a->unroll_resolve_copy(_context, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By default print all successor edge, unadorned.  
 */
ostream&
action::dump_successor_edges(ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context&) const {
	return e.dump_successor_edges_default(o, i);
}

//=============================================================================
// struct data_expr_unroll_resolver method definitions

count_ptr<const data_expr>
data_expr_unroll_resolver::operator () (
		const count_ptr<const data_expr>& e) const {
	NEVER_NULL(e);
	return e->unroll_resolve_copy(_context, e);
}

//=============================================================================
// struct data_ref_unroll_resolver method definitions

count_ptr<const data_nonmeta_instance_reference>
data_ref_unroll_resolver::operator () (
	const count_ptr<const data_nonmeta_instance_reference>& e) const {
	NEVER_NULL(e);
	return e->unroll_resolve_copy(_context, e);
}

//=============================================================================
// class action_sequence method definitions

action_sequence::action_sequence() : parent_type(), list_type() { }

action_sequence::~action_sequence() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(action_sequence)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
action_sequence::dump(ostream& o, const expr_dump_context& c) const {
	o << "sequential: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
action_sequence::dump_event(ostream& o, const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
action_sequence::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> ret(new this_type);
	NEVER_NULL(ret);
#if 0
	try {
		transform(begin(), end(), back_inserter(*ret), 
			action::transformer(c)
		);
	} catch (...) {
		cerr << "Error unrolling action_sequence." << endl;
		return unroll_action_return_type();
	}
	const bool eq = equal(begin(), end(), ret->begin());
	return unroll_action_return_type(!eq, ret);
#else
	transform(begin(), end(), back_inserter(*ret), action::transformer(c));
	const const_iterator
		f(find(ret->begin(), ret->end(), action_ptr_type(NULL)));
	if (f != ret->end()) {
		cerr << "Error unrolling action_sequence." << endl;
		return action_ptr_type(NULL);
	}
	if (equal(begin(), end(), ret->begin()))
		return p;
	else	return ret;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a sequence of events backwards from back to
	front, where each event 'connects' to its successor(s).
 */
void
action_sequence::accept_sequence(const action_list_type& l, 
		StateConstructor& s) {
	STACKTRACE_VERBOSE;
//	for_each(rbegin(), rend(), util::visitor_ptr(s));
	const_iterator i(l.begin()), e(l.end());
	INVARIANT(i!=e);	// else would be empty
	do {
		--e;
		(*e)->accept(s);
		// events will link themselves (callee responsible)
	} while (i!=e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Do we need to construct event successor edges and graphs?
	NOTE: this does not actually allocate an event for itself.  
	Constructs events in a backwards order to simplify event-chaining
	of predecessors to successors.  
 */
void
action_sequence::accept(StateConstructor& s) const {
	accept_sequence(*this, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequences should never be used as leaf events, 
	so this does nothing.  
 */
void
action_sequence::execute(const nonmeta_context&, 
		global_reference_array_type&) const {
	// no-op
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequences should never be used as leaf events, 
	so this does nothing.  
 */
char
action_sequence::recheck(const nonmeta_context&) const {
	// no-op
	ICE_NEVER_CALL(cerr);
	return RECHECK_UNBLOCKED_THIS;	// don't care
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::load_object(const persistent_object_manager& m, 
		istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// class concurrent_actions method definitions

concurrent_actions::concurrent_actions() : parent_type(), list_type() { }

concurrent_actions::~concurrent_actions() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(concurrent_actions)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just the core dump routine, auto-indented with terminating newline.  
 */
ostream&
concurrent_actions::__dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		(*i)->dump(o << auto_indent, c) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
concurrent_actions::dump(ostream& o, const expr_dump_context& c) const {
	o << "concurrent: {" << endl;
	{
		INDENT_SECTION(o);
		__dump(o, c);
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for dumping resolved footprint, called from footprint::dump.
	\param f parent footprint (currently unused)
 */
ostream&
concurrent_actions::dump(ostream& o, const entity::footprint& f,
		const expr_dump_context& c) const {
if (!empty()) {
	o << auto_indent << "resolved concurrent actions:" << endl;
	{
		INDENT_SECTION(o);
		__dump(o, c);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
concurrent_actions::dump_event(ostream& o, const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
concurrent_actions::__unroll(const unroll_context& c, this_type& r) const {
	transform(begin(), end(), back_inserter(r), action::transformer(c));
	const const_iterator
		f(find(r.begin(), r.end(), action_ptr_type(NULL)));
	if (f != r.end()) {
		cerr << "Error unrolling action_sequence." << endl;
		// cerr << "Error unrolling action list." << endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a partially-deep copy of the language tree, 
	re-using references to unchanged members where possible.  
	If unrolling any list member results in a different pointer, 
	then a new sequence is created.  
	Purpose: resolve all meta-parameter dependents to constants.  
	\return pointer to self-if unchanged, pointer to newly
		allocated sequence if changed, NULL if error encountered.  
 */
action_ptr_type
concurrent_actions::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> ret(new this_type);
	NEVER_NULL(ret);
	if (__unroll(c, *ret).good) {
		if (equal(begin(), end(), ret->begin()))
			return p;
		else	return ret;
	} else {
		return action_ptr_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by process_definition::create_complete_type().
 */
good_bool
concurrent_actions::unroll(const unroll_context& c,
		entity::footprint& f) const {
	STACKTRACE_VERBOSE;
	this_type& t(f.get_chp_footprint());
	return __unroll(c, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fork and join graph structure.  
	TODO: this dependencies for this event is the union of 
		all dependencies of the respective guards.  
		If there is an else clause, we may omit dependencies, 
		as a branch is always taken immediately.
		Will we ever check for guard stability?
 */
void
concurrent_actions::accept(StateConstructor& s) const {
	// TODO: using footprint frame, allocate event edge graph
	// there will be multiple outgoing edges
	STACKTRACE_VERBOSE;
	const size_t branches = this->size();
	STACKTRACE_INDENT_PRINT("branches: " << branches << endl);
// check for degenerate cases first: 0, 1
// these can arise from meta-expansions
if (!branches) {
	return;
} else if (branches == 1) {
	// don't bother forking and joining
	front()->accept(s);
	return;
}
// else do the normal thing
	// create a join event first (bottom-up)
	const size_t join_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_CONCURRENT_JOIN, 
			s.current_process_index));
{
	STACKTRACE_INDENT_PRINT("join index: " << join_index << endl);
	// join shouldn't need an action ptr (unless we want back-reference)
	EventNode& join_event(s.get_event(join_index));
	s.connect_successor_events(join_event);
	join_event.set_predecessors(branches);	// expect number of branches
	// reminder, reference may be invalidated after push_back
}
	// NOTE: no dependencies to track, unblocked concurrency
	const_iterator i(begin()), e(end());
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		s.last_event_index = join_index;	// pass down
		(*i)->accept(s);
		tmp.push_back(s.last_event_index);	// head of each chain
	}
{
	// have to set it again!?  must get clobbered by above loop...
	EventNode& join_event(s.get_event(join_index));
	join_event.set_predecessors(branches);
	s.count_predecessors(join_event);
}

	// construct successor event graph edge? or caller's responsibility?
	const size_t fork_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_CONCURRENT_FORK, 
			s.current_process_index));
{
	STACKTRACE_INDENT_PRINT("fork index: " << fork_index << endl);
	EventNode& fork_event(s.get_event(fork_index));

	fork_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &fork_event.successor_events[0]);

	// updates successors' predecessor-counts
	s.count_predecessors(fork_event);
}
	// leave trail of this event for predecessor
	s.last_event_index = fork_index;
	// construct an event join graph-node?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Action groups should never be used as leaf events, 
	so this does nothing.  
	Conditionally enqueue all successors, blocking on ones that are not
	ready to execute.  
 */
void
concurrent_actions::execute(const nonmeta_context& c, 
		global_reference_array_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	recheck_all_successor_events(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Action groups should never be used as leaf events, 
	so this does nothing.  
	This event also never blocks, although its successors may block.
 */
char
concurrent_actions::recheck(const nonmeta_context&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	// no-op
	return RECHECK_NEVER_BLOCKED;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::load_object(const persistent_object_manager& m, 
		istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// struct guarded_action::unroll_resolver method definitions

count_ptr<const guarded_action>
guarded_action::unroll_resolver::operator () (
		const count_ptr<const guarded_action>& g) const {
	NEVER_NULL(g);
	return g->unroll_resolve_copy(_context, g);
}

//=============================================================================
// class guarded_action method definitions

guarded_action::guarded_action() : persistent(), guard(), stmt() { }

guarded_action::guarded_action(
		const guard_ptr_type& g, const stmt_ptr_type& p) :
		persistent(), guard(g), stmt(p) {
	// guard may be NULL
	// stmt may be NULL
}

guarded_action::~guarded_action() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(guarded_action)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
guarded_action::dump(ostream& o, const expr_dump_context& c) const {
	if (guard)
		guard->dump(o, c);
	else 	o << "else";
	o << " -> ";
	if (stmt)
		return stmt->dump(o, c);
	else 	return o << "skip";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unroll-resolves the data-expression and action.  
 */
guarded_action::unroll_return_type
guarded_action::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const this_type>& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	guard_ptr_type g(NULL);
	if (guard) {
		g = guard->unroll_resolve_copy(c, guard);
		if (!g) {
			cerr << "Error resolving guard expression." << endl;
			return unroll_return_type(NULL);
		}
	}
if (stmt) {
	const action_ptr_type a(stmt->unroll_resolve_copy(c, stmt));
	if (!a) {
		cerr << "Error resolving action statement of guarded action."
			<< endl;
		return unroll_return_type(NULL);
	}
	if (g == guard && a == stmt) {
		// resolving resulted in no change
		return p;
	} else {
		return unroll_return_type(new this_type(g, a));
	}
} else {
	// have a 'skip' statement (NULL)
	if (g == guard) {
		// resolving resulted in no change
		return p;
	} else {
		return unroll_return_type(
			new this_type(g, action_ptr_type(NULL)));
	}
}
}	// end method unroll_resolve_copy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre a slection-merge event is pointed to by s.last_event_index.
	Plan: construct guards in all branches first
	(guarded actions), then move them into a union.  
	It is the selection' (caller) responsibility to collect
		guard-dependencies for subscription sets.  
		The dependencies for the guarded action are computed
		by the action sequence.  
 */
void
guarded_action::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	if (stmt) {
		stmt->accept(s);
	// it is the selection's responsibility to evaluate the guards
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
guarded_action::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	if (guard)	guard->collect_transient_info(m);
	if (stmt)	stmt->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
guarded_action::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, guard);
	m.write_pointer(o, stmt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
guarded_action::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, guard);
	m.read_pointer(i, stmt);
}

//=============================================================================
/**
	Helper routine, to apply copy-on-write unroll-resolving
	from one list of guarded actions to another.  
	\param s the source selection list.
	\param c the unrolling context.
	\param d the destination selection list (appended to).  
	\return bad upon error.  
 */
good_bool
unroll_resolve_selection_list(const selection_list_type& s, 
		const unroll_context& c, selection_list_type& d) {
	typedef	selection_list_type::value_type	ga_ptr_type;
	transform(s.begin(), s.end(), back_inserter(d),
		guarded_action::unroll_resolver(c));
	const selection_list_type::const_iterator
		f(find(d.begin(), d.end(), ga_ptr_type(NULL)));
	if (f != d.end()) {
		return good_bool(false);
	}
	return good_bool(true);
}

//=============================================================================
// class guarded_action::selection_evaluator definition

/**
	Functor for evaluating guarded statements.  
	This class can be given hidden visibility (local to module).  
 */
struct guarded_action::selection_evaluator {
	// operator on selection_list_type::const_reference
	typedef	const count_ptr<const guarded_action>&	argument_type;
	const nonmeta_context_base& 		context;
	/// successor index (induction variable)
	size_t					index;
	/// list of successors whose guards evaluated true (accumulate)
	vector<size_t>				ready;

	explicit
	selection_evaluator(const nonmeta_context_base& c) :
			context(c), index(0), ready() {
		ready.reserve(2);
	}

	void
	operator () (argument_type g) {
		STACKTRACE_CHPSIM_VERBOSE;
		NEVER_NULL(g);
		STACKTRACE_INDENT_PRINT("evaluating guard " << index << endl);
	if (g->guard) {
		const count_ptr<const pbool_const>
			b(g->guard->__nonmeta_resolve_rvalue(
				context, g->guard));
		// error handling
		if (!b) {
			cerr << "Run-time error evaluating guard expression."
				<< endl;
			THROW_EXIT;
		}
		if (b->static_constant_value()) {
			STACKTRACE_INDENT_PRINT("true guard" << endl);
			ready.push_back(index);
		} else {
			STACKTRACE_INDENT_PRINT("false guard" << endl);
		}
	} else {
		// NULL guard is an else clause (in last position)
		STACKTRACE_INDENT_PRINT("else guard" << endl);
		if (ready.empty()) {
			ready.push_back(index);
		}
	}
		++index;
	}	// end operator ()

private:
	typedef	selection_evaluator		this_type;

	/// no-copy or assign
	explicit
	selection_evaluator(const this_type&);

};	// end class selection_evaluator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor reference wrapper.
 */
class guarded_action::selection_evaluator_ref :
		public reference_wrapper<guarded_action::selection_evaluator> {
	typedef	reference_wrapper<guarded_action::selection_evaluator>
				parent_type;
	typedef	parent_type::type::argument_type	argument_type;
public:
	selection_evaluator_ref(reference r) : parent_type(r) { }

	/**
		Forwarding operator to underlying reference.  
	 */
	void
	operator () (argument_type a) {
		get()(a);
	}

};	// end class selection_evaluator_ref

//=============================================================================
// class deterministic_selection method definitions

deterministic_selection::deterministic_selection() :
		parent_type(), list_type() { }

deterministic_selection::~deterministic_selection() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(deterministic_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
deterministic_selection::dump(ostream& o, const expr_dump_context& c) const {
	o << "deterministic: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
deterministic_selection::dump_event(ostream& o, 
		const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
deterministic_selection::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> r(new this_type);
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling deterministic selection." << endl;
		return action_ptr_type(NULL);
	}
	if (equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		return r;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Almost exact same code for non-deterministic selection.
	TODO: static dependency sets of guards.
 */
void
deterministic_selection::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = this->size();
	const size_t merge_index = s.allocate_event(
		EventNode(NULL, SIM::CHPSIM::EVENT_SELECTION_END, 
			s.current_process_index));
	// don't pass this, as that would cause re-evaluation at join node!
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	EventNode& merge_event(s.get_event(merge_index));
	s.connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	s.count_predecessors(merge_event);
}

	const_iterator i(begin()), e(end());
	SIM::CHPSIM::DependenceSetCollector deps(s);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		s.last_event_index = merge_index;	// pass down
		(*i)->accept(s);
		tmp.push_back(s.last_event_index);	// head of each chain
		const guarded_action::guard_ptr_type& g((*i)->get_guard());
		if (g) {
			g->accept(deps);
		} else {
			// is else clause, don't need any guard dependencies!
			deps.clear();
			// TODO: check terminating clause *first*
			// before bothering...
		}
	}

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_SELECTION_BEGIN, 
			s.current_process_index));
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	EventNode& split_event(s.get_event(split_index));
	split_event.import_block_dependencies(deps);

	split_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &split_event.successor_events[0]);

	// updates successors' predecessor-counts
	s.count_predecessors(split_event);
}
	// leave trail of this event for predecessor
	s.last_event_index = split_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Action groups should never be used as leaf events, 
	so this does nothing other than evaluate guards, via recheck().  
	Q: this is checked twice: pre-enqueue, and during execution.
		What if guard is unstable?  and conditions change?
 */
void
deterministic_selection::execute(const nonmeta_context& c, 
		global_reference_array_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
#if 0
	const bool b = recheck(c);
	INVARIANT(b);
#else
	// never enqueues itself, only successors
	// see recheck() below
	ICE_NEVER_CALL(cerr);
#endif
	// violation is possible if guard was true but because
	// false due to concurrent events
	// we should alert user with run-time error
	// TODO: Is it possible to re-subscribe this event for re-checking?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Selections act like a proxy event that dispatches one of its
	successors, but never executes "itself" when unblocked.  
	Action groups should never be used as leaf events, 
	so this does nothing.
	When a successor is ready to enqueue, unsubscribe this event from
	its dependencies.  
	\return false signaling that this event is never enqueued, 
		(only successors are enqueued).
	Q: this is checked twice? A: no, doesn't follow conventional execution
	TODO: test nested selections!
 */
char
deterministic_selection::recheck(const nonmeta_context& c) const {
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(c);	// needs reference wrap
	for_each(begin(), end(), guarded_action::selection_evaluator_ref(G));
	switch (G.ready.size()) {
	case 0: {
		return RECHECK_BLOCKED_THIS;	// no successor to enqueue
		// caller will subscribe this event's dependencies
	}
	case 1: {
		EventNode& t(c.get_event());	// this event
		const size_t ei = t.successor_events[G.ready.front()];
		STACKTRACE_INDENT_PRINT("have a winner! eid: " << ei << endl);
		t.reset_countdown();
		// act like this event (its predecessor) executed
		EventNode::countdown_decrementer(c.event_pool)(ei);
#if 0
		c.rechecks.insert(ei);
		// HERE TODO: subscribe... check event on the spot, don't insert
		// return true or false?
		// never enqueue self?
		// but we also DON'T want to subscribe this event's deps!
#else
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
	}
	default:
		cerr << "Run-time error: multiple exclusive guards of "
			"deterministic selection evaluated TRUE!" << endl;
		THROW_EXIT;
		return RECHECK_BLOCKED_THIS;		// unreachable
	}	// end switch
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
deterministic_selection::dump_successor_edges(ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
// class nondeterministic_selection method definitions

nondeterministic_selection::nondeterministic_selection() :
		parent_type(), list_type() { }

nondeterministic_selection::~nondeterministic_selection() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(nondeterministic_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nondeterministic_selection::dump(ostream& o, const expr_dump_context& c) const {
	o << "nondeterministic: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
nondeterministic_selection::dump_event(ostream& o, 
		const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
nondeterministic_selection::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> r(new this_type);
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling nondeterministic selection." << endl;
		return action_ptr_type(NULL);
	}
	if (equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		return r;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Code ripped from deterministic_selection::accept().
 */
void
nondeterministic_selection::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// TODO: run-time check for guard exclusion
	const size_t branches = this->size();
	const size_t merge_index = s.allocate_event(
		EventNode(NULL, SIM::CHPSIM::EVENT_SELECTION_END, 
			s.current_process_index));
	// don't pass this, as that would cause re-evaluation at join node!
{
	STACKTRACE_INDENT_PRINT("merge index: " << merge_index << endl);
	EventNode& merge_event(s.get_event(merge_index));
	s.connect_successor_events(merge_event);
	merge_event.set_predecessors(1);	// expect ONE branch only
	s.count_predecessors(merge_event);
}

	const_iterator i(begin()), e(end());
	SIM::CHPSIM::DependenceSetCollector deps(s);	// args
	vector<size_t> tmp;
	tmp.reserve(branches);
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		s.last_event_index = merge_index;	// pass down
		(*i)->accept(s);
		tmp.push_back(s.last_event_index);	// head of each chain
		const guarded_action::guard_ptr_type& g((*i)->get_guard());
		if (g) {
			g->accept(deps);
		} else {
			// is else clause, don't need any guard dependencies!
			deps.clear();
			// TODO: check terminating clause *first*
			// before bothering...
		}
	}

	// construct successor event graph edge? or caller's responsibility?
	const size_t split_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_SELECTION_BEGIN, 
			s.current_process_index));
{
	STACKTRACE_INDENT_PRINT("split index: " << split_index << endl);
	EventNode& split_event(s.get_event(split_index));
	split_event.import_block_dependencies(deps);

	split_event.successor_events.resize(branches);
	copy(tmp.begin(), tmp.end(), &split_event.successor_events[0]);

	// updates successors' predecessor-counts
	s.count_predecessors(split_event);
}
	// leave trail of this event for predecessor
	s.last_event_index = split_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: we may want this to behave differently than determinstic sel.
	We may want to introduce a delay window from recheck to enqueue
	so multiple input guards may become true to require arbitration, 
	not just the arrival of the first true guard.
	In this case, we want recheck behavior upon execution.  
	Recheck will be slightly different.  
	Note that the window of delay also allows for the possibility
	of unstable guards upon re-evaluation!
	Deterministic selection does not have this problem, 
	as branch selection occurs immediately.  

	Action groups should never be used as leaf events, 
	so this does nothing other than evaluate guards, via recheck().  
	Q: this is checked twice: pre-enqueue, and during execution.
		What if guard is unstable?  and conditions change?
 */
void
nondeterministic_selection::execute(const nonmeta_context& c, 
		global_reference_array_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
#if 0
	ICE_NEVER_CALL(cerr);
#else
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(c);	// needs reference wrap
	for_each(begin(), end(), guarded_action::selection_evaluator_ref(G));
	const size_t m = G.ready.size();
	EventNode& t(c.get_event());
	switch (m) {
	case 0: {
		// this can happen because guards may be unstable, 
		// and can thus become invalidated between enqueue and execute
		c.subscribe_this_event();	// deduce own event index
		break;
	}
	case 1: {
		const size_t ei = t.successor_events[G.ready.front()];
#if 0
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
#else
		c.rechecks.insert(ei);
		EventNode::countdown_decrementer(c.event_pool)(ei);
#endif
		break;
	}
	default: {
		// pick one at random
		static rand48<unsigned long> rgen;
		const size_t r = rgen();	// random-generate
		const size_t ei = t.successor_events[G.ready[r%m]];
#if 0
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
#else
		c.rechecks.insert(ei);
		EventNode::countdown_decrementer(c.event_pool)(ei);
#endif
	}
	}	// end switch
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: implement this differently, see comment in execute().
	Action groups should never be used as leaf events, 
	so this does nothing.
	When a successor is ready to enqueue, unsubscribe this event from
	its dependencies.  
	\pre selection has no else clause.  
	\return false signaling that this event is never enqueued, 
		(only successors are enqueued).
	Q: this is checked twice?
 */
char
nondeterministic_selection::recheck(const nonmeta_context& c) const {
	// 1) evaluate all clauses, which contain guard expressions
	//	Use functional pass.
	// 2) if exactly one is true, return reference to it as the successor
	//	event to enqueue (not execute right away)
	//	a) if more than one true, signal a run-time error
	//	b) if none are true, and there is an else clause, use it
	//	c) if none are true, without else clause, 'block',
	//		subscribing this event to its set of dependents.  
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(c);	// needs reference wrap
	for_each(begin(), end(), guarded_action::selection_evaluator_ref(G));
#if 0
	const size_t m = G.ready.size();
	EventNode& t(c.get_event());
	switch (m) {
	case 0: {
		// TODO: see determinstic selection
		return RECHECK_BLOCKED_THIS;	// no successor to enqueue
	}
	case 1: {
		const size_t ei = t.successor_events[G.ready.front()];
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
#if 0
		c.rechecks.insert(ei);
		return true;
#else
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
	}
	default: {
		// pick one at random
		static rand48<long> rgen;
		const size_t r = rgen();	// random-generate
		const size_t ei = t.successor_events[G.ready[r%m]];
		t.reset_countdown();
		EventNode::countdown_decrementer(c.event_pool)(ei);
#if 0
		c.rechecks.insert(ei);
		return true;
#else
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
#endif
	}
	}	// end switch
	return RECHECK_BLOCKED_THIS;
#else
	if (G.ready.size()) {
		// defer actual selection until the execution phase, 
		// with some delay
		return RECHECK_UNBLOCKED_THIS;
	} else {
		return RECHECK_BLOCKED_THIS;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nondeterministic_selection::dump_successor_edges(
		ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
// class metaloop_selection method definitions

metaloop_selection::metaloop_selection() :
		action(), meta_loop_base(), body(), selection_type(false) {
}

metaloop_selection::metaloop_selection(const ind_var_ptr_type& i, 
		const range_ptr_type& r, const body_ptr_type& b, 
		const bool t) :
		action(), meta_loop_base(i, r), body(b), selection_type(t) {
	NEVER_NULL(body);
}

metaloop_selection::~metaloop_selection() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(metaloop_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
metaloop_selection::dump(ostream& o, const expr_dump_context& c) const {
	o << (body ? "deterministic " : "nondeterministic ");
	o << ind_var->get_name() << ':';
	range->dump(o, entity::expr_dump_context(c)) <<
		": {" << endl;
	{
		INDENT_SECTION(o);
		body->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
metaloop_selection::dump_event(ostream& o, const expr_dump_context&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This expands a meta-loop into unrolled form.  
	Partially ripped from entity::PRS::rule_loop::unroll().  
 */
action_ptr_type
metaloop_selection::unroll_resolve_copy(const unroll_context& c,
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const_range cr;
	if (!range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, entity::expr_dump_context::default_value)
			<< endl;
		return action_ptr_type(NULL);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	// if range is empty or backwards, then what?
	if (min > max) {
		cerr << "Error: loop range of metaloop_selection is empty."
			<< endl;
		return action_ptr_type(NULL);
	}
	selection_list_type result;	// unroll into here
	entity::footprint f;
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& i(var->get_instance().value);
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (i = min; i <= max; ++i) {
		const selection_list_type::value_type	// guarded_action
			g(body->unroll_resolve_copy(cc, body));
		if (!g) {
			cerr << "Error resolving metaloop_selection at "
				"iteration " << i << "." << endl;
			return action_ptr_type(NULL);
		}
		result.push_back(g);
	}
	if (selection_type) {
		const count_ptr<deterministic_selection>
			ret(new deterministic_selection);
		NEVER_NULL(ret);
		ret->swap(result);
		return ret;
	} else {
		const count_ptr<nondeterministic_selection>
			ret(new nondeterministic_selection);
		NEVER_NULL(ret);
		ret->swap(result);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never supposed to be called, because these are expanded.  
 */
void
metaloop_selection::accept(StateConstructor& s) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
metaloop_selection::execute(const nonmeta_context&, 
		global_reference_array_type&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
char
metaloop_selection::recheck(const nonmeta_context&) const {
	ICE_NEVER_CALL(cerr);
	return RECHECK_BLOCKED_THIS;	// don't care
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	m.write_pointer(o, body);
	write_value(o, selection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::load_object(const persistent_object_manager& m,
		istream& i) {
	meta_loop_base::load_object_base(m, i);
	m.read_pointer(i, body);
	read_value(i, selection_type);
}

//=============================================================================
// class metaloop_statement method definitions

metaloop_statement::metaloop_statement() :
		action(), meta_loop_base(), body(), statement_type(false) {
}

metaloop_statement::metaloop_statement(const ind_var_ptr_type& i, 
		const range_ptr_type& r, const body_ptr_type& b, 
		const bool t) :
		action(), meta_loop_base(i, r), body(b), statement_type(t) {
	NEVER_NULL(body);
}

metaloop_statement::~metaloop_statement() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(metaloop_statement)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
metaloop_statement::dump(ostream& o, const expr_dump_context& c) const {
	o << (body ? "concurrent " : "sequential ");
	o << ind_var->get_name() << ':';
	range->dump(o, entity::expr_dump_context(c)) <<
		": {" << endl;
	{
		INDENT_SECTION(o);
		body->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << '}';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print anything.  Not a single statement/event.  
 */
ostream&
metaloop_statement::dump_event(ostream& o, const expr_dump_context&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This expands a meta-loop into unrolled form.  
	Partially ripped from entity::PRS::rule_loop::unroll().  
 */
action_ptr_type
metaloop_statement::unroll_resolve_copy(const unroll_context& c,
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const_range cr;
	if (!range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, entity::expr_dump_context::default_value)
			<< endl;
		return action_ptr_type(NULL);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	// if range is empty or backwards, then what?
	if (min > max) {
		cerr << "Error: loop range of metaloop_statement is empty."
			<< endl;
		return action_ptr_type(NULL);
	}
	action_list_type result;	// unroll into here
	entity::footprint f;
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& i(var->get_instance().value);
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (i = min; i <= max; ++i) {
		const action_list_type::value_type	// guarded_action
			g(body->unroll_resolve_copy(cc, body));
		if (!g) {
			cerr << "Error resolving metaloop_statement at "
				"iteration " << i << "." << endl;
			return action_ptr_type(NULL);
		}
		result.push_back(g);
	}
	if (statement_type) {
		const count_ptr<concurrent_actions>
			ret(new concurrent_actions);
		NEVER_NULL(ret);
		ret->swap(result);
		return ret;
	} else {
		const count_ptr<action_sequence>
			ret(new action_sequence);
		NEVER_NULL(ret);
		ret->swap(result);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never supposed to be called, because these are expanded.  
 */
void
metaloop_statement::accept(StateConstructor& s) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
void
metaloop_statement::execute(const nonmeta_context&, 
		global_reference_array_type&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, always expanded.  
 */
char
metaloop_statement::recheck(const nonmeta_context&) const {
	ICE_NEVER_CALL(cerr);
	return RECHECK_BLOCKED_THIS;	// don't care
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_VERBOSE;
	meta_loop_base::collect_transient_info_base(m);
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_VERBOSE;
	meta_loop_base::write_object_base(m, o);
	m.write_pointer(o, body);
	write_value(o, statement_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_VERBOSE;
	meta_loop_base::load_object_base(m, i);
	m.read_pointer(i, body);
	read_value(i, statement_type);
}

//=============================================================================
// class assignment method definitions

assignment::assignment() : parent_type(), lval(), rval() { }

assignment::assignment(const lval_ptr_type& l, const rval_ptr_type& r) :
		parent_type(), lval(l), rval(r) {
	NEVER_NULL(lval);
	NEVER_NULL(rval);
}

assignment::~assignment() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(assignment)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
assignment::dump(ostream& o, const expr_dump_context& c) const {
	// const expr_dump_context& c(expr_dump_context::default_value);
	return rval->dump(lval->dump(o, c) << " := ", c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defer to normal dump.  
 */
ostream&
assignment::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
assignment::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const lval_ptr_type lc(lval->unroll_resolve_copy(c, lval));
	const rval_ptr_type rc(rval->unroll_resolve_copy(c, rval));
	if (!lc || !rc) {
		return action_ptr_type(NULL);
	} else if (lc == lval && rc == rval) {
		return p;
	} else {
		return action_ptr_type(new this_type(lc, rc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_ASSIGN, 
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("new assigment: " << new_index << endl);
	EventNode& new_event(s.get_event(new_index));

#if CHPSIM_READ_WRITE_DEPENDENCIES
{
	SIM::CHPSIM::ReadDependenceSetCollector rdeps(s);	// rvalues
	SIM::CHPSIM::WriteDependenceSetCollector wdeps(s);	// lvalues
	rval->accept(rdeps);
	lval->accept(wdeps);
	new_event.import_read_dependencies(rdeps);
	new_event.import_write_dependencies(wdeps);
}
#endif

	s.connect_successor_events(new_event);
	// assignments are atomic and never block
	// thus we need no dependencies.  

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	s.last_event_index = new_index;

	// updates successors' predecessor-counts
	s.count_predecessors(new_event);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	lvalue must be bool, int, or enum reference.  
	\param u collection of references updated by the assignment execution,
		namely, the lvalues.
 */
void
assignment::execute(const nonmeta_context& c,
		global_reference_array_type& u) const {
	typedef	EventNode		event_type;
	STACKTRACE_CHPSIM_VERBOSE;
	lval->nonmeta_assign(rval, c, u);	// also tracks updated reference
	recheck_all_successor_events(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assignments are non-blocking, and thus need no re-evaluation.
 */
char
assignment::recheck(const nonmeta_context&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	// no-op
	return RECHECK_NEVER_BLOCKED;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lval->collect_transient_info(m);
	rval->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, lval);
	m.write_pointer(o, rval);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, lval);
	m.read_pointer(i, rval);
}

//=============================================================================
// class condition_wait method definitions

condition_wait::condition_wait() : parent_type(), cond() { }

condition_wait::condition_wait(const cond_ptr_type& c) :
		parent_type(), cond(c) {
	NEVER_NULL(cond);
	// what about else?
}

condition_wait::~condition_wait() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(condition_wait)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
condition_wait::dump(ostream& o, const expr_dump_context& c) const {
	return cond->dump(o << '[', c) << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defer to normal dump.  
 */
ostream&
condition_wait::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
condition_wait::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const cond_ptr_type g(cond->unroll_resolve_copy(c, cond));
	if (!g) {
		cerr << "Error resolving condition-wait." << endl;
		return action_ptr_type(NULL);
	} else if (g == cond) {
		return p;
	} else {
		return action_ptr_type(new this_type(g));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: alternative: fuse this event with successor if single.  
		rationale: every CHPSIM event is "guarded"
	TODO: what if several conditional waits occur in succession?
		Take conjunction or sequential evaluation
		using auxiliary null events?
 */
void
condition_wait::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// register guard expression dependents
	// construct successor event graph edge? or caller's responsibility?
	const size_t new_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_NULL, 
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("wait index: " << new_index << endl);
	EventNode& new_event(s.get_event(new_index));
	if (cond) {
		SIM::CHPSIM::DependenceSetCollector deps(s);
		cond->accept(deps);
		new_event.import_block_dependencies(deps);
	}

	s.connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	s.last_event_index = new_index;

	// updates successors' predecessor-counts
	s.count_predecessors(new_event);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing, is a NULL event.  
	NOTE: it is possible that guard is no longer true, 
		as it may be invalidated since the time it was enqueued.
		We do not check for guard stability... yet.  
 */
void
condition_wait::execute(const nonmeta_context& c, 
		global_reference_array_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	recheck_all_successor_events(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: (Q?) condition-wait is currently non-atomic, see execute() note.
	If we wanted true atomicity, then we would have the recheck()
	enqueue the *succcessor* events, rather than itself, just like
	deterministic_selection::recheck().  

	The 'guarded' action is a NULL event, which can always occur.  
	The guard expression is already checked by the caller
	as a part of event processing.  
 */
char
condition_wait::recheck(const nonmeta_context& c) const {
	STACKTRACE_CHPSIM_VERBOSE;
	if (cond) {
		// TODO: decide error handling via exceptions?
		const count_ptr<const pbool_const>
			g(cond->__nonmeta_resolve_rvalue(c, cond));
		if (!g) {
			cerr << "Failure resolving run-time value of "
				"boolean expression: ";
			cond->dump(cerr,
				expr_dump_context::default_value) << endl;
			// temporary
			THROW_EXIT;
		}
		return g->static_constant_value() ?
			RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
	} else {
		return RECHECK_UNBLOCKED_THIS;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	cond->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, cond);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, cond);
}

//=============================================================================
/**
	\param ncr the nonmeta channel reference to resolve.
	\param c the unroll context.
	\param d true for send, false for receive.
 */
good_bool
set_channel_alias_directions(
		const simple_channel_nonmeta_instance_reference& ncr, 
		const unroll_context& c, const bool d) {
	// flag that a channel(s) is connected to producers
	// recall that nonmeta references MUST be scalar..
	const never_ptr<const nonmeta_index_list> ind(ncr.get_indices());
	const count_ptr<dynamic_meta_index_list>
		mil(ind ? ind->make_meta_index_list() :
			count_ptr<dynamic_meta_index_list>(NULL));
	if (ind && !mil) {
		// there was at least one non-meta index
		// we flag all members of the array as nonmeta-accessed
		const never_ptr<channel_instance_collection>
			cic(c.lookup_instance_collection(
				*ncr.get_inst_base_subtype())
				.is_a<channel_instance_collection>());
		// can collection reference subinstance-actuals/members?
		NEVER_NULL(cic);
		// remember to use canonical aliases!
		if (!cic->set_alias_connection_flags(
			d ? directional_connect_policy<true>::
				CONNECTED_TO_CHP_NONMETA_PRODUCER
			: directional_connect_policy<true>::
				CONNECTED_TO_CHP_NONMETA_CONSUMER
			).good) {
			ncr.dump(cerr << "Error referencing ", 
				expr_dump_context::default_value)
				<< "." << endl;
			return good_bool(false);
		}
	} else {
		// should already be resolved to constants (or NULL)
		// construct an auxiliary meta-instance reference
		// to resolve the reference.  
		const simple_channel_meta_instance_reference
			cr(ncr.get_inst_base_subtype(), mil);
		const never_ptr<channel_instance_alias_info>
			ca(cr.unroll_generic_scalar_reference(c));
		if (!ca) {
			cerr << "Error: reference to uninstantiated "
				"channel alias." << endl;
			return good_bool(false);
		}
		// remember to use canonical alias!
		// can this result in an error? yes!
		if (!ca->find()->set_connection_flags(
				d ? directional_connect_policy<true>::
					CONNECTED_TO_CHP_META_PRODUCER
				: directional_connect_policy<true>::
					CONNECTED_TO_CHP_META_CONSUMER
					).good) {
			ncr.dump(cerr << "Error referencing ", 
				expr_dump_context::default_value)
				<< "." << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}	// end function set_channel_alias_directions

//=============================================================================
// class channel_send method definitions

channel_send::channel_send() : parent_type(), chan(), exprs() { }

channel_send::channel_send(const chan_ptr_type& c) :
		parent_type(), chan(c), exprs() {
	NEVER_NULL(chan);
	// what about else?
}

channel_send::~channel_send() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(channel_send)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_send::dump(ostream& o, const expr_dump_context& c) const {
	typedef	expr_list_type::const_iterator	const_iterator;
	// const expr_dump_context& c(expr_dump_context::default_value);
	chan->dump(o, c) << "!(";
	INVARIANT(!exprs.empty());
	const_iterator i(exprs.begin());
	const const_iterator e(exprs.end());
	(*i)->dump(o, c);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ',', c);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defer to normal dump.  
 */
ostream&
channel_send::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
channel_send::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const chan_ptr_type cc(chan->unroll_resolve_copy(c, chan));
	expr_list_type exprs_c;
	transform(exprs.begin(), exprs.end(), back_inserter(exprs_c), 
		data_expr_unroll_resolver(c));
	if (!cc) {
		cerr << "Error resolving channel reference of send." << endl;
		return action_ptr_type(NULL);
	}
#if 1
	// pass true to indicate send
	if (!set_channel_alias_directions(*cc, c, true).good) {
		// diagnostic?
		return action_ptr_type(NULL);
	}
#endif
	typedef	expr_list_type::const_iterator	const_iterator;
	const const_iterator
		f(find(exprs_c.begin(), exprs_c.end(), data_ptr_type(NULL)));
	if (f != exprs_c.end()) {
		cerr << "At least one error resolving arguments of send."
			<< endl;
		return action_ptr_type(NULL);
	}
	if (cc == chan && equal(exprs.begin(), exprs.end(), exprs_c.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
		const count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->exprs.swap(exprs_c);	// faster than copying/assigning
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Only the channel is 'modified' by a send, so we register it
	with the update set.  
	\throws an exception if anything goes wrong with expression
		evaluation.  
 */
void
channel_send::execute(const nonmeta_context& c, 
		global_reference_array_type& u) const {
	STACKTRACE_CHPSIM_VERBOSE;
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
#if 0
	// don't need
	const global_entry<channel_tag>&
		ch(c.sm->get_pool<channel_tag>()[chan_index]);
#endif
	// evaluate rvalues of channel send statement (may throw!)
	// write to the ChannelState using canonical_fundamental_type
	for_each(exprs.begin(), exprs.end(), 
		nonmeta_expr_evaluator_channel_writer(c, nc));
	// track the updated-reference (channel)
	// expressions are only read, no lvalue data modified
	u.push_back(std::make_pair(
		size_t(entity::META_TYPE_CHANNEL), chan_index));
	NEVER_NULL(nc.can_send());	// else run-time exception
	nc.send();
	recheck_all_successor_events(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue event if it is ready to execute.  
	\return true if this event can be unblocked and enqueued for execution.
 */
char
channel_send::recheck(const nonmeta_context& c) const {
	STACKTRACE_CHPSIM_VERBOSE;
	// see if referenced channel is ready to send
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	const ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
	return nc.can_send() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const size_t new_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_SEND, 
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("send index: " << new_index << endl);
	EventNode& new_event(s.get_event(new_index));

{
	// can block on channel, so we add dependencies
	SIM::CHPSIM::DependenceSetCollector deps(s);
	chan->accept(deps);
	new_event.import_block_dependencies(deps);
}

	s.connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	s.last_event_index = new_index;

	// updates successors' predecessor-counts
	s.count_predecessors(new_event);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	chan->collect_transient_info(m);
	m.collect_pointer_list(exprs);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, chan);
	m.write_pointer_list(o, exprs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, chan);
	m.read_pointer_list(i, exprs);
}

//=============================================================================
// class channel_receive method definitions

channel_receive::channel_receive() : parent_type(), chan(), insts() { }

channel_receive::channel_receive(const chan_ptr_type& c) :
		parent_type(), chan(c), insts() {
	NEVER_NULL(chan);
	// what about else?
}

channel_receive::~channel_receive() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(channel_receive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_receive::dump(ostream& o, const expr_dump_context& c) const {
	typedef	inst_ref_list_type::const_iterator	const_iterator;
	// const expr_dump_context& c(expr_dump_context::default_value);
	chan->dump(o, c) << "?(";
	INVARIANT(!insts.empty());
	const_iterator i(insts.begin());
	const const_iterator e(insts.end());
	(*i)->dump(o, c);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ',', c);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defer to normal dump.  
 */
ostream&
channel_receive::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
channel_receive::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const chan_ptr_type cc(chan->unroll_resolve_copy(c, chan));
	inst_ref_list_type refs;
	transform(insts.begin(), insts.end(), back_inserter(refs), 
		data_ref_unroll_resolver(c));
	if (!cc) {
		cerr << "Error resolving channel reference of send." << endl;
		return action_ptr_type(NULL);
	}
#if 1
	// pass false to indicate receive
	if (!set_channel_alias_directions(*cc, c, false).good) {
		// diagnostic?
		return action_ptr_type(NULL);
	}
#endif
	typedef	inst_ref_list_type::const_iterator	const_iterator;
	const const_iterator
		f(find(refs.begin(), refs.end(), inst_ref_ptr_type(NULL)));
	if (f != refs.end()) {
		cerr << "At least one error resolving arguments of send."
			<< endl;
		return action_ptr_type(NULL);
	}
	if (cc == chan && equal(insts.begin(), insts.end(), refs.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
		count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->insts.swap(refs);	// faster than copying/assigning
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// atomic event
	// construct event graph
	const size_t new_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_RECEIVE, 
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("receive index: " << new_index << endl);
	EventNode& new_event(s.get_event(new_index));

{
	// can block on channel, so we add dependencies
	SIM::CHPSIM::DependenceSetCollector deps(s);
	chan->accept(deps);
	new_event.import_block_dependencies(deps);
}
	s.connect_successor_events(new_event);

	// leave trail of this event for predecessor
	// s.last_event_indices.resize(1);
	// s.last_event_indices[0] = new_index;
	s.last_event_index = new_index;

	// updates successors' predecessor-counts
	s.count_predecessors(new_event);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Both the channel and lvalues are 'modified' by a receive, 
	so we register them all with the update set.  
 */
void
channel_receive::execute(const nonmeta_context& c, 
		global_reference_array_type& u) const {
	STACKTRACE_CHPSIM_VERBOSE;
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
#if 0
	// don't need
	const global_entry<channel_tag>&
		ch(c.sm->get_pool<channel_tag>()[chan_index]);
#endif
	// evaluate lvalues of channel receive statement (may throw!)
	// read from the ChannelState using canonical_fundamental_type
	for_each(insts.begin(), insts.end(), 
		nonmeta_reference_lookup_channel_reader(c, nc, u));
	// track the updated-reference (channel)
	u.push_back(std::make_pair(
		size_t(entity::META_TYPE_CHANNEL), chan_index));
	INVARIANT(nc.can_receive());	// else run-time exception
	nc.receive();
	recheck_all_successor_events(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enqueue event if it is ready to execute.  
	\return true if this event can be unblocked and enqueued for execution.
 */
char
channel_receive::recheck(const nonmeta_context& c) const {
	STACKTRACE_CHPSIM_VERBOSE;
	// see if referenced channel is ready to receive
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	STACKTRACE_INDENT_PRINT("chan index " << chan_index << endl);
	const ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
	return nc.can_receive() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	chan->collect_transient_info(m);
	m.collect_pointer_list(insts);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, chan);
	m.write_pointer_list(o, insts);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, chan);
	m.read_pointer_list(i, insts);
}

//=============================================================================
// class do_forever_loop method definitions

do_forever_loop::do_forever_loop() : parent_type(), body() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
do_forever_loop::do_forever_loop(const body_ptr_type& b) :
		parent_type(), body(b) {
	NEVER_NULL(body);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
do_forever_loop::~do_forever_loop() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(do_forever_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
do_forever_loop::dump(ostream& o, const expr_dump_context& c) const {
	o << "*[" << endl;
	{
		INDENT_SECTION(o);
		body->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print, not a single statement/event.  
 */
ostream&
do_forever_loop::dump_event(ostream& o, const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
do_forever_loop::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const action_ptr_type b(body->unroll_resolve_copy(c, body));
	if (!b) {
		cerr << "Error resolving do-forever loop." << endl;
		return action_ptr_type(NULL);
	}
	if (b == body) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		return action_ptr_type(new this_type(b));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: nothing can follow a do-forever loop, 
	so we need not worry about an initial successor.  
	However, there may be entries into an infinite loop, so we
	must return the index to the first event in the loop.  
	TODO: optimization: overwrite the loopback null event slot
		if the event is trivial -- may result in self-reference, OK.
 */
void
do_forever_loop::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	// OR use the 0th event slot as the dummy!
	// -- works only if we need one dummy at a time
	const size_t loopback_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_NULL,
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("forever loopback: " << loopback_index << endl);
	s.last_event_index = loopback_index;	// point to dummy, pass down
	body->accept(s);
	// never blocks, no need for dependency checking
{
	// find last event and loop it back to the beginning
	EventNode& loopback_event(s.get_event(loopback_index));
	EventNode& head_event(s.get_event(s.last_event_index));
	STACKTRACE_INDENT_PRINT("head of body: " << s.last_event_index << endl);
// some redundant operations going on here...
	// re-link loop
	loopback_event.successor_events.resize(1);
	loopback_event.successor_events[0] = s.last_event_index;
#if OPTIMIZE_CHPSIM_EVENTS
/***
	Two exclusive strategies possible from here:
	1) move the back event to the loopback placeholder slot, 
		this way minimizes dead-events in the middle.
		We do this only if it is 'convenient', i.e.
		the back event is trivial and has a lone successor.  
	2) just free the loopback placeholder, by forwarding around it, 
		return a reference to the head_event
***/
	const size_t back_index = s.event_pool_size() -1;
	EventNode& back_event(s.get_event(back_index));
	STACKTRACE_INDENT_PRINT("considering back: " << back_index << endl);
if (back_event.is_dispensible()) {
	// 2) recycle the back event, involves re-linking up to two events
	// Q: does the loopback already point to back? (corner case)
	if (back_index != loopback_event.successor_events[0]) {
		s.forward_successor(loopback_index);	// pointers to loopback
	}
	s.forward_successor(back_index, loopback_index, s.last_event_index);
	// INVARIANT(s.last_event_index != back_index);
	loopback_event = back_event;	// MOVE into placeholder slot!
	STACKTRACE_INDENT_PRINT("recycling back: " << back_index << endl);
	s.deallocate_event(back_index);		// recycle it!
	if (back_index == s.last_event_index) {
		STACKTRACE_INDENT_PRINT("back is s.last_event_index" << endl);
	// need to update return value (head index), using loopback slot
		s.last_event_index = loopback_index;
	}
} else {
	// 1) then we can recycle the loopback slot safely (tested)
	// first: forward successors through condemned event
	const size_t ret = s.forward_successor(loopback_index);
	INVARIANT(ret == s.last_event_index);
	// last: free the condemned event
	s.deallocate_event(loopback_index);		// recycle it!
	// loopback_event is now dead
	STACKTRACE_INDENT_PRINT("recycling loopback: " << loopback_index << endl);
//	s.last_event_index = ret;	// redundant
}
#endif	// OPTIMIZE_CHPSIM_EVENTS
	head_event.set_predecessors(1);	// but may have multiple entries
	// caller will count_predecessors
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op, this should never be called from simulator, as loop
	body events are expanded.  
 */
void
do_forever_loop::execute(const nonmeta_context&, 
		global_reference_array_type&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char
do_forever_loop::recheck(const nonmeta_context&) const {
	ICE_NEVER_CALL(cerr);
	return RECHECK_BLOCKED_THIS;	// don't care
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, body);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, body);
}

//=============================================================================
// class do_while_loop method definitions

do_while_loop::do_while_loop() :
		parent_type(), list_type() { }

do_while_loop::~do_while_loop() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(do_while_loop)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
do_while_loop::dump(ostream& o, const expr_dump_context& c) const {
	o << "*[" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent, c) << endl;
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't print, not a single statement/event.  
 */
ostream&
do_while_loop::dump_event(ostream& o, const expr_dump_context&) const {
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
do_while_loop::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> r(new this_type);
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling do-while loop." << endl;
		return action_ptr_type(NULL);
	}
	if (equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		return r;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Code ripped from do_forever_loop::accept().
	Need to synthesize a deterministic selection with an exit branch.
	Semantics: never blocking, as there is an implicit else-clause
	that skips past the loop.  
	Reminder: guards cannot include an explicit else clause.  
 */
void
do_while_loop::accept(StateConstructor& s) const {
	STACKTRACE_VERBOSE;
	// construct cyclic event graph
	// create a dummy event first (epilogue) and loop it around.
	const size_t branches = this->size();
	const size_t loopback_index = s.allocate_event(
		EventNode(this, SIM::CHPSIM::EVENT_SELECTION_BEGIN,
			s.current_process_index));
	STACKTRACE_INDENT_PRINT("do-while loopback index: "
		<< loopback_index << endl);
{
	EventNode& loopback_event(s.get_event(loopback_index));
	loopback_event.successor_events.resize(branches +1);
	if (s.last_event_index) {
		// exit
		loopback_event.successor_events[branches] = s.last_event_index;
	} else {
		// there was no successor, create a terminator
		const size_t terminal_index = s.allocate_event(
			EventNode(NULL, SIM::CHPSIM::EVENT_NULL,
				s.current_process_index));
		// loopback_event reference invalidated by push_back
		s.get_event(loopback_index).successor_events[branches]
			= terminal_index;
		STACKTRACE_INDENT_PRINT("new terminal index: "
			<< terminal_index << endl);
	}
	// convention: 
	// 1st events will go into the body of the do-while loop
	// last event will be the exit branch, corresponding to the else clause
}
	// NOTE: no need to add guard dependencies because
	// action is always taken immediately (implicit else-clause skips body)
	vector<size_t> tmp;
	tmp.reserve(branches);
{
	const_iterator i(begin()), e(end());
	// construct concurrent chains
	for ( ; i!=e; ++i) {
		s.last_event_index = loopback_index;	// pass down
		(*i)->accept(s);		// guarded actions
		tmp.push_back(s.last_event_index);	// head of each chain
	}
}
{
	// find last event and loop it back to the beginning
	EventNode& loopback_event(s.get_event(loopback_index));
	copy(tmp.begin(), tmp.end(), &loopback_event.successor_events[0]);
#if ENABLE_STACKTRACE
	cerr << "tmp: ";
	copy(tmp.begin(), tmp.end(), std::ostream_iterator<size_t>(cerr, ","));
	cerr << endl;
	cerr << "else: " << loopback_event.successor_events[branches] << endl;
#endif
	// loopback_event.successor_events[0] = s.last_event_index;
	// s.last_event_index now points to first action(s) in loop
	// EventNode& head_event(event_pool[s.last_event_index]);
	// head_event.set_predecessors(1);	// but may have multiple entries
	s.count_predecessors(loopback_event);
	s.last_event_index = loopback_index;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate the guards immediately.
	If evaluation is true, execute the body branch, else take the
	else-clause (exit) successor, enumerated as the last branch.
	NOTE: there's no else clause.
	\return true, as this node is never blocking.  
 */
void
do_while_loop::execute(const nonmeta_context& c,
		global_reference_array_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	guarded_action::selection_evaluator G(c);	// needs reference wrap
	for_each(begin(), end(), guarded_action::selection_evaluator_ref(G));
	size_t si = 0;
	switch (G.ready.size()) {
	case 0:	si = size();
		// this case should never be reached because the else-clause
		// has been expanded and is always evaluated true
		STACKTRACE_INDENT_PRINT("no guards true, fall-through " << si << endl);
		break;
	case 1:	si = G.ready.front();
		STACKTRACE_INDENT_PRINT("guard true, taking successor " << si << endl);
		break;
	default:
		cerr << "Run-time error: multiple exclusive guards of "
			"do-while-loop evaluated TRUE!" << endl;
		THROW_EXIT;
	}	// end switch
	const size_t ei = c.get_event().successor_events[si];
	c.rechecks.insert(ei);
	EventNode::countdown_decrementer(c.event_pool)(ei);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This event never blocks and thus never needs to be rechecked for 
	unblocking because the loop-condition comes with an implicit
	else-clause which is taken if the evaluated condition is false.  
	\return true because this event never rechecks itself (never blocks), 
		only selects a succcessor.  
 */
char
do_while_loop::recheck(const nonmeta_context& c) const {
	STACKTRACE_CHPSIM_VERBOSE;
	return RECHECK_NEVER_BLOCKED;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
do_while_loop::dump_successor_edges(
		ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

