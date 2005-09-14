/**
	\file "Object/common/dump_flags.h"
	Dump attributes class.  
	$Id: dump_flags.h,v 1.2 2005/09/14 15:30:27 fang Exp $
 */

#ifndef	__OBJECT_COMMON_DUMP_FLAGS_H__
#define	__OBJECT_COMMON_DUMP_FLAGS_H__

namespace ART {
namespace entity {

/**
	Common dump flags used to tweak output formatting and style.  
 */
struct dump_flags {
	/**
		Whether or not to print the scope to which a particuluar
		hierarchical instance belongs.  
		Default value: true.
		Will want to be false when piecing together hierarchical
		canonical names, see global_entry::dump_canonical_name().  
		See use in instance_collection_base::dump_hierarchical_name.
	 */
	bool	show_definition_owner;

	dump_flags() : show_definition_owner(true) { }

	~dump_flags() { }

	/**
		Static const global default values.  
		(Shouldn't be used before the end of global initialization.)
	 */
	static const dump_flags			default_value;
};	// end class dump_flags

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_DUMP_FLAGS_H__

