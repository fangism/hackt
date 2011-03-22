/**
	\file "Object/unroll/direction_declaration.h"
	Definition of generic attribute statements.  
	$Id: direction_declaration.h,v 1.1.2.2 2011/03/22 00:51:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_H__
#define	__HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_H__

#include <iosfwd>
#include <map>
#include "Object/unroll/instance_management_base.h"
#include "Object/ref/references_fwd.h"
#include "Object/type/channel_direction_enum.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
template <class> class instance_alias_info;
using util::memory::count_ptr;
using util::persistent_object_manager;
using std::istream;

//=============================================================================
#define	DIRECTION_DECLARATION_TEMPLATE_SIGNATURE		template <class Tag>
#define	DIRECTION_DECLARATION_CLASS		direction_declaration<Tag>

//=============================================================================
/**
	Meta-type specific direction declaration statement.
 */
template <class Tag>
class direction_declaration : public instance_management_base {
	typedef	direction_declaration<Tag>			this_type;
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

private:
	/**
		The instance(s) to which relaxed parameters should be 
		attached.  
	 */
	reference_ptr_type				ref;
	/**
		The relaxed template parameters to associate with 
		each referenced instance.  
	 */
	direction_type					dir;
public:
	direction_declaration();
	direction_declaration(const reference_ptr_type&, 
		const direction_type);
	// default copy-ctor
	~direction_declaration();

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

};	// end class direction_declaration


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_H__

