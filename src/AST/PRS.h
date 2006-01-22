/**
	\file "AST/PRS.h"
	PRS-specific syntax tree classes.
	$Id: PRS.h,v 1.3 2006/01/22 06:52:52 fang Exp $
	This used to be the following before it was renamed:
	Id: art_parser_prs.h,v 1.15.12.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_PRS_H__
#define	__HAC_AST_PRS_H__

#include "AST/common.h"
#include "AST/PRS_fwd.h"
#include "AST/expr_base.h"
#include "AST/definition_item.h"
#include "util/STL/vector_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
// be careful of namespaces...
namespace PRS {
	class rule;
	class attribute;
}
}
namespace parser {
class inst_ref_expr_list;
/**
	This is the namespace for the PRS sub-language.  
 */
namespace PRS {
using util::memory::count_ptr;
using std::default_vector;

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
	const excl_ptr<const attribute_list>		attribs;
	const excl_ptr<const expr>		guard;
	const excl_ptr<const char_punctuation_type>	arrow;
	const excl_ptr<const inst_ref_expr>		r;
	const excl_ptr<const char_punctuation_type>	dir;
public:
	rule(const attribute_list*, const expr* g, 
		const char_punctuation_type* a,
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
	typedef	default_vector<body_item::return_type>::type
				checked_rules_type;
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
	temporary hacks:
	PRS-macros look like function calls.  
	The programmer can design these to do whatever.  
 */
class macro : public body_item {
	const excl_ptr<const token_identifier>		name;
	const excl_ptr<const inst_ref_expr_list>	args;
public:
	macro(const token_identifier*, const inst_ref_expr_list*);
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
 */
class body : public language_body {
	typedef	default_vector<body_item::return_type>::type
				checked_rules_type;
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
/**
	Production rule attributes.
 */
class attribute {
	const excl_ptr<const token_identifier>		key;
	const excl_ptr<const expr_list>			values;
public:
	typedef	entity::PRS::attribute			return_type;
public:
	attribute(const token_identifier*, const expr_list*);
	~attribute();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	// TODO: param can be const& after branch merge...
	return_type
	check(context&) const;
};	// end class attribute

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace HAC

#endif	//	__HAC_AST_PRS_H__

