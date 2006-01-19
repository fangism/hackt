/**
	\file "AST/parse_context.tcc"
	Template methods for context object passed around during 
	type-checking, and object construction.  
	This file was "Object/art_context.tcc" in a previous life.
 	$Id: parse_context.tcc,v 1.4.2.1 2006/01/19 07:42:39 fang Exp $
 */

#ifndef	__AST_PARSE_CONTEXT_TCC__
#define	__AST_PARSE_CONTEXT_TCC__

#include <cassert>
#include <exception>
#include <iostream>

#include "AST/parse_context.h"
#include "AST/token_string.h"
#include "AST/identifier.h"
#include "Object/common/namespace.h"

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	THIS NEEDS SERIOUS RE-WORK.  
	Registers a definition's signature.  
	\param D the specific definition type to open.  
	\param pname the process signature, which contains and identifier, 
		optional template signature, and port signature.  
	Details: checks to see if prototype was already declared.  
	If already declared, then this re-declaration MUST be indentical, 
	else report error of mismatched re-declaration.  
	If not already declared, create an entry...
 */
template <class D>
void
context::open_definition(const token_identifier& pname) {
	// concept check code
	INVARIANT(!static_cast<const definition_base*>(NULL));
	const never_ptr<D>
		def(current_namespace->lookup_member_with_modify(pname)
				.template is_a<D>());
	if (def) {
		if (def->is_defined()) {
			cerr << pname << " is already defined!  attempted "
				"redefinition at " << where(pname) << endl;
			THROW_EXIT;
		}
		INVARIANT(!current_open_definition);	// sanity check
		current_open_definition = def;
		sequential_scope_stack.push(
			def.template as_a<sequential_scope>());
		def->mark_defined();
		indent++;
	} else {
		// no real reason why this should ever fail...
		type_error_count++;
		cerr << where(pname) << endl;
		THROW_EXIT;			// temporary
		// return NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a process definition in the context.  
	Just sets current_open_definition to NULL.  
 */
template <class D>
void
context::close_definition(void) {
	// sanity check
	current_open_definition.template must_be_a<D>();
	sequential_scope_stack.pop();
	close_current_definition();
}

//=============================================================================
template <class DefType>
context::definition_frame<DefType>::definition_frame(context& c, 
		const token_identifier& i) : _context(c) {
	_context.open_definition<DefType>(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class DefType>
context::definition_frame<DefType>::~definition_frame() {
	_context.close_definition<DefType>();
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __AST_PARSE_CONTEXT_TCC__

