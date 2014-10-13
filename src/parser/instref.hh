/**
	\file "parser/instref.hh"
	Interface to reference-parsing.
	$Id: instref.hh,v 1.15 2011/03/12 07:46:19 fang Exp $
	This file originated from "sim/prsim/Reference.h"
	Id: Reference.h,v 1.5 2006/07/30 05:50:13 fang Exp
 */

#ifndef	__HAC_PARSER_INSTREF_H__
#define	__HAC_PARSER_INSTREF_H__

#include <iosfwd>
#include <utility>		// for std::pair
#include <vector>
#include <set>			// for set<string>
#include "Object/ref/reference_enum.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "util/string_fwd.hh"
#include "util/memory/count_ptr.hh"

#define	INVALID_PROCESS_INDEX			size_t(-1)

/**
	Define to 1 to use the same functions for global and local
	lookups of references, given footprint contexts.
	Goal: 1
	Status: perm'd
 */
// #define	REUSE_PARSE_GLOBAL_FOR_LOCAL		1
#define parse_local_reference		parse_global_reference
#define parse_local_references		parse_global_references

namespace util {
class directory_stack;
}	// end namespace util

namespace HAC {
namespace entity {
class footprint;
class meta_reference_union;
class process_definition;
struct entry_collection;
struct dump_flags;
}
using util::memory::count_ptr;

namespace parser {
using std::ostream;
using std::vector;
using std::string;
using entity::process_definition;
using entity::footprint;
using entity::bool_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::dump_flags;
using entity::meta_reference_union;
using entity::global_reference_array_type;
using entity::global_indexed_reference;

class inst_ref_expr;

typedef	std::set<string>		string_set;

// from "AST/expr_base.hh: inst_ref_expr::reference_array_type
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

	typed_indexed_reference(const string&, const footprint&);

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

	typed_indexed_references(const string&, const footprint&);

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
check_reference(const inst_ref_expr&, const process_definition&);

extern
int
expand_reference(const count_ptr<const inst_ref_expr>&, 
	reference_array_type&);

extern
meta_reference_union
parse_and_check_reference(const char*, const process_definition&);

extern
bool
expand_global_references(const string&, const footprint&, 
	expanded_global_references_type&);

extern
bool
parse_nodes_to_indices(const string&, const footprint&, vector<size_t>&);

extern
bool_index
parse_node_to_index(const string&, const footprint&);

extern
bool
parse_processes_to_indices(const string&, const footprint&, vector<size_t>&);

extern
process_index
parse_process_to_index(const string&, const footprint&);

extern
global_indexed_reference
parse_global_reference(const string&, const footprint&, ostream*);

extern
global_indexed_reference
parse_global_reference(const string&, const footprint&);

inline
global_indexed_reference
parse_global_reference_default(const string& s, const footprint& f) {
	return parse_global_reference(s, f);
}

extern
global_indexed_reference
parse_global_reference(const meta_reference_union&, const footprint&);

extern
int
parse_global_references(const string&, const footprint&,
	global_reference_array_type&);

extern
int
parse_global_references(const meta_reference_union&,
	const footprint&, global_reference_array_type&);

extern
int
parse_name_to_what(ostream&, const string&, const footprint&);

extern
int
parse_name_to_members(ostream&,
	const string&, const footprint&);

extern
int
parse_name_to_get_subinstances(
	const string&, const footprint&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subinstances(
	const global_indexed_reference&, const footprint&, 
	entity::entry_collection&);

extern
int
parse_name_to_get_subnodes(
	const string&, const footprint&, 
	vector<size_t>&);
	// node_index_type

extern
int
parse_name_to_get_subnodes_local(
	const process_index&,
//	const string&,
	const footprint&, 
	vector<size_t>&);

extern
int
parse_name_to_get_ports(
	const process_index&, 
//	const string&,
	const footprint&, 
	vector<size_t>&, 
	const vector<bool>* pred = NULL);

extern
int
parse_name_to_aliases(string_set&, const string&, 
	const footprint&, const dump_flags&);

extern
int
parse_name_to_aliases(ostream&, const string&, 
	const footprint&, const dump_flags&, const char* sep);

extern
void
complete_instance_names(const char*, const footprint&, 
	const util::directory_stack*, vector<string>&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_PARSER_INSTREF_H__

