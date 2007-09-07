/**
	\file "sim/chpsim/EventExecutor.h"
	Visitor classes for CHP events.  
	$Id: EventExecutor.h,v 1.5.8.3 2007/09/07 01:33:17 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_EVENTEXECUTOR_H__
#define	__HAC_SIM_CHPSIM_EVENTEXECUTOR_H__

#include <iosfwd>
#include "Object/lang/CHP_visitor.h"
#include "sim/chpsim/devel_switches.h"

namespace HAC {
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
namespace entity {
	struct expr_dump_context;
}	/// end namespace entity
#endif
namespace SIM {
namespace CHPSIM {
using std::ostream;
using entity::CHP::chp_visitor;
using entity::CHP::action_sequence;
using entity::CHP::concurrent_actions;
using entity::CHP::guarded_action;
using entity::CHP::deterministic_selection;
using entity::CHP::nondeterministic_selection;
using entity::CHP::metaloop_selection;
using entity::CHP::metaloop_statement;
using entity::CHP::assignment;
using entity::CHP::condition_wait;
using entity::CHP::channel_send;
using entity::CHP::channel_receive;
using entity::CHP::do_while_loop;
using entity::CHP::do_forever_loop;
using entity::CHP::function_call_stmt;
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
using entity::expr_dump_context;
#endif
class nonmeta_context;
class EventExecutor;
class EventRechecker;
class EventNode;

//=============================================================================
/**
	Event executing visitor.  
 */
class EventExecutor : public chp_visitor {
	typedef	EventExecutor			this_type;
protected:
	nonmeta_context&			context;
public:
	explicit
	EventExecutor(nonmeta_context&);

	void
	visit(const action_sequence&);

	void
	visit(const concurrent_actions&);

	void
	visit(const guarded_action&);

	void
	visit(const deterministic_selection&);

	void
	visit(const nondeterministic_selection&);

	void
	visit(const metaloop_selection&);

	void
	visit(const metaloop_statement&);

	void
	visit(const assignment&);

	void
	visit(const condition_wait&);

	void
	visit(const channel_send&);

	void
	visit(const channel_receive&);

	void
	visit(const do_while_loop&);

	void
	visit(const do_forever_loop&);

	void
	visit(const function_call_stmt&);

private:
	using chp_visitor::visit;

	EventExecutor(const this_type&);

	this_type&
	operator = (const this_type&);
};	// end class EventExecutor

//-----------------------------------------------------------------------------
/**
	Event rechecking visitor.
 */
class EventRechecker : public chp_visitor {
	typedef	EventRechecker			this_type;
protected:
	const nonmeta_context&			context;
public:
	/// uninitialized return value
	char					ret;
public:
	explicit
	EventRechecker(const nonmeta_context& c);

	void
	visit(const action_sequence&);

	void
	visit(const concurrent_actions&);

	void
	visit(const guarded_action&);

	void
	visit(const deterministic_selection&);

	void
	visit(const nondeterministic_selection&);

	void
	visit(const metaloop_selection&);

	void
	visit(const metaloop_statement&);

	void
	visit(const assignment&);

	void
	visit(const condition_wait&);

	void
	visit(const channel_send&);

	void
	visit(const channel_receive&);

	void
	visit(const do_while_loop&);

	void
	visit(const do_forever_loop&);

	void
	visit(const function_call_stmt&);

private:
	using chp_visitor::visit;

	EventRechecker(const this_type&);

	this_type&
	operator = (const this_type&);

};	// end class EventRechecker

//-----------------------------------------------------------------------------
#if !CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS
/**
	Successor printing visitor.
 */
class EventSuccessorDumper : public chp_visitor {
	typedef	EventSuccessorDumper		this_type;
protected:
	ostream&				os;
	const EventNode&			event;
	const size_t				index;
	const expr_dump_context&		dump_context;
public:
	/// uninitialized return value
	char					ret;
public:
	EventSuccessorDumper(ostream& o, const EventNode& e, const size_t i, 
		const expr_dump_context& edc) : 
		os(o), event(e), index(i), dump_context(edc) { }

	void
	visit(const action_sequence&);

	void
	visit(const concurrent_actions&);

	void
	visit(const guarded_action&);

	void
	visit(const deterministic_selection&);

	void
	visit(const nondeterministic_selection&);

	void
	visit(const metaloop_selection&);

	void
	visit(const metaloop_statement&);

	void
	visit(const assignment&);

	void
	visit(const condition_wait&);

	void
	visit(const channel_send&);

	void
	visit(const channel_receive&);

	void
	visit(const do_while_loop&);

	void
	visit(const do_forever_loop&);

	void
	visit(const function_call_stmt&);

private:
	using chp_visitor::visit;

	EventSuccessorDumper(const this_type&);

	this_type&
	operator = (const this_type&);

};	// end class EventSuccessorDumper
#endif	// CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_EVENTEXECUTOR_H__

