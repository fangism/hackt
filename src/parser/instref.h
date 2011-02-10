/**
	\file "parser/instref.h"
	Interface to reference-parsing.
	$Id: instref.h,v 1.14 2011/02/10 22:32:40 fang Exp $
	This file originated from "sim/prsim/Reference.h"
	Id: Reference.h,v 1.5 2006/07/30 05:50:13 fang Exp
 */

#ifndef	__HAC_PARSER_INSTREF_H__
#define	__HAC_PARSER_INSTREF_H__

#include <iosfwd>
#include <utility>		// for std::pair
#include <vector>
#include "Object/ref/reference_enum.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/string_fwd.h"
#include "util/memory/count_ptr.h"

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
using util::memory::count_ptr;

namespace parser {
using std::vector;
using std::string;
using entity::module;
using entity::bool_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::dump_flags;
using entity::meta_reference_union;
using entity::global_reference_array_type;
using entity::global_indexed_reference;

class inst_ref_expr;

// from "AST/expr_base.h: inst_ref_expr::reference_array_type
typedef count_ptr<const inst_ref_expr>	AST_reference_ptr;
typedef	vector<AST_reference_ptr>	reference_array_type;

typedef std::pair<AST_reference_ptr, global_indexed_reference>
					expanded_global_reference;
typedef vector<expanded_global_reference>
					expanded_global_references_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// helper types for organizing global indices

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

/**
	For looking up collections.  
 */
template <class Tag>
struct typed_indexed_references {
	vector<size_t>					indices;

	// default value is invalid
	typed_indexed_references() : indices() { }

	typed_indexed_references(const string&, const module&);

	bool
	valid(void) const {
		return !indices.empty();
	}

};	// end class generic_reference

typedef	typed_indexed_references<bool_tag>		bool_indices;
typedef	typed_indexed_references<channel_tag>		channel_indices;
typedef	typed_indexed_references<process_tag>		process_indices;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
count_ptr<inst_ref_expr>
parse_reference(const char*);

extern
meta_reference_union
check_reference(const inst_ref_expr&, const module&);

extern
int
expand_reference(const count_ptr<const inst_ref_expr>&, 
	reference_array_type&);

extern
meta_reference_union
parse_and_check_reference(const char*, const module&);

extern
bool
expand_global_references(const string&, const module&, 
	expanded_global_references_type&);

extern
bool
parse_nodes_to_indices(const string&, const module&, vector<size_t>&);

extern
bool_index
parse_node_to_index(const string&, const module&);

extern
bool
parse_processes_to_indices(const string&, const module&, vector<size_t>&);

extern
process_index
parse_process_to_index(const string&, const module&);

extern
global_indexed_reference
parse_global_reference(const string&, const module&);

extern
global_indexed_reference
parse_global_reference(const meta_reference_union&,
	const module&);

extern
int
parse_global_references(const string&, const module&,
	global_reference_array_type&);

extern
int
parse_global_references(const meta_reference_union&,
	const module&, global_reference_array_type&);

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
	const global_indexed_reference&, const module&, 
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

