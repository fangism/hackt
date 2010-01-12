/**
	\file "Object/lang/CHP.cc"
	Class implementations of CHP objects.  
	$Id: CHP.cc,v 1.30.14.1 2010/01/12 02:48:50 fang Exp $
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
// #include "Object/expr/nonmeta_expr_list.h"
#include "Object/expr/nonmeta_func_call.h"
#include "Object/expr/dynamic_meta_index_list.h"
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
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/global_channel_entry.h"
#endif
#if CHP_ACTION_PARENT_LINK
#include <functional>
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"
#endif

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/visitor_functor.h"
#include "util/indent.h"
#include "util/IO_utils.tcc"

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
SPECIALIZE_UTIL_WHAT(HAC::entity::CHP::function_call_stmt, 
		"CHP-func-call")

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
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::CHP::function_call_stmt, CHP_FUNCTION_CALL_TYPE_KEY, 0)
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
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
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
	const unroll_context&, const bool, const bool);

static
ostream&
dump_selection_event(ostream& o, const selection_list_type& sl, 
		const expr_dump_context& c, 
		const char* b, const char* d, const char* f) {
	typedef	selection_list_type::const_iterator	const_iterator;
	NEVER_NULL(b);
	NEVER_NULL(d);
	NEVER_NULL(f);
	// want to print some shorthand for selection event...
	// [G1 -> ... [] G2 -> ... ]
	const_iterator i(sl.begin());
	const const_iterator e(sl.end());
	INVARIANT(i!=e);
	(*i)->dump_brief(o << b, c);
	for (++i; i!=e; ++i)
		(*i)->dump_brief(o << d, c);
	return o << f;
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
#if CHP_ACTION_PARENT_LINK
	return a->unroll_resolve_copy(_context);
#else
	return a->unroll_resolve_copy(_context, a);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default destructor
action::action() : 
		persistent(), 
#if CHP_ACTION_PARENT_LINK
		parent(NULL), 
#endif
		delay() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action::action(const attributes_type& a) : 
		persistent(), 
#if CHP_ACTION_PARENT_LINK
		parent(NULL), 
#endif
		delay(a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHP_ACTION_PARENT_LINK
// inline
action::action(const action& a) : 
		persistent(), parent(NULL), delay(a.delay) {
}
#endif

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
	// DO NOT write the parent-link pointer, 
	// that can and will  be trivially reconstructed upon load
	m.read_pointer(i, delay);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traces an action's position in its syntax tree by following
	parent links.  
	Accumulates parent path recursively.  
 */
