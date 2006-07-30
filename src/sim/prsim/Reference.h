/**
	\file "sim/prsim/Reference.h"
	Interface to reference-parsing.
	Consider making this a general function in main/main_funcs, 
		or in AST/reference.h.  
	$Id: Reference.h,v 1.5 2006/07/30 05:50:13 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_REFERENCE_H__
#define	__HAC_SIM_PRSIM_REFERENCE_H__

#include <iosfwd>
#include "sim/common.h"
#include "util/string_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/STL/vector_fwd.h"

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

extern
excl_ptr<parser::inst_ref_expr>
parse_reference(const char*);

extern
entity::meta_reference_union
check_reference(const parser::inst_ref_expr&, const entity::module&);

extern
entity::meta_reference_union
parse_and_check_reference(const char*, const entity::module&);

extern
node_index_type
parse_node_to_index(const std::string&, const entity::module&);

extern
int
parse_name_to_what(std::ostream&, const std::string&, const entity::module&);

extern
int
parse_name_to_get_subnodes(
	std::ostream&, const std::string&, const entity::module&, 
	std::default_vector<node_index_type>::type&);

extern
int
parse_name_to_aliases(std::ostream&, const std::string&, const entity::module&);

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_REFERENCE_H__

