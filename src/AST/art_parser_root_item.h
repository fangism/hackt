/**
	\file "art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_root_item.h,v 1.1.18.2 2005/04/10 21:36:38 fang Exp $
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
	const excl_ptr<const generic_keyword_type>	ns;	///< keyword "namespace"
	const excl_ptr<const token_identifier>	name;	///< name of namespace
	const excl_ptr<const root_body>		body;	///< contents of namespace
	const excl_ptr<const char_punctuation_type>	semi;	///< semicolon token
public:
	namespace_body(const generic_keyword_type* s, 
		const token_identifier* n, const root_body* b,
		const char_punctuation_type* c);

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
	const excl_ptr<const generic_keyword_type>	open;
	const excl_ptr<const namespace_id>		id;
	const excl_ptr<const generic_keyword_type>	as;
	const excl_ptr<const token_identifier>		alias;
	const excl_ptr<const char_punctuation_type>	semi;
public:
	using_namespace(const generic_keyword_type* o, const namespace_id* i,
		const char_punctuation_type* s);
		// a "AS" and n (alias) are optional

	using_namespace(const generic_keyword_type* o, const namespace_id* i, 
		const generic_keyword_type* a, const token_identifier* n,
		const char_punctuation_type* s);

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

