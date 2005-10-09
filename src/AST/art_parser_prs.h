/**
	\file "AST/art_parser_prs.h"
	PRS-specific syntax tree classes.
	$Id: art_parser_prs.h,v 1.14.30.1 2005/10/09 17:30:21 fang Exp $
 */

#ifndef	__AST_ART_PARSER_PRS_H__
#define	__AST_ART_PARSER_PRS_H__

#include "AST/art_parser_base.h"
#include "AST/art_parser_prs_fwd.h"
#include "AST/art_parser_expr_base.h"
#include "AST/art_parser_definition_item.h"
#include "util/STL/vector_fwd.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
namespace PRS {
	class rule;
}
}
namespace parser {
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
	typedef	count_ptr<entity::PRS::rule>		return_type;
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
	Single production rule.  
 */
class rule : public body_item {
protected:
	const excl_ptr<const expr>		guard;
	const excl_ptr<const char_punctuation_type>	arrow;
	const excl_ptr<const inst_ref_expr>		r;
	const excl_ptr<const char_punctuation_type>	dir;
public:
	rule(const expr* g, const char_punctuation_type* a,
		const inst_ref_expr* rhs, const char_punctuation_type* d);

	~rule();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	PRS_ITEM_CHECK_PROTO;
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
	Collection of production rules.  
 */
class body : public language_body {
	typedef	DEFAULT_VECTOR(body_item::return_type)	checked_rules_type;
protected:
	const excl_ptr<const rule_list>		rules;
public:
	body(const generic_keyword_type* t, const rule_list* r);
	~body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	// needs the return-type of language-body
	ROOT_CHECK_PROTO;
#endif
};	// end class body

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
}	// end namespace ART

#endif	//	__AST_ART_PARSER_PRS_H__

