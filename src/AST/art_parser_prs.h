// "art_parser_prs.h"
// PRS-specific syntax tree

#ifndef	__ART_PARSER_PRS_H__
#define	__ART_PARSER_PRS_H__

#include "art_macros.h"

namespace ART {
namespace parser {
//=============================================================================
// forward declarations
class node;
class expr;
class terminal;
class language_body;
class range;

/**
	This is the namespace for the PRS sub-language.  
 */
namespace PRS {

//=============================================================================
// forward declarations

class rule;
class body;

//=============================================================================
/// a single production rule
class body_item : public node {
protected:
	// no members
public:
	body_item();
virtual	~body_item();

};	// end class body_item

typedef node_list<body_item>		rule_list;
#define prs_rule_list_wrap(b,l,e)					\
	IS_A(PRS::rule_list*, l->wrap(b,e))
#define prs_rule_list_append(l,d,n)					\
	IS_A(PRS::rule_list*, l->append(d,n))

//=============================================================================
/**
	Single production rule.  
 */
class rule : public body_item {
protected:
	const excl_const_ptr<expr>		guard;
	const excl_const_ptr<terminal>		arrow;
	const excl_const_ptr<expr>		r;
	const excl_const_ptr<terminal>		dir;
public:
	rule(const expr* g, const terminal* a,
		const expr* rhs, const terminal* d);
	~rule();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
//	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class rule

//=============================================================================
/**
	Repetition of production rules in a loop.  
 */
class loop : public body_item {
protected:
	const excl_const_ptr<token_char>	lp;
	const excl_const_ptr<token_char>	col1;
	const excl_const_ptr<token_identifier>	index;
	const excl_const_ptr<token_char>	col2;
	const excl_const_ptr<range>		bounds;
	const excl_const_ptr<token_char>	col3;
	const excl_const_ptr<rule_list>		rules;
	const excl_const_ptr<token_char>	rp;
public:
	loop(const token_char* l, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const rule_list* rl, const token_char* r);
	~loop();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
//	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class loop

//=============================================================================
/**
	Collection of production rules.  
 */
class body : public language_body {
protected:
	const excl_const_ptr<rule_list>		rules;
public:
	body(const token_keyword* t, const rule_list* r);
	~body();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class body

//=============================================================================
/**
	Shortcut loop of AND or OR operation.  
	Don't know if this will be useful outside of the PRS context.  
 */
class op_loop : public expr {
protected:
	const excl_const_ptr<token_char>	lp;
	const excl_const_ptr<token_char>	op;
	const excl_const_ptr<token_char>	col1;
	const excl_const_ptr<token_identifier>	index;
	const excl_const_ptr<token_char>	col2;
	const excl_const_ptr<range>		bounds;
	const excl_const_ptr<token_char>	col3;
	const excl_const_ptr<expr>		ex;
	const excl_const_ptr<token_char>	rp;
public:
	op_loop(const token_char* l, const token_char* o, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const expr* e, const token_char* r);
	~op_loop();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class op_loop

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace ART

#endif	//	__ART_PARSER_PRS_H__

