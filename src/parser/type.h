/**
	\file "parser/type.h"
	$Id: type.h,v 1.1 2006/08/01 06:35:55 fang Exp $
 */

#ifndef	__HAC_PARSER_TYPE_H__
#define	__HAC_PARSER_TYPE_H__

#include <iosfwd>
#include "AST/type_base.h"

namespace HAC {
namespace entity {
class module;
}

namespace parser {

extern
concrete_type_ref::return_type
parse_and_check_complete_type(const char*, const entity::module&);

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_PARSER_TYPE_H__