void
make_action_parent_path(const action& a, action_parent_list_type& path) {
	STACKTRACE_VERBOSE;
	const action* p = &a;
	do {
		path.push_front(p);
		STACKTRACE_INDENT_PRINT("parent = " << p << endl);
		p = p->get_parent();
	} while (p);
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
#if CHP_ACTION_PARENT_LINK
/**
	\pre this sequence is the exclusive owner of action s, 
		to correctly establish its back-link.
 */
void
action_sequence::push_back(list_type::const_reference s) {
	list_type::push_back(s);
//	NEVER_NULL(s);		// caller accumulator checks for NULLs
	if (s) {
		s->set_parent(this);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
action_sequence::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
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
	transform(begin(), end(), back_inserter(*ret), action::transformer(c));
	const const_iterator
		f(find(ret->begin(), ret->end(), action_ptr_type(NULL)));
	if (f != ret->end()) {
		cerr << "Error unrolling action_sequence." << endl;
		return action_ptr_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (
		// eventually to attribute list pointer comparison
		(delay == ret->delay) && 
		equal(begin(), end(), ret->begin()))
		return p;
	else
#else
	// always return fresh copy
#endif
		return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
/**
	Give me boost::functional!!! PLEASE!
 */
template <class ListIterator, class ParentType>
static
void
relink_parent_actions(ListIterator b, ListIterator e, const ParentType* p) {
	typedef	typename ListIterator::value_type	ptr_type;
	typedef	typename util::memory::pointee<ptr_type>::type
							element_type;
	STACKTRACE_VERBOSE;
	std::for_each(b, e, 
		ADS::unary_compose(
		std::bind2nd(std::mem_fun_ref(&element_type::set_parent), p), 
		util::dereference<ptr_type>())
	);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer_list(i, static_cast<list_type&>(*this));
#if CHP_ACTION_PARENT_LINK
	relink_parent_actions(begin(), end(), this);
#endif
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
#if CHP_ACTION_PARENT_LINK
/**
	\pre this sequence is the exclusive owner of action s, 
		to correctly establish its back-link.
 */
void
concurrent_actions::push_back(list_type::const_reference s) {
	list_type::push_back(s);
//	NEVER_NULL(s);		// caller accumulator checks for NULLs
	if (s) {
		s->set_parent(this);
	}
}
#endif

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
concurrent_actions::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const count_ptr<this_type> ret(new this_type);
	NEVER_NULL(ret);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
	if (__unroll(c, *ret).good) {
#if !CHP_ACTION_PARENT_LINK
		if (equal(begin(), end(), ret->begin()))
			return p;
		else
#else
		// always return fresh copy, with parents linked
#endif
			return ret;
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
if (!empty()) {
	this_type& t(f.get_chp_footprint());	// allocates on-demand
	return __unroll(c, t);
} else {
	// don't bother if is empty
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
	relink_parent_actions(begin(), end(), this);
#endif
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
#if CHP_ACTION_PARENT_LINK
	return g->unroll_resolve_copy(_context);
#else
	return g->unroll_resolve_copy(_context, g);
#endif
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
#if CHP_ACTION_PARENT_LINK
/**
	\pre guarded action must ALREADY BE LOADED from persistent 
		reconstruction, so stmt pointer is valid.  
 */
void
guarded_action::set_parent(const action* p) const {
	if (stmt) {
		stmt->set_parent(p);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unroll-resolves the data-expression and action.  
 */
guarded_action::unroll_return_type
guarded_action::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const count_ptr<const this_type>& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	guard_ptr_type g(NULL);
	if (guard) {
		g = guard->unroll_resolve_copy(c, guard);
		if (!g) {
			cerr << "Error resolving guard expression." << endl;
			return unroll_return_type(NULL);
		}
	}
if (stmt) {
	const action_ptr_type
#if CHP_ACTION_PARENT_LINK
		a(stmt->unroll_resolve_copy(c));
#else
		a(stmt->unroll_resolve_copy(c, stmt));
#endif
	if (!a) {
		cerr << "Error resolving action statement of guarded action."
			<< endl;
		return unroll_return_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (g == guard && a == stmt) {
		// resolving resulted in no change
		return p;
	} else {
#else
		// always return deep copy, caller will link parents
#endif
		return unroll_return_type(new this_type(g, a));
#if !CHP_ACTION_PARENT_LINK
	}
#endif
} else {
	// have a 'skip' statement (NULL)
#if !CHP_ACTION_PARENT_LINK
	if (g == guard) {
		// resolving resulted in no change
		return p;
	} else {
#else
	// return fresh deep copy
#endif
		return unroll_return_type(
			new this_type(g, action_ptr_type(NULL)));
#if !CHP_ACTION_PARENT_LINK
	}
#endif
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
ostream&
deterministic_selection::dump_event(ostream& o, 
		const expr_dump_context& c) const {
	// want to print some shorthand for selection event...
	// [G1 -> ... [] G2 -> ... ]
	return dump_selection_event(o, *this, c, "[ ", " [] ", " ]");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHP_ACTION_PARENT_LINK
/**
	\pre this sequence is the exclusive owner of action s, 
		to correctly establish its back-link.
 */
void
deterministic_selection::push_back(list_type::const_reference s) {
	list_type::push_back(s);
//	NEVER_NULL(s);		// caller accumulator checks for NULLs
	if (s) {
		s->set_parent(this);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
deterministic_selection::unroll_resolve_copy(const unroll_context& c 
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const count_ptr<this_type> r(new this_type);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling deterministic selection." << endl;
		return action_ptr_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
#else
		// always return fresh deep copy
#endif
		// return newly constructed copy
		return r;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
deterministic_selection::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
	// need to guarantee that actions are loaded before relinking
	m.load_once_pointer_list(static_cast<list_type&>(*this));
	relink_parent_actions(begin(), end(), this);
#endif
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
	// want to print some shorthand for selection event...
	// [G1 -> ... : G2 -> ... ]
	return dump_selection_event(o, *this, c, "[ ", " : ", " ]");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHP_ACTION_PARENT_LINK
/**
	\pre this sequence is the exclusive owner of action s, 
		to correctly establish its back-link.
 */
void
nondeterministic_selection::push_back(list_type::const_reference s) {
	list_type::push_back(s);
//	NEVER_NULL(s);		// caller accumulator checks for NULLs
	if (s) {
		s->set_parent(this);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
nondeterministic_selection::unroll_resolve_copy(const unroll_context& c 
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const count_ptr<this_type> r(new this_type);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling nondeterministic selection." << endl;
		return action_ptr_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
#else
		// always return fresh deep copy
#endif
		// return newly constructed copy
		return r;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nondeterministic_selection::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
	// need to guarantee that actions are loaded before relinking
	m.load_once_pointer_list(static_cast<list_type&>(*this));
	relink_parent_actions(begin(), end(), this);
#endif
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
#if CHP_ACTION_PARENT_LINK
	body->set_parent(this);
#endif
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
metaloop_selection::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
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
	DECLARE_TEMPORARY_FOOTPRINT(f);
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& i(var->get_instance().value);
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (i = min; i <= max; ++i) {
		const selection_list_type::value_type	// guarded_action
#if CHP_ACTION_PARENT_LINK
			g(body->unroll_resolve_copy(cc));
#else
			g(body->unroll_resolve_copy(cc, body));
#endif
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
#if CHP_ACTION_PARENT_LINK
		// back_insert (push_back) automatically link parent-child
		copy(result.begin(), result.end(), back_inserter(*ret));
#else
		ret->swap(result);
#endif
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	} else {
		const count_ptr<nondeterministic_selection>
			ret(new nondeterministic_selection);
		NEVER_NULL(ret);
#if CHP_ACTION_PARENT_LINK
		// back_insert (push_back) automatically link parent-child
		copy(result.begin(), result.end(), back_inserter(*ret));
#else
		ret->swap(result);
#endif
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
#if CHP_ACTION_PARENT_LINK
	NEVER_NULL(body);
	body->set_parent(this);
#endif
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
#if CHP_ACTION_PARENT_LINK
	body->set_parent(this);
#endif
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
metaloop_statement::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
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
	DECLARE_TEMPORARY_FOOTPRINT(f);
	const never_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& i(var->get_instance().value);
		// acquire direct reference
	const unroll_context cc(&f, c);
	for (i = min; i <= max; ++i) {
		const action_list_type::value_type	// guarded_action
#if CHP_ACTION_PARENT_LINK
			g(body->unroll_resolve_copy(cc));
#else
			g(body->unroll_resolve_copy(cc, body));
#endif
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
#if CHP_ACTION_PARENT_LINK
		// back_insert (push_back) automatically link parent-child
		copy(result.begin(), result.end(), back_inserter(*ret));
#else
		ret->swap(result);
#endif
		UNROLL_ATTACH_RESULT_ATTRIBUTES(ret)
		return ret;
	} else {
		const count_ptr<action_sequence>
			ret(new action_sequence);
		NEVER_NULL(ret);
#if CHP_ACTION_PARENT_LINK
		// back_insert (push_back) automatically link parent-child
		copy(result.begin(), result.end(), back_inserter(*ret));
#else
		ret->swap(result);
#endif
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
#if CHP_ACTION_PARENT_LINK
	NEVER_NULL(body);
	body->set_parent(this);
#endif
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
assignment::unroll_resolve_copy(const unroll_context& c 
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
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
#if !CHP_ACTION_PARENT_LINK
	} else if (
		(delay == atts) &&
		(lc == lval) && (rc == rval)) {
		return p;
#endif
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
condition_wait::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const cond_ptr_type g(cond->unroll_resolve_copy(c, cond));
	UNROLL_COPY_ATTRIBUTES
	if (!g) {
		cerr << "Error resolving condition-wait." << endl;
		return action_ptr_type(NULL);
#if !CHP_ACTION_PARENT_LINK
	} else if (
		(delay == atts) &&
		(g == cond)) {
		return p;
#endif
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
	\param p true if receive is actually a peek.
 */
good_bool
set_channel_alias_directions(
		const simple_channel_nonmeta_instance_reference& ncr, 
		const unroll_context& c, const bool d, const bool p) {
	// flag that a channel(s) is connected to producers
	// recall that nonmeta references MUST be scalar..
	const never_ptr<const nonmeta_index_list> ind(ncr.get_indices());
	const count_ptr<dynamic_meta_index_list>
		mil(ind ? ind->make_meta_index_list() :
			count_ptr<dynamic_meta_index_list>(NULL));
	typedef	channel_connect_policy		connect_policy;
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
			d ? connect_policy::CONNECTED_TO_CHP_NONMETA_PRODUCER
			: p ? connect_policy::DEFAULT_CONNECT_FLAGS
			: connect_policy::CONNECTED_TO_CHP_NONMETA_CONSUMER
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
		// no such thing as meta language peek
		if (!ca->find()->set_connection_flags(
			d ? connect_policy::CONNECTED_TO_CHP_META_PRODUCER
			: p ? connect_policy::DEFAULT_CONNECT_FLAGS
			: connect_policy::CONNECTED_TO_CHP_META_CONSUMER
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
	chan->dump(o, c) << '!';
	const_iterator i(exprs.begin());
	const const_iterator e(exprs.end());
if (i!=e) {
	(*i)->dump(o << '(', c);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ',', c);
	}
	o << ')';
}
	return o;
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
channel_send::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
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
	if (!set_channel_alias_directions(*cc, c, true, false).good) {
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
#if !CHP_ACTION_PARENT_LINK
	if ((cc == chan) && 
		(atts == delay) &&
			equal(exprs.begin(), exprs.end(), exprs_c.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
#else
		// always return deep copy
#endif
		const count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->exprs.swap(exprs_c);	// faster than copying/assigning
		ret->set_delay(atts);
		return ret;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

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

channel_receive::channel_receive() : parent_type(), chan(), insts(), 
	peek(false) { }

channel_receive::channel_receive(const chan_ptr_type& c, const bool p) :
		parent_type(), chan(c), insts(), peek(p) {
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
	chan->dump(o, c);
	o << (peek ? '#' : '?');
if (!insts.empty()) {
	const_iterator i(insts.begin());
	const const_iterator e(insts.end());
	(*i)->dump(o << '(', c);
	for (++i; i!=e; ++i) {
		(*i)->dump(o << ',', c);
	}
	o << ')';
}
	return o;
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
channel_receive::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const chan_ptr_type cc(chan->unroll_resolve_copy(c, chan));
	inst_ref_list_type refs;
	transform(insts.begin(), insts.end(), back_inserter(refs), 
		data_ref_unroll_resolver(c));
	if (!cc) {
		cerr << "Error resolving channel reference of send." << endl;
		return action_ptr_type(NULL);
	}
#if 1
	// pass false to indicate receive (3rd arg)
	// NOTE: peek does not count as connecting to a consumer!
	if (!set_channel_alias_directions(*cc, c, false, peek).good) {
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
#if !CHP_ACTION_PARENT_LINK
	if ((cc == chan) &&
		(atts == delay) && 
			equal(insts.begin(), insts.end(), refs.begin())) {
		// resolved members match exactly, return copy
		return p;
	} else {
#else
		// always return deep copy
#endif
		count_ptr<this_type> ret(new this_type(cc, peek));
		NEVER_NULL(ret);
		ret->insts.swap(refs);	// faster than copying/assigning
		ret->set_delay(atts);
		return ret;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
	write_value(o, peek);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_receive::load_object(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer(i, chan);
	m.read_pointer_list(i, insts);
	read_value(i, peek);
}

//=============================================================================
// class do_forever_loop method definitions

do_forever_loop::do_forever_loop() : parent_type(), body() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
do_forever_loop::do_forever_loop(const body_ptr_type& b) :
		parent_type(), body(b) {
	NEVER_NULL(body);
#if CHP_ACTION_PARENT_LINK
	body->set_parent(this);
#endif
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
do_forever_loop::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const action_ptr_type
#if CHP_ACTION_PARENT_LINK
		b(body->unroll_resolve_copy(c));
#else
		b(body->unroll_resolve_copy(c, body));
#endif
	if (!b) {
		cerr << "Error resolving do-forever loop." << endl;
		return action_ptr_type(NULL);
	}
	UNROLL_COPY_ATTRIBUTES
#if !CHP_ACTION_PARENT_LINK
	if (
		(atts == delay) && 
		(b == body)) {
		// return self-copy
		return p;
	} else {
#endif
		// return newly constructed copy
		const count_ptr<this_type> ret(new this_type(b));
		ret->set_delay(atts);
		return ret;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_forever_loop::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
	NEVER_NULL(body);
	body->set_parent(this);
#endif
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
ostream&
do_while_loop::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump_selection_event(o, *this, c, "*[ ", " [] ", " ]");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CHP_ACTION_PARENT_LINK
/**
	\pre this sequence is the exclusive owner of action s, 
		to correctly establish its back-link.
 */
void
do_while_loop::push_back(list_type::const_reference s) {
	list_type::push_back(s);
//	NEVER_NULL(s);		// caller accumulator checks for NULLs
	if (s) {
		s->set_parent(this);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
do_while_loop::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
	STACKTRACE_VERBOSE;
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const count_ptr<this_type> r(new this_type);
	NEVER_NULL(r);
	UNROLL_ATTACH_RESULT_ATTRIBUTES(r)
	if (!unroll_resolve_selection_list(*this, c, *r).good) {
		cerr << "Error unrolling do-while loop." << endl;
		return action_ptr_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (
		(delay == r->delay) &&
		equal(begin(), end(), r->begin())) {
		// return self-copy
		return p;
	} else {
#endif
		// return newly constructed copy
		return r;
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
do_while_loop::accept(chp_visitor& v) const {
	v.visit(*this);
}

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
#if CHP_ACTION_PARENT_LINK
	// need to guarantee that actions are loaded before relinking
	m.load_once_pointer_list(static_cast<list_type&>(*this));
	relink_parent_actions(begin(), end(), this);
#endif
}

//=============================================================================
// class function_call_stmt method definitions

function_call_stmt::function_call_stmt() : parent_type(), call_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
function_call_stmt::function_call_stmt(const call_expr_ptr_type& e) :
		parent_type(), call_expr(e) {
	NEVER_NULL(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
function_call_stmt::~function_call_stmt() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(function_call_stmt)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
function_call_stmt::dump(ostream& o, const expr_dump_context& c) const {
	return call_expr->dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
function_call_stmt::dump_event(ostream& o, const expr_dump_context& c) const {
	return dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
action_ptr_type
function_call_stmt::unroll_resolve_copy(const unroll_context& c
#if !CHP_ACTION_PARENT_LINK
		, const action_ptr_type& p
#endif
		) const {
#if !CHP_ACTION_PARENT_LINK
	INVARIANT(p == this);
#endif
	const call_expr_ptr_type
		e(call_expr->__unroll_resolve_copy(c, call_expr));
	if (!e) {
		// got error message already?
		return action_ptr_type(NULL);
	}
#if !CHP_ACTION_PARENT_LINK
	if (call_expr == e) {
		return p;
	} else {
#endif
		return action_ptr_type(new this_type(e));
#if !CHP_ACTION_PARENT_LINK
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
function_call_stmt::accept(chp_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
function_call_stmt::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	call_expr->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
function_call_stmt::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, call_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
function_call_stmt::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, call_expr);
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

