/**
	\file "Object/lang/CHP_context_printer.cc"
	Implementation of CHP context printer.  
	$Id: CHP_context_printer.cc,v 1.1.2.2 2007/09/20 04:26:48 fang Exp $
 */

#include <iostream>
#include "Object/lang/CHP_context_printer.h"
#include "Object/expr/bool_expr.h"
#include "common/ICE.h"
#include "util/value_saver.h"
#include "util/indent.h"
#include "util/type_traits.h"
#include "util/static_assert.h"
#include "util/wtf.h"

namespace HAC {
namespace entity {
namespace CHP {
using util::value_saver;
using util::auto_indent;
#include "util/using_ostream.h"

//=============================================================================
// class chp_context_printer method definitions

chp_context_printer::chp_context_printer(const action& a, ostream& o, 
		const expr_dump_context& d) :
		chp_visitor(), 
		os(o), 
//		stmt(&a), 
		dump_context(d), 
		path(), here(),
//		end()
		print_all(false)
		{
	make_action_parent_path(a, path);
	INVARIANT(!path.empty());
	here = path.begin();
//	end = path.end();
//	INVARIANT(here != end);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::operator () (void) {
	// here points to the root action
	INVARIANT(here != path.end());
	const path_iterator _this(here);
	const value_saver<path_iterator> v(here);
	++here;
	NEVER_NULL(*_this);
	(*_this)->accept(*this);
	os << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::print_if_match(action_list_type::const_reference a,
		const action* sub) {
	NEVER_NULL(a);
	os << auto_indent;
	if (print_all || (a == sub)) {
		// recurse
		a->accept(*this);
	} else {
		// skip
		os << "...";
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::print_if_match(selection_list_type::const_reference a,
		const action* sub) {
	NEVER_NULL(a);
	os << auto_indent;
	if (print_all || (a->get_action() == sub)) {
		// recurse
		a->accept(*this);
	} else {
		// skip
		os << "... -> ...";
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Prints all list elements.  
 */
template <class ListType>
void
chp_context_printer::print_list(const ListType& l, const char* delim) {
	typedef	typename ListType::const_iterator	const_iterator;
	NEVER_NULL(delim);
	// print the whole sequence
	const path_iterator match(here);
	const value_saver<path_iterator> v(here);
	INDENT_SECTION(os);
	++here;
	const_iterator i(l.begin()), e(l.end());
	INVARIANT(i != e);
	(*i)->accept(*this);
	for (++i; i!=e; ++i) {
		os << delim << auto_indent;
		(*i)->accept(*this);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class ListType>
void
chp_context_printer::print_list_on_match(const ListType& l, 
		const char* open, const char* delim, const char* close) {
	NEVER_NULL(delim);
	// print the whole sequence
	// print only the one that matches
	if (open)
		os << open << endl;
{
	typedef	typename ListType::const_iterator	const_iterator;
	INDENT_SECTION(os);
	const path_iterator match(here);
	const value_saver<path_iterator> v(here);
	++here;
	const_iterator i(l.begin()), e(l.end());
	INVARIANT(i != e);
	print_if_match(*i, *match);
	for (++i; i!=e; ++i) {
		os << delim;
		print_if_match(*i, *match);
	}
}
	if (close)
		os << endl << auto_indent << close;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const guarded_action& s) {
	// print the whole sequence
	if (s.get_guard())
		s.get_guard()->dump(os, dump_context);
	else	os << "else";
	os << " -> ";	// endl?
	if (s.get_action())
		s.get_action()->accept(*this);
	else	os << "skip";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const action_sequence& s) {
if (s.size() == 1) {
	// don't bother wrapping singletons in braces
	const path_iterator match(here);
	const value_saver<path_iterator> v(here);
	++here;
	print_if_match(s.front(), *match);
} else if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	print_list_on_match(s, "{", ";\n", "}");
} else {
	print_list_on_match(s, "{", ";\n", "}");
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const concurrent_actions& s) {
if (s.size() == 1) {
	// don't bother wrapping singletons in braces
	const path_iterator match(here);
	const value_saver<path_iterator> v(here);
	++here;
	print_if_match(s.front(), *match);
} else if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	print_list_on_match(s, "{", ",\n", "}");
} else {
	print_list_on_match(s, "{", ",\n", "}");
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const deterministic_selection& s) {
if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	print_list_on_match(s, "[", " []\n", "]");
} else {
	print_list_on_match(s, "[", " []\n", "]");
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const nondeterministic_selection& s) {
if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	print_list_on_match(s, "[", " :\n", "]");
} else {
	print_list_on_match(s, "[", " :\n", "]");
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const do_while_loop& s) {
if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	print_list_on_match(s, "*[", " []\n", "]");
} else {
	print_list_on_match(s, "*[", " []\n", "]");
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_context_printer::visit(const do_forever_loop& s) {
	os << "*[";
{
	// INDENT_SECTION(os);
	const path_iterator match(here);
	const value_saver<path_iterator> v(here);
	++here;
	NEVER_NULL(s.get_body());
if (here == path.end()) {
	// print the whole sequence
//	s.dump(os, dump_context);
	const value_saver<bool> B(print_all);
	print_all = true;
	s.get_body()->accept(*this);
} else {
	s.get_body()->accept(*this);
}
}
	os << "]";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	PRINT_NEVER_REACH(type)						\
void									\
chp_context_printer::visit(const type& a) {				\
	ICE_NEVER_CALL(cerr);						\
}

PRINT_NEVER_REACH(metaloop_selection)
PRINT_NEVER_REACH(metaloop_statement)

#undef	PRINT_NEVER_REACH
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	PRINT_LEAF_TYPE(type)						\
void									\
chp_context_printer::visit(const type& a) {				\
	if (!print_all) {						\
		INVARIANT(here == path.end());				\
	}								\
	a.dump(os, dump_context);					\
}
// need endl?

PRINT_LEAF_TYPE(assignment)
PRINT_LEAF_TYPE(condition_wait)
PRINT_LEAF_TYPE(channel_send)
PRINT_LEAF_TYPE(channel_receive)
PRINT_LEAF_TYPE(function_call_stmt)

#undef	PRINT_LEAF_TYPE

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC


