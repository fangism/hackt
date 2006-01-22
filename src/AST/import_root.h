/**
	\file "AST/import_root.h"
	Placeholder for a imported AST root.  
	$Id: import_root.h,v 1.3.2.1 2006/01/22 00:39:58 fang Exp $
	This file used to be the following before it was renamed:
	Id: import_root.h,v 1.2.2.1 2005/12/11 00:45:12 fang Exp
 */

#ifndef	__AST_IMPORT_ROOT_H__
#define	__AST_IMPORT_ROOT_H__

#include <string>
#include "AST/node_list.h"
#include "AST/namespace.h"

// new: 1
#define	NONTERMINAL_IMPORT			1

#if !NONTERMINAL_IMPORT
#include "AST/terminal.h"
#endif

namespace HAC {
namespace parser {
using std::string;
#if NONTERMINAL_IMPORT
class keyword_position;
class token_quoted_string;
#endif
//=============================================================================
/**     
	An imported root is returned by the lexer when an import
	directive is encountered.  
	Derived from terminal to inherit automatic line/position tracking.  
 */
class imported_root
#if !NONTERMINAL_IMPORT
		: public terminal
#endif
		{
	excl_ptr<const root_body>			root;
#if NONTERMINAL_IMPORT
	/// the "import" keyword
	const excl_ptr<const keyword_position>		import;
	/**
		file name, not full-path.
		We keep this for the sake of token position tracking only.  
	 */
	const excl_ptr<const token_quoted_string>	rel_file;
	// screw the semicolon
#endif
	/// full path name to file
	string						name;
	bool						seen;
public:
	// imported_root();
#if NONTERMINAL_IMPORT
	imported_root(excl_ptr<root_body>&,
		excl_ptr<const keyword_position>&,
		excl_ptr<const token_quoted_string>&,
		const string&, 
		const bool);
#else
	imported_root(excl_ptr<root_body>&, const string&, const bool);
#endif
	~imported_root();
	
	ostream&
	what(ostream&) const;

	int
	string_compare(const char*) const;

#if NONTERMINAL_IMPORT
	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
#else
	using terminal::leftmost;
	using terminal::rightmost;
#endif

	ROOT_CHECK_PROTO;

};      // end class imported_root

//=============================================================================
typedef	node_list<const imported_root>		imported_root_list_base;

/**
	Bundle of file imports at the beginning of the file.  
 */
class imported_root_list : public root_item, public imported_root_list_base {
	typedef	imported_root_list_base		parent_type;
public:
	imported_root_list();

	explicit
	imported_root_list(const imported_root*);

	~imported_root_list();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	ROOT_CHECK_PROTO;

};	// end class imported_root_list

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_IMPORT_ROOT_H__

