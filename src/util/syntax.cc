/** 
	\file "util/syntax.cc"
	$Id: $
 */

#include "util/syntax.h"
#include "util/memory/count_ptr.tcc"

namespace util {
namespace syntax {
// TODO: move lexer/hac_lex.h into util/lexer/
// line_position should belong to util
using HAC::lexer::line_position;

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

void
node_list::accept(tree_walker& v) const {
	v.visit(*this);
}

ostream&
node_list::what(ostream& o) const {
	return o << "(node-list)";	// size?
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

