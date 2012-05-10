/**
	\file "Object/lang/cflat_printer.h"
	Cflat printer functor.  
	$Id: cflat_printer.h,v 1.19 2011/05/02 21:27:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__
#define	__HAC_OBJECT_LANG_CFLAT_PRINTER_H__

#include "Object/lang/cflat_context_visitor.h"
#include "Object/lang/cflat_visitor.h"
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
class cflat_prs_printer : public cflat_context_visitor, public cflat_visitor {
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
	cflat_prs_printer(const global_process_context& c,
			ostream& _os, const cflat_options& _cfo) :
			cflat_context_visitor(c), 
			os(_os), cfopts(_cfo) { }
	~cflat_prs_printer();

	template <class Tag>
	void
	__dump_canonical_literal(const size_t) const;

	template <class Tag>
	void
	__dump_resolved_canonical_literal(const size_t) const;

	template <class Tag>
	void
	__dump_canonical_literal(ostream&, const size_t) const;

	template <class Tag>
	void
	__dump_resolved_canonical_literal(ostream&, const size_t) const;

	template <class Tag>
	void
	__dump_canonical_literal_group(const directive_node_group_type&) const;

	template <class Tag>
	void
	__dump_canonical_literal_group(const directive_node_group_type&, 
		const char* l, const char* d, const char* r) const;

	template <class Tag>
	void
	__dump_resolved_literal_group(const directive_node_group_type&) const;

	template <class Tag>
	void
	__dump_resolved_literal_group(const directive_node_group_type&, 
		const char* l, const char* d, const char* r) const;

protected:
	using cflat_context_visitor::visit;
	using cflat_visitor::visit;

	void
	__visit(const entity::footprint&);

	void
	visit(const entity::footprint&);

	// override
	void
	visit(const state_instance<bool_tag>&);

	void
	visit(const state_instance<process_tag>&);

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

