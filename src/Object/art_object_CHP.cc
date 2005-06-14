/**
	\file "Object/art_object_CHP.cc"
	Class implementations of CHP objects.  
	$Id: art_object_CHP.cc,v 1.1.2.4.2.1 2005/06/14 05:28:06 fang Exp $
 */

#include "Object/art_object_CHP.h"
#include "Object/art_object_expr_base.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_nonmeta_inst_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_inst_ref_subtypes.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_collection.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"

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
}	// end namespace util

namespace ART {
namespace entity {
namespace CHP {
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
	return what(o);		// temporary
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
action_sequence::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
// class concurrent_actions method definitions

concurrent_actions::concurrent_actions() : parent_type(), list_type() { }

concurrent_actions::~concurrent_actions() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(concurrent_actions)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
concurrent_actions::dump(ostream& o) const {
	return what(o);		// temporary
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(static_cast<const list_type&>(*this));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, static_cast<const list_type&>(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
concurrent_actions::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, static_cast<list_type&>(*this));
}

//=============================================================================
// class guarded_action method definitions

guarded_action::guarded_action() : persistent(), guard(), stmt() { }

guarded_action::guarded_action(
		const guard_ptr_type& g, const stmt_ptr_type& p) :
		persistent(), guard(g), stmt(p) {
}

guarded_action::~guarded_action() { }

PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(guarded_action)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
guarded_action::dump(ostream& o) const {
	return what(o);		// temporary
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
	return what(o);		// temporary
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
	return what(o);		// temporary
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
	return what(o);		// temporary
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
	return what(o);		// temporary
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
	return what(o);		// temporary
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Type-checked wrapping around interface for adding expressions.  
	TODO: type-check (2005-06-11)
	TODO: this is inside a for-loop, apply transformation to 
		save some work, minimize repeated function calls. 
	\param e the nonmeta expression to append to send list (never NULL).  
	\pre already checked for match of number of expressions expected.
	\return good if successful, bad if type check failed.  
 */
good_bool
channel_send::push_back(const expr_list_type::value_type& e) {
	NEVER_NULL(e);
	// check type
	const size_t index = exprs.size();
	// get chan's type, in canonical form
	const never_ptr<const channel_instance_collection>
		inst_base(chan->get_inst_base_subtype());
	const count_ptr<const channel_type_reference_base>
		type_ref(inst_base->get_type_ref()
			.is_a<const channel_type_reference_base>());
	// critical that this next pointer only exists locally
	// see channel_type_reference::resolve_builtin_channel_type
	const never_ptr<const builtin_channel_type_reference>
		bctr(type_ref->resolve_builtin_channel_type());
	// the remainder belongs in a for-loop
	if (bctr) {
		const size_t max = bctr->num_datatypes();
		if (index >= max) {
			cerr << "You doofus, you tried to add too many "
				"expressions to channel-send, which requires "
				<< max << " arguments." << endl;
			// somewhere need to catch insufficient...
			return good_bool(false);
		}
		// was able to resolve built-in channel type
		const builtin_channel_type_reference::datatype_ptr_type
			dt(bctr->index_datatype(index));
		// TODO: TYPE CHECK e against dt
		exprs.push_back(e);
		return good_bool(true);
	} else {
		// not able to resolve, probably because of template
		// parameter dependence.  Will resolve in meta-expansion
		// phase.  Assume it's good for now, do not reject.
		exprs.push_back(e);
		return good_bool(true);
	}
}
#endif

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
	return what(o);		// temporary
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
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

