/**
	\file "art_parser_prs.h"
	PRS-specific syntax tree classes.
	$Id: art_parser_prs.h,v 1.6 2005/01/14 00:00:53 fang Exp $
 */

#ifndef	__ART_PARSER_PRS_H__
#define	__ART_PARSER_PRS_H__

#include "art_parser_base.h"

namespace ART {
namespace parser {
/**
	This is the namespace for the PRS sub-language.  
 */
namespace PRS {

//=============================================================================
// local forward declarations

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

typedef node_list<const body_item>		rule_list;
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
	const excl_ptr<const expr>		guard;
	const excl_ptr<const terminal>		arrow;
	const excl_ptr<const expr>		r;
	const excl_ptr<const terminal>		dir;
public:
	rule(const expr* g, const terminal* a,
		const expr* rhs, const terminal* d);

	~rule();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	never_ptr<const object>
	check_build(context& c) const;
#endif
};	// end class rule

//=============================================================================
/**
	Repetition of production rules in a loop.  
 */
class loop : public body_item {
protected:
	const excl_ptr<const token_char>	lp;
	const excl_ptr<const token_char>	col1;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const token_char>	col2;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const token_char>	col3;
	const excl_ptr<const rule_list>		rules;
	const excl_ptr<const token_char>	rp;
public:
	loop(const token_char* l, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const rule_list* rl, const token_char* r);

	~loop();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	never_ptr<const object>
	check_build(context& c) const;
#endif
};	// end class loop

//=============================================================================
/**
	Collection of production rules.  
 */
class body : public language_body {
protected:
	const excl_ptr<const rule_list>		rules;
public:
	body(const token_keyword* t, const rule_list* r);
	~body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};	// end class body

//=============================================================================
/**
	Shortcut loop of AND or OR operation.  
	Don't know if this will be useful outside of the PRS context.  
 */
class op_loop : public expr {
protected:
	const excl_ptr<const token_char>	lp;
	const excl_ptr<const token_char>	op;
	const excl_ptr<const token_char>	col1;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const token_char>	col2;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const token_char>	col3;
	const excl_ptr<const expr>		ex;
	const excl_ptr<const token_char>	rp;
public:
	op_loop(const token_char* l, const token_char* o, const token_char* c1,
		const token_identifier* id, const token_char* c2, 
		const range* b, const token_char* c3, 
		const expr* e, const token_char* r);

	~op_loop();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class op_loop

//=============================================================================
}	// end namespace PRS
}	// end namespace parser
}	// end namespace ART

#endif	//	__ART_PARSER_PRS_H__

