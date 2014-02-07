/**
	\file "AST/SPEC.hh"
	$Id: SPEC.hh,v 1.7 2010/09/29 00:13:37 fang Exp $
 */

#ifndef	__HAC_AST_SPEC_H__
#define	__HAC_AST_SPEC_H__

#include "AST/common.hh"
#include "AST/SPEC_fwd.hh"
#include "AST/lang.hh"

namespace HAC {
namespace entity {
namespace SPEC {
	class directive;
	class directives_set;
}	// end namespace SPEC
}	// end namespace entity

namespace parser {
class expr;		// used for PRS-expr
class token_string;
namespace SPEC {
//=============================================================================
/**
	Abstract base class for SPEC directives.  
 */
class directive_base {
public:
	typedef	void					return_type;
public:
virtual	~directive_base();

	PURE_VIRTUAL_NODE_METHODS

virtual	return_type
	check_spec(context&) const = 0;

};	// end class directive_base

//=============================================================================
/**
	Syntax structure for a spec directive
	Kinda similar/identical to PRS-macros.  
 */
class directive : public directive_base {
public:
	typedef	directive_base::return_type		return_type;
private:
	const excl_ptr<const token_identifier>		name;
	const excl_ptr<const expr_list>			params;
	const excl_ptr<const inst_ref_expr_list>	args;
public:
	directive(const token_identifier*, const expr_list*,
		const inst_ref_expr_list*);
	~directive();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_spec(context&) const;

};	// end class directive

//=============================================================================
/**
	Production rule invariant expression, always-assert.
 */
class invariant : public directive_base {
	const excl_ptr<const expr>			_expr;
	const excl_ptr<const token_string>		_msg;
	/// if true, use RTE syntax, else use PRS syntax
	bool						rte_mode;
public:
	typedef	directive_base::return_type		return_type;

	invariant(const expr* const, const token_string* const s = NULL, 
		const bool rm = false);
	~invariant();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_spec(context&) const;

};	// end class invariant

//=============================================================================
/**
	SPEC language body.
 */
class body : public language_body {
protected:
	const excl_ptr<const directive_list>		directives;
public:
	body(const generic_keyword_type*, const directive_list*);
	~body();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	// uses the return type of language_body
	ROOT_CHECK_PROTO;

protected:
	bool
	__check_specs(context&) const;

};	// end class body

//=============================================================================
}	// end namespace SPEC
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_SPEC_H__

