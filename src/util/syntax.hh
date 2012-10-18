/**
	\file "util/syntax.hh"
	Generic tree structure for syntax.
	Kind of reminiscent of LISP structures.  
	Uses visitor pattern.
	$Id: $
 */

#ifndef	__UTIL_SYNTAX_H__
#define	__UTIL_SYNTAX_H__

#include <iosfwd>
#include <list>
#include <string>
#include "util/syntax_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "lexer/hac_lex.hh"

namespace util {
namespace syntax {

using std::ostream;
using std::list;
using std::string;
using memory::count_ptr;
using HAC::lexer::line_position;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class tree_walker {
public:
virtual	~tree_walker();

virtual	void
	visit(const terminal&) = 0;

virtual	void
	visit(const node_list&) = 0;
};	// end class tree_walker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Abstract base class.
 */
class node {
public:
virtual ~node();

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&) const = 0;

virtual	void
	accept(tree_walker&) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

};	// end class node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	includes positional information.
 */
class terminal : public node {
protected:
	line_position					pos;
	terminal();
public:
	// overridable global static function pointer
	// can use value_saver<> to temporarily override
	static
	line_position
	(*position_fun)(void);

virtual	~terminal();
virtual	void
	accept(tree_walker&) const = 0;
};	// end class terminal

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class string_token : public terminal {
public:
	string						str;

	explicit
	string_token(const string&);

	~string_token();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	void
	accept(tree_walker&) const;

};	// end class string_token

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic grouping structure.
 */
class node_list : public node, public list<count_ptr<node> > {
	typedef	list<count_ptr<node> >		parent_type;
	typedef	count_ptr<node>			value_type;

// private default dtor
	node_list();

public:
	explicit
	node_list(node*);

	explicit
	node_list(const count_ptr<node>&);

	~node_list();

	using parent_type::push_front;
	using parent_type::push_back;

	void
	push_front(node*);

	void
	push_back(node*);

	bool
	contains_list(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	void
	accept(tree_walker&) const;
};	// end class node_list

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace syntax
}	// end namespace util

#endif	// __UTIL_SYNTAX_H__
