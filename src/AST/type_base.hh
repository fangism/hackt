/**
	\file "AST/type.hh"
	Base set of classes for the HAC parser.  
	$Id: type_base.hh,v 1.4 2006/07/30 05:49:18 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_type_base.h,v 1.6.34.1 2005/12/11 00:45:12 fang Exp
 */

#ifndef __HAC_AST_TYPE_BASE_H__
#define __HAC_AST_TYPE_BASE_H__

#include "AST/common.hh"
#include "util/memory/count_ptr.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace entity {
	class module;
	class definition_base;
	class fundamental_type_reference;
}
namespace parser {
using entity::fundamental_type_reference;
using util::memory::count_ptr;
using util::memory::excl_ptr;
//=============================================================================
/**
	Abstract base class for types in general (parameters, data, channel, 
	process...)
	Does not include any template parameters.  
 */
class type_base {
public:
	typedef	never_ptr<const entity::definition_base>	return_type;

public:
	type_base() { }

virtual	~type_base() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#define	TYPE_BASE_CHECK_PROTO						\
	type_base::return_type						\
	check_definition(const context&) const

	/**
		Should return valid pointer to a fundamental type definition, 
		parameter, data, channel, or process.  
	 */
virtual	TYPE_BASE_CHECK_PROTO = 0;

virtual	bool
	is_atomic(void) const;
};	// end class type_base

//=============================================================================
/**
	Base complete type abstract class.  
 */
class concrete_type_ref {
public:
	typedef	count_ptr<const fundamental_type_reference>	return_type;

public:
	concrete_type_ref() { }
virtual ~concrete_type_ref() { }

	PURE_VIRTUAL_NODE_METHODS

virtual return_type
	check_type(const context&) const = 0;

virtual	bool
	is_atomic(void) const;
};      // end class concrete_type_ref

//=============================================================================
// public functions

// calls yacc-generated type_parse()
extern
concrete_type_ref::return_type
parse_and_check_complete_type(const char*, const entity::module&);

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_TYPE_BASE_H__

