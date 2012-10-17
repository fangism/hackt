/**
	\file "AST/PRS.hh"
	PRS-specific syntax tree classes.
	$Id: PRS.hh,v 1.15 2011/02/08 02:06:44 fang Exp $
	This used to be the following before it was renamed:
	Id: art_parser_prs.h,v 1.15.12.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_PRS_H__
#define	__HAC_AST_PRS_H__

#include "AST/common.hh"
#include "AST/PRS_fwd.hh"
#include "AST/expr_base.hh"
#include "AST/lang.hh"
#include "AST/attribute.hh"
#include "util/STL/pair_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "Object/devel_switches.hh"	// for PRS_SUPPLY_OVERRIDES

namespace HAC {
namespace entity {
// be careful of namespaces...
	class generic_attribute;
namespace PRS {
	class rule;
	class rule_conditional;
	class precharge_expr;
}
}
namespace parser {
class inst_ref_expr_list;
struct expr_attr_list;
/**
	This is the namespace for the PRS sub-language.  
 */
namespace PRS {
using util::memory::count_ptr;

//=============================================================================
// local forward declarations

class rule;
class body;

//=============================================================================
/// a single production rule
class body_item {
public:
	typedef	void				return_type;
protected:
	// no members
public:
	body_item();

virtual	~body_item();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#define	PRS_ITEM_CHECK_PROTO						\
	body_item::return_type						\
	check_rule(context&) const

virtual	PRS_ITEM_CHECK_PROTO = 0;
};	// end class body_item

//=============================================================================
/**
	Pairs a literal instance reference with an optional list of parameters.
	May need to extract single unqualified ID for PRS macro.  
	Need to derive the interface from inst_ref_expr to be able to
	use as parts of expression trees.  
 */
class literal : public inst_ref_expr {
	typedef	inst_ref_expr::meta_return_type		meta_return_type;
//	typedef	inst_ref_expr::nonmeta_return_type	nonmeta_return_type;
	/// not const, b/c we may wish to transfer it to macro
	excl_ptr<inst_ref_expr>				ref;
	/// not const, b/c we may wish to transfer it to macro
	excl_ptr<const expr_attr_list>			params;
	/**
		If true this refers to an internal node, 
		and should use a different lookup.  
	 */
	bool						internal;
public:
	explicit
	literal(inst_ref_expr*);

	explicit
	literal(excl_ptr<inst_ref_expr>&);

	literal(inst_ref_expr*, const expr_attr_list*);

	~literal();

#if 0
	excl_ptr<inst_ref_expr>&
	release_reference(void) {
		return ref;
	}
#endif

	excl_ptr<const token_identifier>
	extract_identifier(void);

	excl_ptr<const expr_attr_list>
	extract_parameters(void);

	void
	attach_parameters(const expr_attr_list*);

	void
	mark_internal(void) { internal = true; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	/// This wraps a call to ref's check, and also attaches/checks params
	CHECK_META_REFERENCE_PROTO;
	/// this is not actually used
	CHECK_NONMETA_REFERENCE_PROTO;
	EXPAND_CONST_REFERENCE_PROTO;

	// overrides base
	prs_literal_ptr_type
	check_prs_literal(const context&) const;

	prs_literal_ptr_type
	check_prs_rhs(context&) const;

	// no need to override the following, because they all defer to
	// the above two (pure virtual) methods.  
	// CHECK_META_EXPR_PROTO
	// CHECK_NONMETA_EXPR_PROTO
	// CHECK_GENERIC_PROTO
	// CHECK_PRS_EXPR_PROTO

protected:
	typedef	entity::generic_attribute		attribute_type;

public:
	// so macro::check_prs_rule can use this
	static
	attribute_type
	check_literal_attribute(const generic_attribute&, const context&);

};	// end class literal

//=============================================================================
/**
	Store precharge expression.
 */
class precharge {
	const excl_ptr<const node_position>	dir;
	const excl_ptr<const expr>		pchg_expr;
public:
	precharge(const node_position*, const expr*);
	~precharge();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	entity::PRS::precharge_expr
	check_prs_expr(context& c) const;
	// CHECK_PRS_EXPR_PROTO;

};	// end class precharge

//=============================================================================
/**
	Single production rule.  
 */
class rule : public body_item {
public:
	typedef	entity::generic_attribute		attribute_type;
protected:
	const excl_ptr<const generic_attribute_list>		attribs;
	const excl_ptr<const expr>		guard;
	const excl_ptr<const char_punctuation_type>	arrow;
	const excl_ptr<const literal>		r;
	const excl_ptr<const char_punctuation_type>	dir;
public:
	rule(const generic_attribute_list*, const expr* g, 
		const char_punctuation_type* a,
		literal* rhs, const char_punctuation_type* d);

