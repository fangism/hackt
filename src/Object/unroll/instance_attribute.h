/**
	\file "Object/unroll/instance_attribute.h"
	Definition of generic attribute statements.  
	$Id: instance_attribute.h,v 1.2 2008/10/07 03:22:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__
#define	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__

#include <iosfwd>
#include <map>
#include "Object/unroll/instance_management_base.h"
#include "Object/lang/generic_attribute.h"
#include "Object/lang/attribute_visitor_entry.h"
#include "Object/ref/references_fwd.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
template <class> class instance_alias_info;
using util::memory::count_ptr;
using util::persistent_object_manager;
using std::istream;

//=============================================================================
#define	INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE		template <class Tag>
#define	INSTANCE_ATTRIBUTE_CLASS		instance_attribute<Tag>

//=============================================================================
/**
	Meta-type specific instance attribute statement.
 */
template <class Tag>
class instance_attribute : public instance_management_base {
	typedef	instance_attribute<Tag>			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	/**
		Should aggregate references be allowed? member references?
		Yes, eventually.  
	 */
	typedef	meta_instance_reference<Tag>		reference_type;
//	typedef	simple_meta_instance_reference<Tag>	reference_type;
	typedef	count_ptr<const reference_type>	reference_ptr_type;
	typedef	instance_alias_info<Tag>		alias_type;

	typedef	attribute_visitor_entry<alias_type>	attribute_funcs_type;
	typedef	std::map<std::string, attribute_funcs_type>
						attribute_registry_type;
	template <class S>
	static	size_t	register_attribute_functions(void);
	static	bool	attribute_exists(const string&);
private:
	static	attribute_registry_type			attribute_registry;
	/**
		The instance(s) to which relaxed parameters should be 
		attached.  
	 */
	reference_ptr_type				ref;
	/**
		The relaxed template parameters to associate with 
		each referenced instance.  
	 */
	generic_attribute_list_type			attrs;
public:
	instance_attribute();
	instance_attribute(const reference_ptr_type&, 
		const generic_attribute_list_type&);
	// default copy-ctor
	~instance_attribute();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class instance_attribute


//=============================================================================
/**
	Generic macro for declaring instance attribute classes.  
	Ripped from "Object/lang/PRS_attribute_registry.h"
	The base classes are declared in ...
	TODO: These classes should have hidden visibility.  
	TODO: could also push name[] into the base class, but would we be 
		creating an initialization order dependence?
 */
#define DECLARE_INSTANCE_ATTRIBUTE_CLASS(class_name, base_class, visitor) \
struct class_name : public base_class {					\
	typedef	base_class				parent_type;	\
	typedef	visitor					visitor_type;	\
	typedef	entity::attribute_visitor_entry<visitor_type>		\
					definition_entry_type;		\
	typedef	definition_entry_type::values_type	values_type;	\
	typedef	values_type::value_type			value_type;	\
public:									\
	static const char				name[];		\
	static void main(visitor_type&, const values_type&);		\
	static good_bool check_vals(const values_type&);		\
private:								\
	static const size_t				id;		\
};	// end class class_name

/**
	\param registrar is the name of the template function used to register
		the class for a specific tool.  
 */
#define DEFINE_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name, registrar) \
const char class_name::name[] = att_name;				\
good_bool								\
class_name::check_vals(const values_type& v) {				\
	return parent_type::__check_vals(name, v);			\
}									\
const size_t class_name::id = registrar<class_name>();

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__

