/**
	\file "Object/global_entry_dumper.hh"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_dumper.hh,v 1.1 2010/04/07 00:12:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_DUMPER_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_DUMPER_H__


#include <iosfwd>
#include "Object/global_entry_context.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Doesn't need the footprint_frame pointer because we're not
	traversing the hierarchy.  
 */
struct global_entry_dumper :
		public global_entry_context	// needs footprint_frame
{
public:
	typedef	global_entry_context		parent_type;
	using global_entry_context_base::topfp;

	ostream&				os;
	// parent process index (1-based, 0 means top-level)
	size_t					pid;

	static const char			table_header[];

	global_entry_dumper(ostream& _o, const global_process_context& c) :
		parent_type(c), os(_o), pid(0)
		{ }

virtual	~global_entry_dumper();

virtual	void
	visit(const footprint&);
virtual	void
	visit(const state_instance<process_tag>&);
virtual	void
	visit(const state_instance<channel_tag>&);
virtual	void
	visit(const state_instance<enum_tag>&);
virtual	void
	visit(const state_instance<int_tag>&);
virtual	void
	visit(const state_instance<bool_tag>&);

protected:
	template <class Tag>
	void
	__default_visit(const state_instance<Tag>&);

};	// end struct global_entry_dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
struct global_allocation_dumper : public global_entry_dumper {

	global_allocation_dumper(ostream& o,
		const global_process_context& c) :
		global_entry_dumper(o, c) { }

	void
	visit(const footprint&);

	using global_entry_dumper::visit;

};	// end struct global_allocation_dumper

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_DUMPER_H__

