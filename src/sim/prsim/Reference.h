/**
	\file "sim/prsim/Reference.h"
	Interface to refrence-parsing.
	Consider making this a general function in main/main_funcs.
	$Id: Reference.h,v 1.2.26.1 2006/03/24 03:56:26 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_REFERENCE_H__
#define	__HAC_SIM_PRSIM_REFERENCE_H__

#include <iosfwd>
#include "sim/common.h"
#include "util/string_fwd.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace parser {
class inst_ref_expr;
}
namespace entity {
class module;
class meta_reference_union;
}
namespace SIM {
namespace PRSIM {
using util::memory::excl_ptr;

excl_ptr<parser::inst_ref_expr>
parse_reference(const char*);

entity::meta_reference_union
check_reference(const parser::inst_ref_expr&, const entity::module&);

extern
node_index_type
parse_node_to_index(const std::string&, const entity::module&);

extern
int
parse_name_to_what(std::ostream&, const std::string&, const entity::module&);

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_REFERENCE_H__

