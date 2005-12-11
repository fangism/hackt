/**
	\file "AST/namespace.h"
	Base set of classes for the HAC parser.  
	$Id: namespace.h,v 1.1.2.1 2005/12/11 04:41:28 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_root_item.h,v 1.5.48.1 2005/12/11 00:45:10 fang Exp
 */

#ifndef __HAC_AST_NAMESPACE_H__
#define __HAC_AST_NAMESPACE_H__

#include "AST/root.h"

namespace HAC {
namespace parser {
//=============================================================================
/// namespace enclosed body
class namespace_body : public root_item {
protected:
	const excl_ptr<const generic_keyword_type>	ns;	///< keyword "namespace"
	const excl_ptr<const token_identifier>	name;	///< name of namespace
	const excl_ptr<const root_body>		body;	///< contents of namespace
//	const excl_ptr<const char_punctuation_type>	semi;	///< semicolon token
public:
	namespace_body(const generic_keyword_type* s, 
		const token_identifier* n, const root_body* b);

	~namespace_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	ROOT_CHECK_PROTO;
};	// end class namespace_body

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public root_item {
protected:
	const excl_ptr<const generic_keyword_type>	open;
	const excl_ptr<const namespace_id>		id;
//	const excl_ptr<const generic_keyword_type>	as;
	const excl_ptr<const token_identifier>		alias;
//	const excl_ptr<const char_punctuation_type>	semi;
public:
	using_namespace(const generic_keyword_type* o, const namespace_id* i);
		// a "AS" and n (alias) are optional

	using_namespace(const generic_keyword_type* o, const namespace_id* i, 
//		const generic_keyword_type* a,
		const token_identifier* n);

	~using_namespace();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	ROOT_CHECK_PROTO;
};	// end class using_namespace

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_NAMESPACE_H__

