/**
	\file "Object/lang/CHP.cc"
	Class implementations of CHP objects.  
	$Id: CHP.cc,v 1.22.2.1 2007/04/29 05:56:28 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	ENABLE_STACKTRACE_CHPSIM		(0 && ENABLE_STACKTRACE)

#include <iterator>
#include <algorithm>
#include <exception>
#include "Object/lang/CHP.h"
#include "Object/lang/CHP_visitor.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#if !CHPSIM_VISIT_EXECUTE
#include "Object/expr/pbool_const.h"
#endif
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
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/preal_const.h"
#include "Object/def/template_formals_manager.h"
#if !CHPSIM_VISIT_EXECUTE
#include "Object/nonmeta_context.h"
#include "Object/state_manager.h"
#include "Object/global_channel_entry.h"
#include "Object/nonmeta_channel_manipulator.h"
#endif

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/visitor_functor.h"
#include "util/indent.h"
#include "util/IO_utils.tcc"
#if !CHPSIM_VISIT_EXECUTE
#include "util/STL/valarray_iterator.h"
#include "util/reference_wrapper.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/numeric/random.h"	// for rand48
#endif

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
using std::copy;
using std::find;
using std::transform;
using std::back_inserter;
using std::for_each;
#if !CHPSIM_VISIT_EXECUTE
using util::set_inserter;
#endif
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
#if !CHPSIM_VISIT_EXECUTE
using util::reference_wrapper;
using util::numeric::rand48;
#endif
using entity::preal_const;

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
#if !CHPSIM_VISIT_EXECUTE
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
#endif

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
// default destructor
action::action() : persistent(), delay() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action::action(const attributes_type& a) : persistent(), delay(a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default destructor
action::~action() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action::set_delay(const delay_ptr_type& d) {
	delay = d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
action::dump_attributes(ostream& o, const expr_dump_context& d) const {
	if (delay) {
		delay->dump(o << "[after=", d) << "] ";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prefix dump_event with after-attribute.
 */
