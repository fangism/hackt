/**
	\file "Object/art_object_CHP.cc"
	Class implementations of CHP objects.  
	$Id: art_object_CHP.cc,v 1.3.2.2.2.1 2005/07/11 03:26:43 fang Exp $
 */

#include "Object/art_object_CHP.h"
#include "Object/expr/bool_expr.h"
#include "Object/expr/int_expr.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_nonmeta_inst_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_inst_ref_subtypes.h"
#include "Object/traits/chan_traits.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_collection.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/indent.h"

namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::action_sequence,
		"CHP-action-sequence")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::concurrent_actions,
		"CHP-concurrent-actions")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::guarded_action,
		"CHP-guarded-action")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::deterministic_selection,
		"CHP-deterministic-selection")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::nondeterministic_selection,
		"CHP-nondeterministic-selection")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::assignment,
		"CHP-assignment")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::condition_wait,
		"CHP-condition-wait")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::channel_send,
		"CHP-channel-send")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::channel_receive,
		"CHP-channel-receive")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::do_forever_loop, 
		"CHP-forever-loop")
SPECIALIZE_UTIL_WHAT(ART::entity::CHP::do_while_loop, 
		"CHP-do-while")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::action_sequence, CHP_SEQUENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::concurrent_actions, CHP_CONCURRENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::guarded_action, CHP_GUARDED_ACTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::deterministic_selection, CHP_DET_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::nondeterministic_selection, CHP_NONDET_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::assignment, CHP_ASSIGNMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::condition_wait, CHP_WAIT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::channel_send, CHP_SEND_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::channel_receive, CHP_RECEIVE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::do_forever_loop, CHP_FOREVER_LOOP_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::CHP::do_while_loop, CHP_DO_WHILE_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
namespace CHP {
using util::auto_indent;
using util::persistent_traits;
#include "util/using_ostream.h"
//=============================================================================
// class action_sequence method definitions

action_sequence::action_sequence() : parent_type(), list_type() { }

action_sequence::~action_sequence() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(action_sequence)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
action_sequence::dump(ostream& o) const {
	o << "sequential: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << '}';
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
ostream&
concurrent_actions::dump(ostream& o) const {
	o << "concurrent: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << '}';
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
guarded_action::dump(ostream& o) const {
	if (guard)
		guard->dump(o);
	else 	o << "else";
	o << " -> ";
	if (stmt)
		return stmt->dump(o);
	else 	return o << "skip";
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
// class deterministic_selection method definitions

deterministic_selection::deterministic_selection() :
		parent_type(), list_type() { }

deterministic_selection::~deterministic_selection() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(deterministic_selection)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
deterministic_selection::dump(ostream& o) const {
	o << "deterministic: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << '}';
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
nondeterministic_selection::dump(ostream& o) const {
	o << "nondeterministic: {" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << '}';
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
assignment::dump(ostream& o) const {
	return rval->dump(lval->dump(o) << " := ");
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
condition_wait::dump(ostream& o) const {
	return cond->dump(o << '[') << ']';
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
channel_send::dump(ostream& o) const {
	typedef	expr_list_type::const_iterator	const_iterator;
	chan->dump_briefer(o, never_ptr<const scopespace>()) << "!(";
	INVARIANT(!exprs.empty());
	const_iterator i(exprs.begin());
	const const_iterator e(exprs.end());
	(*i)->dump(o);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ',');
	}
	return o << ')';
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
channel_receive::dump(ostream& o) const {
	typedef	inst_ref_list_type::const_iterator	const_iterator;
	chan->dump_briefer(o, never_ptr<const scopespace>()) << "?(";
	INVARIANT(!insts.empty());
	const_iterator i(insts.begin());
	const const_iterator e(insts.end());
	(*i)->dump(o);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ',');
	}
	return o << ')';
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
do_forever_loop::dump(ostream& o) const {
	o << "*[" << endl;
	{
		INDENT_SECTION(o);
		body->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << ']';
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
do_while_loop::dump(ostream& o) const {
	o << "*[" << endl;
	{
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++)
			(*i)->dump(o << auto_indent) << endl;
	}
	return o << auto_indent << ']';
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
}	// end namespace ART

