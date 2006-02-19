/**
	\file "sim/prsim/Reference.cc"
	$Id: Reference.cc,v 1.2.16.1 2006/02/19 03:53:20 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstdio>
#include <string>
#include "sim/prsim/Reference.h"
#include "parser/instref-parse-real.h"	// for YYSTYPE
#include "AST/expr.h"
#include "AST/parse_context.h"
#include "Object/module.h"
#include "Object/object_fwd.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/ref/meta_reference_union.h"
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
using entity::state_manager;
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
			b(r.inst_ref().is_a<const bool_ref_type>());
		if (!b) {
			// later: write another procedure
			// to print *collections* of bools
			// by prefix matching.
			cerr << "Error: " << n << " does not reference "
				"a bool (node)." << endl;
			return INVALID_NODE_INDEX;
		}
		// reminder: this is a packed_array_generic
		// this code uses the allocation information from the 
		// alloc phase to find the canonical ID number.  
		const state_manager& sm(m.get_state_manager());
		const node_index_type ret =
			b->lookup_globally_allocated_index(sm);
#if 0
		cerr << "index = " << ret << endl;
#endif
		return ret;
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

