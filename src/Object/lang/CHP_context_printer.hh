/**
	\file "Object/lang/CHP_context_printer.hh"
	The visitor that initializes and allocates CHPSIM state.  
	$Id: CHP_context_printer.hh,v 1.2 2007/09/28 05:36:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_CONTEXT_PRINTER_H__
#define	__HAC_OBJECT_LANG_CHP_CONTEXT_PRINTER_H__

#include <iosfwd>
#include "Object/lang/CHP_visitor.hh"
#include "Object/lang/CHP.hh"

namespace HAC {
namespace entity {
struct expr_dump_context;
namespace CHP {
using std::ostream;

//=============================================================================
/**
	Visitor that prints the full-context.
	Do we REALLY need this to have a state_manager and footprint
	if this only traverses CHP?  (wasteful)
 */
class chp_context_printer : public chp_visitor {
	typedef	action_parent_list_type::const_iterator	path_iterator;
protected:
	ostream&				os;
//	const action*				stmt;
	/**
		Optional: if set, can print process instance name prefix, 
		instead of just local source names.
	 */
	const expr_dump_context&		dump_context;
	action_parent_list_type			path;
	path_iterator				here;
//	path_iterator				end;
	bool					print_all;
public:
	chp_context_printer(const action&, ostream&, const expr_dump_context&);

#if 0
	chp_context_printer(const state_manager& _sm, 
		const entity::footprint& _topfp) :
		chp_visitor(_sm, _topfp) { }
#endif

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
	operator () (void);

private:
	void
	print_if_match(action_list_type::const_reference, const action*);

	void
	print_if_match(selection_list_type::const_reference, const action*);

#if 0
	template <class ListType>
	void
	print_list(const ListType&);
#endif

	template <class ListType>
	void
	print_list_on_match(const ListType&, 
		const char*, const char*, const char*);

protected:
	using chp_visitor::visit;

#if 0
	// overrides
	void
	visit(const state_manager&);

	// overrides
	void
	visit(const entity::PRS::footprint&);	// no-op

	void
	visit(const footprint_rule&);	// no-op

	void
	visit(const footprint_expr_node&);	// no-op

	void
	visit(const footprint_macro&);	// no-op

	void
	visit(const entity::SPEC::footprint_directive&);	// no-op
#endif
};	// end class StateConstructor

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_CONTEXT_PRINTER_H__

