/**
	\file "Object/inst/alias_printer.h"
	$Id: alias_printer.h,v 1.5 2010/04/02 22:18:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_PRINTER_H__
#define	__HAC_OBJECT_INST_ALIAS_PRINTER_H__

#include <set>
#include <vector>
#include "Object/common/cflat_args.h"
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/inst/alias_visitor.h"
#endif

namespace HAC {
namespace entity {
struct alias_printer;
using std::set;
using std::vector;
struct wire_alias_set;

//=============================================================================
/**
	Alias-printing visitor.  
 */
struct alias_printer : 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		public alias_visitor, 
#endif
		public cflat_aliases_arg_type {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// local map of bool aliases
	alias_set_type			local_bool_aliases;
	ostream&			o;
	/**
		cflat mode and style flags.  
	 */
	const cflat_options&		cf;
	/**
		\pre is already sized properly to accomodate total
			number of allocated bool nodes.  
	 */
	wire_alias_set&			wires;
#endif
public:
	alias_printer(ostream& _o, 
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			const footprint_frame& _fpf, 
			const global_offset& g,
#else
			const state_manager& _sm,
			const footprint& _f,
			const footprint_frame* const _fpf,
#endif
			const cflat_options& _cf,
			wire_alias_set& _w,
			const string& _p = string());

	// default dtor
	// default copy-ctor

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	// may not need these... depending on traversal algorithm
#define	NOT_VIRTUAL
	VISIT_INSTANCE_ALIAS_INFO_PROTOS(NOT_VIRTUAL)
#undef	NOT_VIRTUAL
	using alias_visitor::visit;
#endif
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	visit(const footprint&);
	void
	visit_footprint(const footprint&);

	void
	visit_recursive(const footprint&);

	using cflat_aliases_arg_type::visit;

	void
	visit(const state_instance<bool_tag>&);

#endif

private:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	// helper functions here
	template <class Tag>
	void
	__visit(const instance_alias_info<Tag>&);
#endif

	// non-copyable
	explicit
	alias_printer(const alias_printer&);

};	// end class alias_printer

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_PRINTER_H__

