/**
	\file "Object/inst/alias_printer.h"
	$Id: alias_printer.h,v 1.3.16.2 2010/02/20 04:38:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_PRINTER_H__
#define	__HAC_OBJECT_INST_ALIAS_PRINTER_H__

#include "Object/inst/alias_visitor.h"
#include "Object/common/cflat_args.h"

/**
	Define to 1 to visit by aliases.
	Define to 0 to visit by unique instances.  
	Visiting by aliases is easier to guarantee coverage.
	Visiting by unique instances is easier to maintain footprint frames.
	Goal: ?
 */
#define	TRAVERSE_BY_ALIAS		1

namespace HAC {
namespace entity {
//=============================================================================
/**
	Alias-printing visitor.  
 */
struct alias_printer : public alias_visitor, public cflat_aliases_arg_type {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION && TRAVERSE_BY_ALIAS
	/**
		At any given level of hierarchy, this points to the
		footprint responsible for allocating local instances.
		This is needed to properly handle process ports.  
	 */
	const footprint*			owner_fp;
#endif

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

	// may not need these... depending on traversal algorithm
	VISIT_INSTANCE_ALIAS_INFO_PROTOS()

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	visit(const footprint&);

#if !TRAVERSE_BY_ALIAS
	void
	visit(const state_instance<bool_tag>&);

	void
	visit(const state_instance<process_tag>&);
#endif

	using alias_visitor::visit;
	using cflat_aliases_arg_type::visit;
#endif

private:
	// helper functions here
	template <class Tag>
	void
	__visit(const instance_alias_info<Tag>&);

	// non-copyable
	explicit
	alias_printer(const alias_printer&);

#if !TRAVERSE_BY_ALIAS
	void
	visit_recursive(const footprint&);
#endif

};	// end class alias_printer

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_PRINTER_H__

