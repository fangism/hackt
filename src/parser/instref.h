/**
	\file "parser/instref.h"
	Interface to reference-parsing.
	$Id: instref.h,v 1.9.2.1 2010/03/31 00:33:13 fang Exp $
	This file originated from "sim/prsim/Reference.h"
	Id: Reference.h,v 1.5 2006/07/30 05:50:13 fang Exp
 */

#ifndef	__HAC_PARSER_INSTREF_H__
#define	__HAC_PARSER_INSTREF_H__

#include <iosfwd>
#include <vector>
#include "Object/ref/reference_enum.h"
#include "util/string_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/STL/vector_fwd.h"

#define	INVALID_PROCESS_INDEX			size_t(-1)

namespace util {
class directory_stack;
}	// end namespace util

namespace HAC {
namespace entity {
class module;
class meta_reference_union;
struct entry_collection;
}
using util::memory::excl_ptr;

namespace parser {
class inst_ref_expr;

extern
excl_ptr<inst_ref_expr>
parse_reference(const char*);

extern
entity::meta_reference_union
check_reference(const inst_ref_expr&, const entity::module&);

extern
entity::meta_reference_union
parse_and_check_reference(const char*, const entity::module&);

extern
// node_index_type
size_t
parse_node_to_index(const std::string&, const entity::module&);

extern
// process_index_type
size_t
parse_process_to_index(const std::string&, const entity::module&);

extern
entity::global_indexed_reference
parse_global_reference(const std::string&, const entity::module&);

extern
entity::global_indexed_reference
parse_global_reference(const entity::meta_reference_union&,
	const entity::module&);

extern
int
parse_name_to_what(std::ostream&, const std::string&, const entity::module&);

extern
int
parse_name_to_members(std::ostream&,
	const std::string&, const entity::module&);

extern
int
parse_name_to_get_subinstances(
	const std::string&, const entity::module&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subinstances(
	const entity::global_indexed_reference&, const entity::module&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subnodes(
	const std::string&, const entity::module&, 
	std::default_vector<size_t>::type&);
	// node_index_type

extern
int
parse_name_to_get_subnodes_local(
	const std::string&, const entity::module&, 
	std::default_vector<size_t>::type&);

extern
int
parse_name_to_get_ports(
	const std::string&, const entity::module&, 
	std::default_vector<size_t>::type&);

extern
int
parse_name_to_aliases(std::ostream&, const std::string&, 
	const entity::module&, const char* sep);

extern
void
complete_instance_names(const char*, const entity::module&, 
	const util::directory_stack*, std::vector<std::string>&);

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_PARSER_INSTREF_H__

