/**
	\file "sim/prsim/Reference.cc"
	$Id: Reference.cc,v 1.1.2.4 2006/01/19 00:16:16 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstdio>
#include <string>
#include "sim/prsim/Reference.h"
#include "parser/instref-parse-real.h"	// for YYSTYPE
#include "AST/expr.h"
#include "AST/parse_context.h"
// #include "Object/module.h"
#include "Object/object_fwd.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "util/stacktrace.h"
#include "util/libc.h"			// for tmpfile, rewind,...
#include "util/memory/excl_ptr.h"
#include "util/memory/deallocation_policy.h"
#include "util/packed_array.h"		// for alias_collection_type
#include "util/ring_node.h"
#include "util/wtf.h"

extern	int instref_parse(void*, YYSTYPE&, FILE*);

namespace HAC {
namespace SIM {
namespace PRSIM {
using parser::context;
using entity::unroll_context;
using entity::module;
using entity::simple_bool_meta_instance_reference;
using entity::substructure_alias;
using std::string;
using parser::inst_ref_expr;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::FILE_tag;
#include "util/using_ostream.h"
//=============================================================================

/**
	TODO: figure out a way to parse a string without
		going through a friggin' temp file.  
		This is really sad. 
	TODO: be able to cache already checked references with a hash.  
	\param n the string that names the instance reference
	\param m the compiled module with the top-level namespace
		and allocated state_manager.  
 */
node_index_type
parse_node_to_index(const string& n, const module& m) {
	STACKTRACE_VERBOSE;
	typedef	excl_ptr<FILE, FILE_tag>	FILE_ptr;
	const FILE_ptr temp(tmpfile());	// will automatically close on return
	if (!temp) {
		// Woe is me!
		cerr << "Failed to create temporary file-buffer!" << endl;
		THROW_EXIT;
	}
	// TODO: look into setting the file buffer (setvbuf, setlinebuf...)
	// fputs returns 0 on success, anything else on error
	if (fputs(n.c_str(), &*temp)) {
		cerr << "Error writing string to temporary file." << endl;
		THROW_EXIT;
	} else {
		typedef	excl_ptr<inst_ref_expr>		lval_ptr_type;
		// need newline or some whitespace to prevent
		// lexer from premature EOF-ing.
		fputc('\n', &*temp);
		// the flush doesn't seem necessary from experiments
		// hopefully this will save from frequent writes to the FS
		// fflush(&*temp);
		rewind(&*temp);		// same as fseek(temp, 0, SEEK_SET);
		YYSTYPE lval;
		try {
			instref_parse(NULL, lval, &*temp);
		} catch (...) {
			cerr << "Error parsing instance name: " << n << endl;
			return INVALID_NODE_INDEX;
		}
		// cerr << "parsed node name successfully... " << endl;
		// here is our mini-parse-tree:
		const lval_ptr_type ref_tree(lval._inst_ref_expr);
	/***
		NOTE: parse::context can only accept a modifiable module&
			as a ctor argument, but we REQUIRE that the 
			module we pass be unscathed (const).
			To help enforce constness in this case, 
			we restrict the constructed context to be const
			which promises not to modify its internal 
			reference to the module.  Thus from this point
			we are restricted to using the context const-ly.  
	***/
		/// this is a reference-counted meta_instance_reference_base
		typedef	inst_ref_expr::meta_return_type
						checked_ref_type;
	/***
		And now for a slice of compiler pie:
		Oh by the way, once we enable non-const expressions
		in indices, this will automatically support 
		meta-expression evaluation using the values present in
		the module.  Yeah, baby.  
		Passing "true" as the 2nd arg says we want all names 
		publicly visible, see AST::parser::context::view_all_publicly.
	***/
		const context c(m, true);
		checked_ref_type r;
		try {
			// NOTE: this checks for PUBLIC members only
			// but we should allow PRIVATE references too!
			// perhaps flag through context?
			r = ref_tree->check_meta_reference(c);
		} catch (...) {
			// temporary have shitty error-handling...
			// already have type-check error message
			return INVALID_NODE_INDEX;
		}
		if (!r) {
			// don't expect this message to ever happen...
			cerr << "Some other error type-checking..." << endl;
			return INVALID_NODE_INDEX;
		}
		// cerr << "Woo-hoo! we found it!" << endl;
		typedef	simple_bool_meta_instance_reference
					bool_ref_type;
		const count_ptr<const bool_ref_type>
			b(r.is_a<const bool_ref_type>());
		if (!b) {
			// later: write another procedure
			// to print *collections* of bools
			// by prefix matching.
			cerr << "Error: " << n << " does not reference "
				"a bool (node)." << endl;
			return INVALID_NODE_INDEX;
		}
		// reminder: this is a packed_array_generic
		typedef	bool_ref_type::alias_collection_type
				alias_collection_type;
		alias_collection_type jar;	// a container of bools
		const unroll_context uc;
		if (b->unroll_references(uc, jar).bad) {
			cerr << "Error unrolling bool reference." << endl;
			return INVALID_NODE_INDEX;
		}
		if (alias_collection_type::sizes_product(jar.size()) > 1) {
			cerr << "Error: bool reference is underspecified, "
				"referring to a collection instead of a "
				"single instance." << endl;
			return INVALID_NODE_INDEX;
		}
		typedef alias_collection_type::value_type	alias_ptr_type;
		typedef	alias_ptr_type::element_type		bool_alias_type;
		// is a bool_instance_alias_info, in case you lost track...
		const alias_ptr_type ap(jar.front());
		NEVER_NULL(ap);	// must've succeeded, else would've errored out
		const bool_alias_type& a(*ap);
		// util::wtf_is(a);
		// OK, now how do we go from point in the alias-name
		// hierarchy to canonical index?
		// query the state_manager?
		// see how instance_alias_info::cflat_aliases is implemented
		// using a cflat_aliases_arg_type visitor.
		const node_index_type ret = a.instance_index;
#if 0
		a.dump_hierarchical_name(cerr << "Hi, my name is ") << endl;
		cerr << "ID = " << ret << endl;
#endif
		INVARIANT(ret);
		// cerr << "Fang finish the lookup!" << endl;
		return ret;
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

