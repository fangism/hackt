/**
	\file "Object/lang/CHP.cc"
	Class implementations of CHP objects.  
	$Id: CHP.cc,v 1.16 2006/12/01 23:28:53 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iterator>
#include <algorithm>
#include <exception>
// #include <functional>
#include "Object/lang/CHP.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/nonmeta_index_list.h"
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
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"

#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/def/template_formals_manager.h"

#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/indent.h"
#include "util/IO_utils.tcc"

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
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

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
#if 0
	const unroll_return_type r(a->unroll(_context));
	if (r.changed && !r.copy) {
		// expect to already have error message
		throw std::exception();
	}
	return (r.changed ? r.copy : a);
#else
	return a->unroll_resolve_copy(_context, a);
#endif
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
			g(body->unroll_resolve_copy(c, body));
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
}

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
		count_ptr<this_type> ret(new this_type(cc));
		NEVER_NULL(ret);
		ret->exprs.swap(exprs_c);	// faster than copying/assigning
		return ret;
	}
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

