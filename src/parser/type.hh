/**
	\file "parser/type.hh"
	$Id: type.hh,v 1.1 2006/08/01 06:35:55 fang Exp $
 */

#ifndef	__HAC_PARSER_TYPE_H__
#define	__HAC_PARSER_TYPE_H__

#include <iosfwd>
#include "AST/type_base.hh"

namespace HAC {
namespace entity {
class module;
class footprint;
}

namespace parser {
using entity::module;
using entity::footprint;

extern
concrete_type_ref::return_type
parse_and_check_complete_type(const char*, const module&);

extern
const footprint*
parse_to_footprint(const char*, const module&);

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_PARSER_TYPE_H__

