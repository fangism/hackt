/**
	\file "Object/lang/cflat_printer.h"
	Cflat printer functor.  
	$Id: cflat_printer.h,v 1.4.20.3 2006/04/12 06:35:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__
#define	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__

#include "Object/lang/cflat_visitor.h"
#include "Object/cflat_context.h"
#include "Object/lang/SPEC_fwd.h"

namespace HAC {
class cflat_options;

namespace entity {
class footprint;

namespace PRS {
//=============================================================================
/**
	Full print context struct. 
 */
class cflat_prs_print_context : public cflat_context {
public:
	ostream&				os;
	const cflat_options&			cfopts;
public:
	cflat_prs_print_context(ostream& _os, const cflat_options& _cfo) :
			cflat_context(), os(_os), cfopts(_cfo) { }

};	// end struct cflat_prs_print_context

//=============================================================================
/**
	PRS print visitor functor.  
 */
class cflat_prs_printer : 
		public cflat_visitor, 
		public cflat_prs_print_context {
protected:
	/**
		Parental context information for optimal parenthesization. 
	 */
	char					parent_expr_type;

	struct expr_type_setter;

public:
	cflat_prs_printer(ostream& _os, const cflat_options& _cfo) :
			cflat_prs_print_context(_os, _cfo) { }

	using cflat_visitor::visit;

	size_t
	__lookup_global_bool_id(const size_t) const;

	void
	__dump_canonical_literal(const size_t) const;

	void
	__dump_resolved_canonical_literal(const size_t) const;

	void
	__resolve_unique_literal_group(const directive_node_group_type&, 
		directive_node_group_type&) const;

	void
	__dump_canonical_literal_group(const directive_node_group_type&) const;

	void
	__dump_canonical_literal_group(const directive_node_group_type&, 
		const char* l, const char* d, const char* r) const;

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

	void
	visit(const SPEC::footprint_directive&);

};	// end struct cflat_prs_printer

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_PRINTER_H__

