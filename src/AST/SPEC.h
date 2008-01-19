/**
	\file "AST/SPEC.h"
	$Id: SPEC.h,v 1.4.2.2 2008/01/19 06:46:10 fang Exp $
 */

#ifndef	__HAC_AST_SPEC_H__
#define	__HAC_AST_SPEC_H__

#include "AST/common.h"
#include "AST/SPEC_fwd.h"
#include "AST/lang.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
namespace SPEC {
	class directive;
	class directives_set;
}	// end namespace SPEC
}	// end namespace entity

namespace parser {
namespace SPEC {
//=============================================================================
/**
	Syntax structure for a spec directive
 */
class directive {
public:
	typedef	void					return_type;
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

