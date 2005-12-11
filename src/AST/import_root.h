/**
	\file "AST/import_root.h"
	$Id: import_root.h,v 1.2.2.1 2005/12/11 00:45:12 fang Exp $
 */

#ifndef	__AST_IMPORT_ROOT_H__
#define	__AST_IMPORT_ROOT_H__

#include <string>
#include "AST/art_parser_node_list.h"
#include "AST/art_parser_root.h"
#include "AST/art_parser_terminal.h"

namespace HAC {
namespace parser {
using std::string;
//=============================================================================
/**     
	An imported root is returned by the lexer when an import
	directive is encountered.  
	Derived from terminal to inherit automatic line/position tracking.  
 */
class imported_root : public terminal {
	excl_ptr<const root_body>			root;
	string						name;
	bool						seen;
public:
	// imported_root();
	imported_root(excl_ptr<root_body>&, const string&, const bool);
	~imported_root();
	
	ostream&
	what(ostream&) const;

	int
	string_compare(const char*) const;

	using terminal::leftmost;
	using terminal::rightmost;

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

