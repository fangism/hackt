/**
	\file "art_parser_range.h"
	Expression-related parser classes for ART.
	$Id: art_parser_range_list.h,v 1.1 2005/02/22 08:15:21 fang Exp $
 */

#ifndef __ART_PARSER_RANGE_LIST_H__
#define __ART_PARSER_RANGE_LIST_H__

#include "art_parser_range.h"
#include "art_parser_node_list.h"

namespace ART {
namespace parser {
//=============================================================================
/// base class for range_list
typedef node_list<const range,none>	range_list_base;

/**
	All sparse range lists are no C-style x[N][M], 
	now limited to 4-dimensions.  
 */
class range_list : public range_list_base {
protected:
	typedef	range_list_base				parent;
	// no additional members
public:
	range_list(const range* r);

	~range_list();

	never_ptr<const object>
	check_build(context& c) const;
};	// end class range_list

#define range_list_wrap(b,l,e)						\
	IS_A(range_list*, l->wrap(b,e))
#define range_list_append(l,d,n)					\
	IS_A(range_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// base class for dense_range_list
typedef node_list<const expr,none>	dense_range_list_base;

/**
	All dense range lists are no C-style x[N][M], 
	now limited to 4-dimensions.  
 */
class dense_range_list : public dense_range_list_base {
protected:
	typedef	dense_range_list_base			parent;
	// no additional members
public:
	explicit
	dense_range_list(const expr* r);

	~dense_range_list();

	never_ptr<const object>
	check_build(context& c) const;
};	// end class range_list

#define dense_range_list_wrap(b,l,e)					\
	IS_A(dense_range_list*, l->wrap(b,e))
#define dense_range_list_append(l,d,n)					\
	IS_A(dense_range_list*, l->append(d,n))

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_RANGE_LIST_H__

