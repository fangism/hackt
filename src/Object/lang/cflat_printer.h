/**
	\file "Object/lang/cflat_printer.h"
	Cflat printer functor.  
	$Id: cflat_printer.h,v 1.14 2009/10/29 23:00:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__
#define	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__

#include "Object/lang/cflat_context_visitor.h"
#include "util/member_saver_fwd.h"
#include "Object/lang/SPEC_fwd.h"

namespace HAC {
class cflat_options;

namespace entity {
class footprint;

namespace PRS {
//=============================================================================
/**
	PRS print visitor functor.  
 */
class cflat_prs_printer : public cflat_context_visitor {
	typedef	cflat_context_visitor		parent_type;
public:
	ostream&				os;
	const cflat_options&			cfopts;
protected:
	/**
		Parental context information for optimal parenthesization. 
	 */
	char					parent_expr_type;

	typedef	util::member_saver<cflat_prs_printer, char,
			&cflat_prs_printer::parent_expr_type>
						expr_type_setter;
	/**
		Compute and propagate up the best and worst case
		path conductances through rules.
		Skip weak rules.  
		These values are just return-value holders for 
		expression traversal, ctor may leave them uninitialized.
	 */
	float					max_conductance;
	float					min_conductance;
	/// strongest single path
	float					one_conductance;

public:
	cflat_prs_printer(ostream& _os, const cflat_options& _cfo) :
			cflat_context_visitor(), 
			os(_os), cfopts(_cfo) { }

	ostream&
	print_node_name(ostream&, const global_entry<bool_tag>&) const;

	void
	__dump_canonical_literal(const size_t) const;

	void
	__dump_resolved_canonical_literal(const size_t) const;

	void
	__dump_canonical_literal_group(const directive_node_group_type&) const;

	void
	__dump_canonical_literal_group(const directive_node_group_type&, 
		const char* l, const char* d, const char* r) const;

	void
	__dump_resolved_literal_group(const directive_node_group_type&) const;

	void
	__dump_resolved_literal_group(const directive_node_group_type&, 
		const char* l, const char* d, const char* r) const;

protected:
	using cflat_visitor::visit;

	// override
	void
	visit(const global_entry<bool_tag>&);

	void
	visit(const footprint_rule&);

	void
	visit(const footprint_expr_node&);

	void
	visit(const footprint_macro&);

	void
	visit(const PRS::footprint&);

	void
	visit(const SPEC::footprint_directive&);

};	// end struct cflat_prs_printer

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_PRINTER_H__

