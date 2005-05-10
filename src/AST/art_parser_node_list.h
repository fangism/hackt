/**
	\file "AST/art_parser_node_list.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_node_list.h,v 1.8 2005/05/10 04:51:08 fang Exp $
 */

#ifndef __AST_ART_PARSER_NODE_LIST_H__
#define __AST_ART_PARSER_NODE_LIST_H__

#include "util/STL/list.h"
#include "AST/art_parser_fwd.h"
#include "AST/art_parser_base.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace parser {

USING_LIST
using util::memory::excl_ptr;
using util::memory::count_ptr;

//=============================================================================
// replacement node list

class node_position;

/**
	Characteristics of syntax details and appearance of 
	node_list template classes.  
	This is just the default definition.
	Each list type may specialize with its own delimiters.  
 */
template <class L>
class node_list_traits {
	static const char open[];
	static const char close[];
	static const char delim[];
	// other useful traits?
};	// end class node_list_traits

#define	NODE_LIST_TEMPLATE_SIGNATURE					\
template <class T>

/**
	To make this print the desired delimiter, 
	we will define an overrideable template that depends on this, 
	OR rely on inferring the delimiter from the value_type parameter.  
	Delimiter is only useful for spitting out source.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
class node_list {
	typedef	node_list<T>				this_type;
protected:
	// consider using a vector
	typedef	list<count_ptr<T> >			list_type;
protected:
	typedef	typename list_type::value_type		value_type;
	typedef	typename list_type::iterator		iterator;
	typedef	typename list_type::const_iterator	const_iterator;
	typedef	typename list_type::reverse_iterator	reverse_iterator;
	typedef	typename list_type::const_reverse_iterator
							const_reverse_iterator;
protected:
	list_type					nodes;
	// consider count_ptr to make copy-constructible
	count_ptr<const node_position>			open;
	count_ptr<const node_position>			close;
public:
	node_list();

	explicit
	node_list(const T*);

	~node_list();

	const list_type&
	raw_list(void) const { return nodes; }

	iterator
	begin(void) { return nodes.begin(); }

	const_iterator
	begin(void) const { return nodes.begin(); }

	iterator
	end(void) { return nodes.end(); }

	const_iterator
	end(void) const { return nodes.end(); }

	bool
	empty(void) const { return nodes.empty(); }

	size_t
	size(void) const { return nodes.size(); }

#if 0
	this_type*
	append(const T*);		// basically push_back
#endif
	void
	push_back(const T* p) { nodes.push_back(value_type(p)); }

	void
	push_front(const T* p) { nodes.push_front(value_type(p)); }

	void
	pop_back(void) { nodes.pop_back(); }

	void
	pop_front(void) { nodes.pop_front(); }

	void
	wrap(const node_position* o, const node_position* c) {
		open = count_ptr<const node_position>(o);
		close = count_ptr<const node_position>(c);
	}

	void
	release_append(this_type&);

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	line_range
	where(void) const;
#endif

	/**
		Consider giving this an aggregate return type.  
		Note that this is NON-VIRTUAL.  
		Determine the return type from the template paraameter.
	 */
	never_ptr<const object>
	check_build(context&) const;

};	// end class node_list

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_NODE_LIST_H__

