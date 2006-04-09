/**
	\file "Object/inst/alias_printer.h"
	$Id: alias_printer.h,v 1.1.2.3 2006/04/09 04:34:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_PRINTER_H__
#define	__HAC_OBJECT_INST_ALIAS_PRINTER_H__

#include "Object/inst/alias_visitor.h"
#include "Object/common/cflat_args.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Alias-printing visitor.  
 */
struct alias_printer : public alias_visitor, public cflat_aliases_arg_type {

	alias_printer(ostream& _o, const state_manager& _sm,
			const footprint& _f,
			const footprint_frame* const _fpf,
			const cflat_options& _cf,
			wire_alias_set& _w,
			const string& _p = string()) :
			cflat_aliases_arg_type(_o, _sm, _f, _fpf, _cf, _w, _p) {
	}

	// default dtor
	// default copy-ctor

	VISIT_INSTANCE_ALIAS_INFO_PROTOS

private:
	// helper functions here
	template <class Tag>
	void
	__visit(const instance_alias_info<Tag>&);

	// non-copyable
	explicit
	alias_printer(const alias_printer&);

};	// end class alias_printer

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_PRINTER_H__

