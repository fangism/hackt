/**
	\file "AST/identifier.h"
	Base set of classes for the HAC parser.  
	$Id: identifier.h,v 1.7 2011/02/08 02:06:45 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_identifier.h,v 1.7.12.1 2005/12/11 00:45:07 fang Exp
 */

#ifndef __HAC_AST_IDENTIFIER_H__
#define __HAC_AST_IDENTIFIER_H__

#include "AST/node_list.h"
#include "util/sublist.h"

namespace HAC {
using util::sublist;
using util::memory::excl_ptr;

namespace entity {
	class instance_placeholder_base;
}

namespace parser {
using entity::instance_placeholder_base;

//=============================================================================
typedef	node_list<const token_identifier>	qualified_id_base;

/**
	Generalized scoped identifier, referring to a type or instance.  
	Has two modes: absolute or relative, depending on whether or 
	not the un/qualified identifier was prefixed by an additional
	scope (::) operator.  
	Even short unqualified identifiers are wrapped into this class
	for uniform use.  
	When type checking with this class, remember to check the 
	path mode before searching, and use all but the last identifier
	chain as the namespace path prefix.  
	e.g. for A::B::C, search for namespace match of A::B with member C.  
	Defined in "AST/expr.cc".
 */
class qualified_id : public qualified_id_base {
protected:
	typedef	qualified_id_base		parent_type;
public:
	typedef	parent_type::iterator		iterator;
	typedef	parent_type::const_iterator	const_iterator;
	typedef	parent_type::reverse_iterator	reverse_iterator;
	typedef	parent_type::const_reverse_iterator	const_reverse_iterator;
protected:
	/**
		Indicates whether identifier is absolute, meaning
		that it is to be resolved from the global scope down, 
		as opposed to inner scope outward (relative).
		Particularly useful for disambiguation.
	 */
	excl_ptr<const string_punctuation_type>			absolute;
public:
	explicit
	qualified_id(const token_identifier* n);

	explicit
	qualified_id(const count_ptr<const token_identifier>&);

	qualified_id(const qualified_id& i);

	~qualified_id();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
	never_ptr<const object>
	check_build(const context& c) const;

	never_ptr<const instance_placeholder_base>
	lookup_instance(const context& c) const;

using parent_type::begin;
using parent_type::end;
using parent_type::empty;
using parent_type::push_back;
using parent_type::pop_back;
using parent_type::pop_front;

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const string_punctuation_type* s);

	bool
	is_absolute(void) const { return absolute; }

// want a method for splitting off the last id, isolating namespace portion
// copy must be using never_ptrs! original must use excl_ptr
	qualified_id
	copy_namespace_portion(void) const;
		// remember to delete this after done using!
	qualified_id
	copy_beheaded(void) const;

	friend
	ostream&
	operator << (ostream& o, const qualified_id& id);
};	// end class qualified_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just a slice reference to a qualified id.  
 */
class qualified_id_slice {
protected:
	typedef	sublist<count_ptr<const token_identifier> >	parent_type;
	parent_type		parent;
public:
 	typedef	parent_type::list_type::size_type
				size_type;
 	typedef	parent_type::list_type::const_iterator
				const_iterator;
protected:
	const bool		absolute;
public:
	/**
		Constructor (implicit) that take a plain qualified_id.  
		Works because qualified_id is a subclass of
		list<excl_ptr<const token_identifier> >.  
		By default, just wrap with begin and end iterators 
		around the entire list.  
	 */
	qualified_id_slice(const qualified_id& qid);

	qualified_id_slice(const qualified_id_slice& qid);

	~qualified_id_slice();

	bool
	is_absolute(void) const { return absolute; }

	size_type
	size(void) const {
		return size_type(std::distance(parent.begin(), parent.end()));
	}

/***
using parent::begin;
using parent::end;
using parent::rbegin;
using parent::rend;
using parent::empty;
***/
	const_iterator
	begin(void) const { return parent.begin(); }

	const_iterator
	end(void) const { return parent.end(); }

	bool
	empty(void) const { return parent.empty(); }

	qualified_id_slice&
	behead(void) { parent.behead(); return *this; }

	qualified_id_slice&
	betail(void) { parent.betail(); return *this; }

	friend
	ostream&
	operator << (ostream& o, const qualified_id_slice& q);
};	// end class qualified_id_slice

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generalized scoped identifier namespace.  
	Has two modes: absolute or relative, depending on whether or 
	not the un/qualified identifier was prefixed by an additional
	scope (::) operator.  
	Even short unqualified identifiers are wrapped into this class
	for uniform use.  
	When type checking with this class, remember to check the 
	path mode before searching, and use all but the last identifier
	chain as the namespace path prefix.  
	e.g. for A::B::C, search for namespace match of A::B with member C.  
 */
class namespace_id {
protected:
	/**
		Wraps around a qualified_id.  
	 */
	const excl_ptr<qualified_id>	qid;
public:
	explicit
	namespace_id(qualified_id* i);

	~namespace_id();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

//	consider c->lookup_namespace()
//	never_ptr<const object> check_build(context& c) const;

	never_ptr<const qualified_id>
	get_id(void) const { return qid; }
//		{ return never_ptr<const qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const string_punctuation_type* s);

	bool
	is_absolute(void) const;

	friend
	ostream&
	operator << (ostream& o, const namespace_id& id);
};	// end class namespace_id

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_IDENTIFIER_H__

