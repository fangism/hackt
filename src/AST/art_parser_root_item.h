/**
	\file "art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_root_item.h,v 1.1 2005/02/22 08:15:21 fang Exp $
 */

#ifndef __ART_PARSER_ROOT_ITEM_H__
#define __ART_PARSER_ROOT_ITEM_H__

#include "art_parser_root.h"

namespace ART {
namespace parser {
//=============================================================================
/// namespace enclosed body
class namespace_body : public root_item {
protected:
	const excl_ptr<const token_keyword>	ns;	///< keyword "namespace"
	const excl_ptr<const token_identifier>	name;	///< name of namespace
	const excl_ptr<const terminal>		lb;
	const excl_ptr<const root_body>		body;	///< contents of namespace
	const excl_ptr<const terminal>		rb;
	const excl_ptr<const terminal>		semi;	///< semicolon token
public:
	namespace_body(const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c);

	~namespace_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class namespace_body

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public root_item {
protected:
	const excl_ptr<const token_keyword>		open;
	const excl_ptr<const namespace_id>		id;
	const excl_ptr<const token_keyword>		as;
	const excl_ptr<const token_identifier>		alias;
	const excl_ptr<const token_char>		semi;
public:
	using_namespace(const token_keyword* o, const namespace_id* i,
		const token_char* s);
		// a "AS" and n (alias) are optional

	using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n,
		const token_char* s);

	~using_namespace();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class using_namespace

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_ROOT_ITEM_H__

