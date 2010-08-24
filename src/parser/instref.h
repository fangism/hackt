/**
	\file "parser/instref.h"
	Interface to reference-parsing.
	$Id: instref.h,v 1.12 2010/08/24 18:08:41 fang Exp $
	This file originated from "sim/prsim/Reference.h"
	Id: Reference.h,v 1.5 2006/07/30 05:50:13 fang Exp
 */

#ifndef	__HAC_PARSER_INSTREF_H__
#define	__HAC_PARSER_INSTREF_H__

#include <iosfwd>
#include <vector>
#include "Object/ref/reference_enum.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/string_fwd.h"
#include "util/memory/excl_ptr.h"

#define	INVALID_PROCESS_INDEX			size_t(-1)

namespace util {
class directory_stack;
}	// end namespace util

namespace HAC {
namespace entity {
class module;
class meta_reference_union;
struct entry_collection;
struct dump_flags;
}
using util::memory::excl_ptr;

namespace parser {
using std::vector;
using std::string;
using entity::module;
using entity::bool_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::dump_flags;
class inst_ref_expr;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
struct invalid_index {
	enum { value = 0 };	// INVALID_NODE_INDEX
};

// specialization for process_tag
template <>
struct invalid_index<process_tag> {
	enum { value = INVALID_PROCESS_INDEX };
};

/**
	Convenience structure for typing-references, indexed gloabally.  
	This could be declared in a more globally visible header...
 */
template <class Tag>
struct typed_indexed_reference {
	size_t				index;

	// default value is invalid
	typed_indexed_reference() : index(invalid_index<Tag>::value) { }

	// avoid this when possible
	explicit
	typed_indexed_reference(const size_t i) : index(i) { }

	typed_indexed_reference(const string&, const module&);

	bool
	valid(void) const {
		return this->index != invalid_index<Tag>::value;
	}

};	// end class generic_reference

typedef	typed_indexed_reference<bool_tag>		bool_index;
typedef	typed_indexed_reference<channel_tag>		channel_index;
typedef	typed_indexed_reference<process_tag>		process_index;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
excl_ptr<inst_ref_expr>
parse_reference(const char*);

extern
entity::meta_reference_union
check_reference(const inst_ref_expr&, const module&);

extern
entity::meta_reference_union
parse_and_check_reference(const char*, const module&);

extern
bool_index
parse_node_to_index(const string&, const module&);

extern
process_index
parse_process_to_index(const string&, const module&);

extern
entity::global_indexed_reference
parse_global_reference(const string&, const module&);

extern
entity::global_indexed_reference
parse_global_reference(const entity::meta_reference_union&,
	const module&);

extern
int
parse_name_to_what(std::ostream&, const string&, const module&);

extern
int
parse_name_to_members(std::ostream&,
	const string&, const module&);

extern
int
parse_name_to_get_subinstances(
	const string&, const module&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subinstances(
	const entity::global_indexed_reference&, const module&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subnodes(
	const string&, const module&, 
	vector<size_t>&);
	// node_index_type

extern
int
parse_name_to_get_subnodes_local(
	const process_index&,
//	const string&,
	const module&, 
	vector<size_t>&);

extern
int
parse_name_to_get_ports(
	const process_index&, 
//	const string&,
	const module&, 
	vector<size_t>&, 
	const vector<bool>* pred = NULL);

extern
int
parse_name_to_aliases(std::ostream&, const string&, 
	const module&, const dump_flags&, const char* sep);

extern
void
complete_instance_names(const char*, const module&, 
	const util::directory_stack*, vector<string>&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_PARSER_INSTREF_H__

