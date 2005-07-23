/**
	\file "AST/parse_context.tcc"
	Template methods for context object passed around during 
	type-checking, and object construction.  
	This file was "Object/art_context.tcc" in a previous life.
 	$Id: parse_context.tcc,v 1.1.2.2 2005/07/23 01:05:37 fang Exp $
 */

#ifndef	__AST_PARSE_CONTEXT_TCC__
#define	__AST_PARSE_CONTEXT_TCC__

#include <cassert>
#include <exception>
#include <iostream>

#include "AST/parse_context.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_identifier.h"
#include "Object/common/namespace.h"
// #include "Object/art_object_instance_management_base.h"

//=============================================================================
namespace ART {
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
	const D* null = NULL;
	INVARIANT(!static_cast<const definition_base*>(null));
	const never_ptr<D>
		def(current_namespace->lookup_object_here_with_modify(pname)
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	INSTANTIATE_CONTEXT_OPEN_CLOSE_DEFINITION(T)			\
template void context::open_definition<T >(const token_identifier&);	\
template void context::close_definition<T >();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
never_ptr<const definition_base>
context::lookup_definition(const token_identifier& id) const {
	INVARIANT(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
	\param id the qualified name of the definition sought.  
	\return const pointer to the identified definition, if found.  
 */
never_ptr<const definition_base>
context::lookup_definition(const qualified_id& id) const {
	INVARIANT(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param id the name of the instance sought.  
	\return const pointer to the named instance sought, if found.  
 */
never_ptr<const instance_collection_base>
context::lookup_instance(const token_identifier& id) const {
	INVARIANT(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param id the qualified name of the instance sought.  
	\return const pointer to the named instance sought, if found.  
 */
never_ptr<const instance_collection_base>
context::lookup_instance(const qualified_id& id) const {
	INVARIANT(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const instance_collection_base>();
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __AST_PARSE_CONTEXT_TCC__

