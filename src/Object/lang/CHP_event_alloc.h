/**
	\file "Object/lang/CHP_event_alloc.h"
	The visitor that initializes and allocates local CHP events.
	Class based on "sim/chpsim/StateConstructor.h".
	$Id: CHP_event_alloc.h,v 1.3 2010/04/02 22:18:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_EVENT_ALLOC_H__
#define	__HAC_OBJECT_LANG_CHP_EVENT_ALLOC_H__

#include <vector>
#include <set>		// or use util/memory/free_list interface
#include "Object/lang/CHP_visitor.h"
#include "Object/lang/CHP_footprint.h"
// #include "sim/common.h"

namespace HAC {
namespace entity {
struct expr_dump_context;
namespace CHP {

//=============================================================================
/**
	Visitor that initializes and allocates CHPSIM state.  
	TODO: re-factor code to not refer to non-CHP visitees.  
	This clearly lacks good organization.  :S
	Convention: event 0 is always the spawning/initialization event
	(local) index for each process.  
	Often, it will be a concurrent fork event to subsequent bodies.  
 */
class local_event_allocator : public chp_visitor {
	typedef	local_event_allocator		this_type;
public:
	// typedef	std::default_vector<size_t>::type	return_indices_type;
	typedef	local_event			event_type;
	typedef	std::vector<event_type>		event_pool_type;
	typedef	size_t				event_index_type;
private:
	typedef	std::set<event_index_type>	free_list_type;
private:
	local_event_footprint&			event_footprint;
	free_list_type				free_list;
public:
	/**
		Return value slot to indicate last allocated event(s).  
		Should be non-zero.  
		There may be more than one in the case of concurrency.  
		These are needed to re-link predecessors to successors.  
	 */
	// return_indices_type			last_event_indices;
	// should be from sim/common.h, must be *unsigned*
	event_index_type			last_event_index;
private:
	// non-copy-able
	explicit
	local_event_allocator(const this_type&);

	this_type&
	operator = (const this_type&);

public:
	explicit
	local_event_allocator(local_event_footprint&);

	~local_event_allocator();

	bool
	valid_last_event_index(void) const;

protected:
	event_type&
	get_event(const event_index_type ei);

	const event_type&
	get_event(const event_index_type ei) const;

	event_index_type
	allocate_event(const event_type&);

	// only needed when trying to be clever and recycle useless events
	void
	deallocate_event(const event_index_type);

	size_t
	event_pool_size(void) const;

	void
	connect_successor_events(event_type&) const;

public:
	void
	count_predecessors(const event_type&) const;

protected:
	event_index_type
	forward_successor(const event_index_type);

	void
	// used to be named forward_successor
	substitute_successor(const event_index_type, const event_index_type, 
		const event_index_type);

public:
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

	void
	compact_and_canonicalize(void);

protected:
	using chp_visitor::visit;

	void
	reset(void);
#if 0

	// overrides
	void
	visit(const state_manager&);
#endif

};	// end class local_event_allocator


//-----------------------------------------------------------------------------
/**
	Successor printing visitor.
	Stolen from "sim/chpsim/EventExecutor.h":EventSuccessorDumper.
 */
class EventSuccessorDumper : public chp_visitor {
	typedef	EventSuccessorDumper		this_type;
	typedef	local_event			event_type;
public:
	typedef	size_t				event_index_type;
protected:
	ostream&				os;
	const event_type&			event;
	// eventually add an offset to translate to global allocated index!
	const event_index_type			index;
	const event_index_type			offset;
	const expr_dump_context&		dump_context;
public:
	/// uninitialized return value
	char					ret;
public:
	EventSuccessorDumper(
		ostream& o, const local_event& e,
		const event_index_type i, 
		const expr_dump_context& edc, 
		const event_index_type off = 0) : 
		os(o), event(e), index(i), offset(off), dump_context(edc) { }

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

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_EVENT_ALLOC_H__

