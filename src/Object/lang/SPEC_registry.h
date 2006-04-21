/**
	\file "Object/lang/SPEC_registry.h"
	$Id: SPEC_registry.h,v 1.3.18.3 2006/04/21 20:10:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__
#define	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__

#include "util/qmap.h"
#include "Object/lang/directive_definition.h"

namespace HAC {
namespace entity {
class state_manager;
namespace PRS {
class cflat_prs_printer;
}
namespace SPEC {
using PRS::cflat_prs_printer;
using std::string;
using util::good_bool;
//=============================================================================
/**
	This is an actual registered entry of a spec definition.  
 */
template <class VisitorType>
class spec_visitor_entry : public directive_definition {
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
	spec_visitor_entry() : directive_definition(), _main(NULL) { }

	spec_visitor_entry(const string& k,
		const main_ptr_type m, 
		const check_num_args_ptr_type np = NULL,
		const check_num_args_ptr_type nn = NULL,
		const check_param_args_ptr_type p = NULL,
		const check_node_args_ptr_type n = NULL) :
		directive_definition(k, np, nn, p, n), 
		_main(m) { }

	operator bool () const { return this->_main; }

	void
	main(visitor_type&, const param_args_type&,
		const node_args_type&) const;

	void
	main(visitor_type&, const node_args_type&) const;


	// everything else inherited from directive_definition

};	// end class spec_definition_entry

//=============================================================================
// TODO: rename for cflat pass

typedef	spec_visitor_entry<cflat_prs_printer>	cflat_spec_definition_entry;

typedef	util::qmap<string, cflat_spec_definition_entry>
						cflat_spec_registry_type;

extern const cflat_spec_registry_type		cflat_spec_registry;

//=============================================================================
/**
	Convenient spec for declaring spec classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_SPEC_DIRECTIVE_CLASS(class_name, visitor)		\
struct class_name : public entity::SPEC::directives::class_name {	\
	typedef	entity::SPEC::directives::class_name	parent_type;	\
	typedef	class_name				this_type;	\
	typedef	visitor					visitor_type;	\
	typedef	entity::SPEC::directives::node_args_type		\
							node_args_type;	\
	typedef	entity::SPEC::directives::param_args_type		\
							param_args_type;\
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
};									\

/**
	\param registrar the class registration function for initialization.  
 */
#define	DEFINE_SPEC_DIRECTIVE_CLASS(class_name, spec_name, registrar)	\
const char class_name::name[] = spec_name;				\
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

//-----------------------------------------------------------------------------
//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_REGISTRY_H__