	~rule();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	PRS_ITEM_CHECK_PROTO;

protected:
	static
	attribute_type
	check_prs_attribute(const generic_attribute&, context&);
};	// end class rule

//=============================================================================
/**
	Repetition of production rules in a loop.  
 */
class loop : public body_item {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const rule_list>		rules;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop(const char_punctuation_type* l,
		const token_identifier* id, const range* b,
		const rule_list* rl, const char_punctuation_type* r);

	~loop();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	PRS_ITEM_CHECK_PROTO;
};	// end class loop

//=============================================================================
/**
	Guarded PRS body clause.  
 */
class guarded_body {
	const excl_ptr<const expr>			guard;
	const excl_ptr<const char_punctuation_type>	arrow;
	const excl_ptr<const rule_list>			rules;
public:
	typedef	void				return_type;
public:
	guarded_body(const expr*, const char_punctuation_type*,
		const rule_list*);
	~guarded_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_clause(context&) const;

};	// end class guarded_body

//=============================================================================
/**
	Class for wrapping production rules inside conditionals.  
 */
class conditional : public body_item {
	const excl_ptr<const guarded_prs_list>		gp;
public:
	// explicit
	conditional(const guarded_prs_list*);
	~conditional();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	PRS_ITEM_CHECK_PROTO;
};	// end class conditional

//=============================================================================
/**
	temporary hacks:
	PRS-macros look like function calls.  
	The programmer can design these to do whatever.  
 */
class macro : public body_item {
	// these are rule-style attributes, we currently ignore
	const excl_ptr<const generic_attribute_list>		attribs;
	excl_ptr<const token_identifier>		name;
	// attributes also go here
	excl_ptr<const expr_attr_list>			params;
	const excl_ptr<const inst_ref_expr_list>	args;
public:
	macro(const generic_attribute_list*, literal*, 
		const inst_ref_expr_list*);
	~macro();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	PRS_ITEM_CHECK_PROTO;
};	// end class macro

//=============================================================================
/**
	Collection of production rules.  
	Now is also a body_item because of nested bodies.  
 */
class body : public language_body, public body_item {
protected:
#if PRS_SUPPLY_OVERRIDES
	const excl_ptr<const inst_ref_expr_list>	supplies;
#endif
	const excl_ptr<const rule_list>		rules;
public:
	body(const generic_keyword_type* t, 
#if PRS_SUPPLY_OVERRIDES
		const inst_ref_expr_list*, 
#endif
		const rule_list* r);
virtual	~body();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	// needs the return-type of language-body
// virtual?
	ROOT_CHECK_PROTO;
	PRS_ITEM_CHECK_PROTO;

protected:
	bool
	__check_rules(context&) const;

};	// end class body

//=============================================================================
/**
	Structure for grouping rules into subcircuits.  
 */
class subcircuit : public body {
	/// really, only use a name/string parameter for now
	const excl_ptr<const expr_list>		params;
public:
	subcircuit(const generic_keyword_type*, 
		const expr_list*, const rule_list*);
	~subcircuit();

	ostream&
	what(ostream& o) const;

	using body::leftmost;
	using body::rightmost;

	// needs the return-type of language-body
	ROOT_CHECK_PROTO;
	PRS_ITEM_CHECK_PROTO;

};	// end class subcircuit

//=============================================================================
/**
	Shortcut loop of AND or OR operation.  
	Don't know if this will be useful outside of the PRS context.  
 */
class op_loop : public expr {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const char_punctuation_type>	op;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const expr>		ex;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	op_loop(const char_punctuation_type* l,
		const char_punctuation_type* o,
		const token_identifier* id, 
		const range* b, 
		const expr* e, const char_punctuation_type* r);

	~op_loop();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
	CHECK_PRS_EXPR_PROTO;
};	// end class op_loop

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace HAC

#endif	//	__HAC_AST_PRS_H__

