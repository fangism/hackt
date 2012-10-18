/**
	\file "Object/lang/PRS_macro_registry.hh"
	$Id: PRS_macro_registry.hh,v 1.7 2009/09/14 21:17:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

#include "util/STL/map_fwd.hh"
#include "Object/lang/directive_definition.hh"

namespace HAC {
namespace entity {
namespace PRS {
using std::string;
//=============================================================================
/**
	This is an actual registered entry of a macro definition.  
 */
template <class VisitorType>
class macro_visitor_entry : public directive_definition {
public:
	typedef	VisitorType			visitor_type;
	/**
		The primary execution function should take a sequence
		of node_id's and a global state_manager as arguments.  
		This is what is executed during cflat.  
	 */
	typedef void (main_type)(visitor_type&,
		const param_args_type&, const node_args_type&);
	typedef main_type*			main_ptr_type;
protected:
	main_ptr_type				_main;
public:
	macro_visitor_entry() : directive_definition(), _main(NULL) { }

	macro_visitor_entry(const string& k, const main_ptr_type m, 
		const check_num_args_ptr_type np, 
		const check_num_args_ptr_type nn, 
		const check_param_args_ptr_type p,
		const check_node_args_ptr_type c) :
		directive_definition(k, np, nn, p, c), 
		_main(m) { }

	operator bool () const { return this->_main; }

	void
	main(visitor_type&, const param_args_type&, 
		const node_args_type&) const;

	void
	main(visitor_type&, const node_args_type&) const;

	// everything else inherited from directive_definition

};	// end class macro_definition_entry

//=============================================================================
// TODO: factor these typedefs and symbols out to tool-dependent header

typedef	macro_visitor_entry<PRS::cflat_prs_printer>
						cflat_macro_definition_entry;

typedef	std::default_map<string, cflat_macro_definition_entry>::type
						cflat_macro_registry_type;

extern const cflat_macro_registry_type&		cflat_macro_registry;

//=============================================================================
/**
	Convenient macro for declaring macro classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_PRS_MACRO_CLASS(class_name, visitor)			\
struct class_name : public entity::PRS::macros::class_name {		\
	typedef	entity::PRS::macros::class_name		parent_type;	\
	typedef	class_name				this_type;	\
	typedef	visitor					visitor_type;	\
	typedef	entity::PRS::macros::node_args_type	node_args_type;	\
	typedef	entity::PRS::macros::param_args_type	param_args_type; \
public:									\
	static const char			name[];			\
	static void main(visitor_type&, const param_args_type&,		\
		const node_args_type&);					\
	static good_bool check_num_params(const size_t);		\
	static good_bool check_num_nodes(const size_t);			\
	static good_bool check_param_args(const param_args_type&);	\
	static good_bool check_node_args(const node_args_type&);	\
private:								\
	static const size_t			id;			\
};	// end class class_name

/**
	Convenient macro for declaring and initializing macro class.
 */
#define	DEFINE_PRS_MACRO_CLASS(class_name, macro_name, registrar)	\
const char class_name::name[] = macro_name;				\
const size_t class_name::id = registrar<class_name>();			\
good_bool								\
class_name::check_num_params(const size_t s) {				\
	return parent_type::__check_num_params(name, s);		\
}									\
good_bool								\
class_name::check_num_nodes(const size_t s) {				\
	return parent_type::__check_num_nodes(name, s);			\
}									\
good_bool								\
class_name::check_param_args(const param_args_type& p) {		\
	return parent_type::__check_param_args(name, p);		\
}									\
good_bool								\
class_name::check_node_args(const node_args_type& n) {			\
	return parent_type::__check_node_args(name, n);			\
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

