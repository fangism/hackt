/**
	\file "art_parser_prs.h"
	PRS-specific syntax tree classes.
	$Id: art_parser_prs.h,v 1.9.4.3 2005/05/04 17:23:19 fang Exp $
 */

#ifndef	__ART_PARSER_PRS_H__
#define	__ART_PARSER_PRS_H__

#include "art_parser_prs_fwd.h"
#include "art_parser_expr_base.h"
#include "art_parser_definition_item.h"

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
class body_item {
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

#if 1
virtual	never_ptr<const object>
	check_build(context& c) const = 0;
#endif
};	// end class body_item

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

#if 1
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
	const excl_ptr<const char_punctuation_type>	lp;
//	const excl_ptr<const char_punctuation_type>	col1;
	const excl_ptr<const token_identifier>	index;
//	const excl_ptr<const char_punctuation_type>	col2;
	const excl_ptr<const range>		bounds;
//	const excl_ptr<const char_punctuation_type>	col3;
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

#if 1
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
	body(const generic_keyword_type* t, const rule_list* r);
	~body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
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
	const excl_ptr<const token_char>	op;
	const excl_ptr<const char_punctuation_type>	col1;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const char_punctuation_type>	col2;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const char_punctuation_type>	col3;
	const excl_ptr<const expr>		ex;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	op_loop(const char_punctuation_type* l,
		const token_char* o,
		const char_punctuation_type* c1,
		const token_identifier* id, const char_punctuation_type* c2, 
		const range* b, const char_punctuation_type* c3, 
		const expr* e, const char_punctuation_type* r);

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

