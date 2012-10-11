/** 
	\file "util/syntax.cc"
	$Id: $
 */

#include <iostream>
#include "util/syntax.h"
#include "util/memory/count_ptr.tcc"
#include "util/indent.h"

namespace util {
namespace memory {
// explicit template instantiation
template class count_ptr<syntax::node>;
}
namespace syntax {
// TODO: move lexer/hac_lex.h into util/lexer/
// line_position should belong to util
using HAC::lexer::line_position;
using std::endl;

//=============================================================================
// class tree_walker method definitions

tree_walker::~tree_walker() { }

//=============================================================================
// class node method definitions

node::~node() { }

//=============================================================================
// class terminal method definitions

static
const line_position
null_position;

static
line_position
null_position_fun(void) {
	return null_position;
}

// global static initialization
line_position
(*terminal::position_fun)(void) = &null_position_fun;

terminal::terminal() : pos((*position_fun)()) { }

terminal::~terminal() { }

//=============================================================================
// class string_token method definitions

string_token::string_token(const string& s) : terminal(), str(s) { }

string_token::~string_token() { }

void
string_token::accept(tree_walker& v) const {
	v.visit(*this);
}

ostream&
string_token::what(ostream& o) const {
	return o << str;
}

ostream&
string_token::dump(ostream& o) const {
	return o << str;
}

line_position
string_token::leftmost(void) const {
	return pos;
}

line_position
string_token::rightmost(void) const {
	return pos;
}

//=============================================================================
// class node_list

node_list::node_list() : node(), parent_type() { }

node_list::node_list(node* n) : node(), parent_type() {
	push_back(n);
}

node_list::node_list(const count_ptr<node>& n) : node(), parent_type() {
	push_back(n);
}

node_list::~node_list() { }

void
node_list::push_front(node* n) {
	const count_ptr<node> p(n);
	push_front(p);
}

void
node_list::push_back(node* n) {
	const count_ptr<node> p(n);
	push_back(p);
}

bool
node_list::contains_list(void) const {
	const_iterator i(begin()), e(end());
	for ( ; i!=e; ++i) {
		if (*i) {
			const count_ptr<node_list> c(i->is_a<node_list>());
			if (c) return true;
			// else continue
		}
	}
	return false;
}

void
node_list::accept(tree_walker& v) const {
	v.visit(*this);
}

ostream&
node_list::what(ostream& o) const {
	return o << "(node-list)";	// size?
}

/**
	Do a reasonable job of printing nested LISP-structure
	Consider passing in indent level?
 */
ostream&
node_list::dump(ostream& o) const {
	const bool nest = contains_list() && (size() > 1);
	const_iterator i(begin()), e(end());
	if (nest) {
		o << "( ";
		{
		// newline separated after first item
			INDENT_SECTION(o);
			NEVER_NULL(*i);
			(*i)->dump(o);
			for (++i ; i!=e; ++i) {
				(*i)->dump(o << endl << auto_indent);
			}
		}
		o << " )";
	} else {
		// just on one line, space separated
		o << "(";
		for ( ; i!=e; ++i) {
			(*i)->dump(o << ' ');
		}
		o << " )";
	}
	return o;
}

line_position
node_list::leftmost(void) const {
	if (size() && front())
		return front()->leftmost();
	else	return null_position;
}

line_position
node_list::rightmost(void) const {
	if (size() && back())
		return back()->leftmost();
	else	return null_position;
}

//=============================================================================

}	// end namespace syntax
}	// end namespace util

