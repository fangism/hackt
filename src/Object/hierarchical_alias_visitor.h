/**
	\file "Object/hierarchical_alias_visitor.h"
	$Id: $
 */

#ifndef	__HAC_OBJECT_HIERARCHICAL_ALIAS_VISITOR_H__
#define	__HAC_OBJECT_HIERARCHICAL_ALIAS_VISITOR_H__

#include <string>
#include <vector>
#include "Object/global_entry_context.h"

namespace HAC {
namespace entity {
using std::string;
using std::vector;

/**
	Instead of visiting each unique instance once, 
	this visits every *alias* once, and stores the
	list of local aliases on a vector-stack.  
	Traversal is hierarchical.
	TODO: cache footprint_frames?
 */
class hierarchical_alias_visitor : public global_entry_context {
	typedef	global_entry_context		parent_type;
protected:
	typedef	vector<string>			scope_stack_type;
	scope_stack_type			scope_stack;
	size_t					id;
public:
	explicit
	hierarchical_alias_visitor(const global_process_context&);

virtual	~hierarchical_alias_visitor();

	template <class Tag>
	void
	visit_ports_and_local(const footprint&);

	void
	visit_recursive(const footprint&);

#if 0
virtual void
	visit(const state_instance<process_tag>&);
virtual void
	visit(const state_instance<channel_tag>&);
virtual void
	visit(const state_instance<enum_tag>&);
virtual void
	visit(const state_instance<int_tag>&);
virtual void
	visit(const state_instance<bool_tag>&);
#endif
virtual void
	visit(const footprint&);

protected:
	using parent_type::visit;

};	// end class hierarchical_alias_visitor

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_HIERARCHICAL_ALIAS_VISITOR_H__