ostream&
action::dump_event_with_attributes(ostream& o, 
		const expr_dump_context& d) const {
	dump_attributes(o, d);
	return this->dump_event(o, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	By default print all successor edge, unadorned.  
 */
ostream&
action::dump_successor_edges(ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context&) const {
	return e.dump_successor_edges_default(o, i);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action::collect_transient_info_base(persistent_object_manager& m) const {
	if (delay)
		delay->collect_transient_info(m);
}

void
action::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, delay);
}

//=============================================================================
// class attribute method definitions

attribute::attribute(const string& k, const value_type& v) :
		_key(k), _value(v) {
	NEVER_NULL(v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
attribute::~attribute() { }

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
	dump_attributes(o, c);
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
// consider using __unroll_resolve_rvalue -> preal_const
#define	UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)				\
	if (delay) {							\
		const attributes_type					\
			atts(delay->unroll_resolve_copy(c, delay));	\
		if (!atts) {						\
			cerr << "Error unrolling attributes." << endl;	\
			return action_ptr_type(NULL);			\
		}							\
		ret->set_delay(atts);					\
	}
	UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
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
	if (
		// eventually to attribute list pointer comparison
		(delay == ret->delay) && 
		equal(begin(), end(), ret->begin()))
		return p;
	else	return ret;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Sequences should never be used as leaf events, 
	so this does nothing.  
 */
void
action_sequence::execute(const nonmeta_context&, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
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
	parent_type::write_object_base(m, o);
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
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
concurrent_actions::dump(ostream& o, const entity::footprint& /* f */,
		const expr_dump_context& c) const {
if (!empty()) {
	o << auto_indent;
	dump_attributes(o, c);
	o << "resolved concurrent actions:" << endl;
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
	UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
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
void
concurrent_actions::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Action groups should never be used as leaf events, 
	so this does nothing.  
	Conditionally enqueue all successors, blocking on ones that are not
	ready to execute.  
 */
void
concurrent_actions::execute(const nonmeta_context& c, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
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
	parent_type::write_object_base(m, o);
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
	parent_type::load_object_base(m, i);
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
	Only print: G -> ...
 */
ostream&
guarded_action::dump_brief(ostream& o, const expr_dump_context& c) const {
	if (guard)
		guard->dump(o, c);
	else 	o << "else";
	return o << " -> ...";
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
void
guarded_action::accept(chp_visitor& v) const {
	v.visit(*this);
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
#if !CHPSIM_VISIT_EXECUTE
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
#endif

//=============================================================================
// class deterministic_selection method definitions

deterministic_selection::deterministic_selection() :
		parent_type(), list_type() { }

deterministic_selection::~deterministic_selection() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(deterministic_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
deterministic_selection::dump(ostream& o, const expr_dump_context& c) const {
	dump_attributes(o, c);
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
		const expr_dump_context& c) const {
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	// want to print some shorthand for selection event...
	// [G1 -> ... [] G2 -> ... ]
	const_iterator i(begin());
	const const_iterator e(end());
	INVARIANT(i!=e);
	(*i)->dump_brief(o << "[ ", c);
	for (++i; i!=e; ++i)
		(*i)->dump_brief(o << " [] ", c);
	return o << " ]";
#else
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
deterministic_selection::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> r(new this_type);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
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
void
deterministic_selection::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Action groups should never be used as leaf events, 
	so this does nothing other than evaluate guards, via recheck().  
	Q: this is checked twice: pre-enqueue, and during execution.
		What if guard is unstable?  and conditions change?
 */
void
deterministic_selection::execute(const nonmeta_context& c, 
		execute_arg_type&) const {
	STACKTRACE_CHPSIM_VERBOSE;
	// never enqueues itself, only successors
	// see recheck() below
	ICE_NEVER_CALL(cerr);
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
		// recheck it on the spot
		EventNode& suc(c.event_pool[ei]);
		const nonmeta_context::event_setter x(c, &suc);
		// temporary, too lazy to copy, will restore upon destruction
		suc.recheck(c, ei);
		return RECHECK_DEFERRED_TO_SUCCESSOR;
	}
	default:
		cerr << "Run-time error: multiple exclusive guards of "
			"deterministic selection evaluated TRUE!" << endl;
		THROW_EXIT;
		return RECHECK_BLOCKED_THIS;		// unreachable
	}	// end switch
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
ostream&
deterministic_selection::dump_successor_edges(ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
		const expr_dump_context& c) const {
#if CHPSIM_DELAYED_SUCCESSOR_CHECKS
	// want to print some shorthand for selection event...
	// [G1 -> ... : G2 -> ... ]
	const_iterator i(begin());
	const const_iterator e(end());
	INVARIANT(i!=e);
	(*i)->dump_brief(o << "[ ", c);
	for (++i; i!=e; ++i)
		(*i)->dump_brief(o << " : ", c);
	return o << " ]";
#else
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
nondeterministic_selection::unroll_resolve_copy(const unroll_context& c, 
		const action_ptr_type& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> r(new this_type);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
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
void
nondeterministic_selection::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
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
		execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
ostream&
nondeterministic_selection::dump_successor_edges(
		ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	} else {
		const count_ptr<nondeterministic_selection>
			ret(new nondeterministic_selection);
		NEVER_NULL(ret);
		ret->swap(result);
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Never called, always expanded.  
 */
void
metaloop_selection::execute(const nonmeta_context&, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	parent_type::write_object_base(m, o);
	m.write_pointer(o, body);
	write_value(o, selection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_selection::load_object(const persistent_object_manager& m,
		istream& i) {
	meta_loop_base::load_object_base(m, i);
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	} else {
		const count_ptr<action_sequence>
			ret(new action_sequence);
		NEVER_NULL(ret);
		ret->swap(result);
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Never called, always expanded.  
 */
void
metaloop_statement::execute(const nonmeta_context&, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_VERBOSE;
	meta_loop_base::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_VERBOSE;
	meta_loop_base::write_object_base(m, o);
	parent_type::write_object_base(m, o);
	m.write_pointer(o, body);
	write_value(o, statement_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
metaloop_statement::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_VERBOSE;
	meta_loop_base::load_object_base(m, i);
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
// consider using __unroll_resolve_rvalue -> preal_const
#define	UNROLL_COPY_ATTRIBUTES						\
	attributes_type atts;						\
	if (delay) {							\
		atts = delay->unroll_resolve_copy(c, delay);		\
		if (!atts) {						\
			cerr << "Error unrolling attributes." << endl;	\
			return action_ptr_type(NULL);			\
		}							\
	}
	UNROLL_COPY_ATTRIBUTES
	if (!lc || !rc) {
		return action_ptr_type(NULL);
	} else if (
		(delay == atts) &&
		(lc == lval) && (rc == rval)) {
		return p;
	} else {
		const count_ptr<this_type> ret(new this_type(lc, rc));
		ret->set_delay(atts);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	lvalue must be bool, int, or enum reference.  
	\param u collection of references updated by the assignment execution,
		namely, the lvalues.
 */
void
assignment::execute(const nonmeta_context& c, execute_arg_type& u) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	lval->collect_transient_info(m);
	rval->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, lval);
	m.write_pointer(o, rval);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
assignment::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
	UNROLL_COPY_ATTRIBUTES
	if (!g) {
		cerr << "Error resolving condition-wait." << endl;
		return action_ptr_type(NULL);
	} else if (
		(delay == atts) &&
		(g == cond)) {
		return p;
	} else {
		const count_ptr<this_type> ret(new this_type(g));
		ret->set_delay(atts);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Does nothing, is a NULL event.  
	NOTE: it is possible that guard is no longer true, 
		as it may be invalidated since the time it was enqueued.
		We do not check for guard stability... yet.  
 */
void
condition_wait::execute(const nonmeta_context& c, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	cond->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, cond);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
condition_wait::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
	UNROLL_COPY_ATTRIBUTES
	typedef	expr_list_type::const_iterator	const_iterator;
	const const_iterator
		f(find(exprs_c.begin(), exprs_c.end(), data_ptr_type(NULL)));
	if (f != exprs_c.end()) {
		cerr << "At least one error resolving arguments of send."
			<< endl;
		return action_ptr_type(NULL);
	}
	if ((cc == chan) && 
		(atts == delay) &&
			equal(exprs.begin(), exprs.end(), exprs_c.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
		const count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->exprs.swap(exprs_c);	// faster than copying/assigning
		ret->set_delay(atts);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Only the channel is 'modified' by a send, so we register it
	with the update set.  
	\throws an exception if anything goes wrong with expression
		evaluation.  
 */
void
channel_send::execute(const nonmeta_context& c, execute_arg_type& u) const {
	STACKTRACE_CHPSIM_VERBOSE;
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
#define	ASSERT_CHAN_INDEX						\
	if (!chan_index) {						\
		chan->dump(cerr << "at: ",				\
			expr_dump_context::default_value) << endl;	\
		THROW_EXIT;						\
	}
	ASSERT_CHAN_INDEX
	ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
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
	ASSERT_CHAN_INDEX
	const ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
	return nc.can_send() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	chan->collect_transient_info(m);
	m.collect_pointer_list(exprs);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, chan);
	m.write_pointer_list(o, exprs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_send::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
	UNROLL_COPY_ATTRIBUTES
	typedef	inst_ref_list_type::const_iterator	const_iterator;
	const const_iterator
		f(find(refs.begin(), refs.end(), inst_ref_ptr_type(NULL)));
	if (f != refs.end()) {
		cerr << "At least one error resolving arguments of send."
			<< endl;
		return action_ptr_type(NULL);
	}
	if ((cc == chan) &&
		(atts == delay) && 
			equal(insts.begin(), insts.end(), refs.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
		count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->insts.swap(refs);	// faster than copying/assigning
		ret->set_delay(atts);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Assigns the 'fields' of the channel and flips the (lock) state bit.  
	Both the channel and lvalues are 'modified' by a receive, 
	so we register them all with the update set.  
 */
void
channel_receive::execute(const nonmeta_context& c, execute_arg_type& u) const {
	STACKTRACE_CHPSIM_VERBOSE;
	const size_t chan_index = chan->lookup_nonmeta_global_index(c);
	ASSERT_CHAN_INDEX
	ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
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
	ASSERT_CHAN_INDEX
	const ChannelState& nc(c.values.get_pool<channel_tag>()[chan_index]);
	return nc.can_receive() ? RECHECK_UNBLOCKED_THIS : RECHECK_BLOCKED_THIS;
}

#undef	ASSERT_CHAN_INDEX
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	chan->collect_transient_info(m);
	m.collect_pointer_list(insts);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, chan);
	m.write_pointer_list(o, insts);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
	UNROLL_COPY_ATTRIBUTES
	if (
		(atts == delay) && 
		(b == body)) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		const count_ptr<this_type> ret(new this_type(b));
		ret->set_delay(atts);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	No-op, this should never be called from simulator, as loop
	body events are expanded.  
 */
void
do_forever_loop::execute(const nonmeta_context&, execute_arg_type&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char
do_forever_loop::recheck(const nonmeta_context&) const {
	ICE_NEVER_CALL(cerr);
	return RECHECK_BLOCKED_THIS;	// don't care
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	body->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer(o, body);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
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
	dump_attributes(o, c);
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
	NEVER_NULL(r);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling do-while loop." << endl;
		return action_ptr_type(NULL);
	}
	if (
		(delay == r->delay) &&
		equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
		// return newly constructed copy
		return r;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
/**
	Evaluate the guards immediately.
	If evaluation is true, execute the body branch, else take the
	else-clause (exit) successor, enumerated as the last branch.
	NOTE: there's no else clause.
	\return true, as this node is never blocking.  
 */
void
do_while_loop::execute(const nonmeta_context& c, execute_arg_type&) const {
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !CHPSIM_VISIT_EXECUTE
ostream&
do_while_loop::dump_successor_edges(
		ostream& o, const EventNode& e, 
		const size_t i, const expr_dump_context& c) const {
	return dump_selection_successor_edges(*this, o, e, i, c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

