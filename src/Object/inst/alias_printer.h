/**
	\file "Object/inst/alias_printer.h"
	$Id: alias_printer.h,v 1.7 2010/05/11 00:18:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_PRINTER_H__
#define	__HAC_OBJECT_INST_ALIAS_PRINTER_H__

#include <set>
#include <vector>
#include "Object/common/cflat_args.h"

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
		public cflat_aliases_arg_type {
	string				alt_prefix;
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
public:
	alias_printer(ostream& _o, 
			const footprint_frame& _fpf, 
			const global_offset& g,
			const cflat_options& _cf,
			wire_alias_set& _w,
			const string& _p = string());

	// default dtor
	// default copy-ctor

	void
	visit(const footprint&);
	void
	visit_footprint(const footprint&);

	void
	visit_recursive(const footprint&);

	using cflat_aliases_arg_type::visit;

	void
	visit(const state_instance<bool_tag>&);

private:
	// non-copyable
	explicit
	alias_printer(const alias_printer&);

};	// end class alias_printer

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_PRINTER_H__

