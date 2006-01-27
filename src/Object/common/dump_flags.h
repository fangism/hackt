/**
	\file "Object/common/dump_flags.h"
	Dump attributes class.  
	$Id: dump_flags.h,v 1.5.4.1 2006/01/27 23:04:23 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__
#define	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__

namespace HAC {
namespace entity {

/**
	Common dump flags used to tweak output formatting and style.  
 */
struct dump_flags {
	/**
		Whether or not to print the scope to which a particuluar
		hierarchical instance belongs.  
		This SHOULD print namespace owners, but NOT definition owners.  
		Default value: true.
		Will want to be false when piecing together hierarchical
		canonical names, see global_entry::dump_canonical_name().  
		See use in instance_collection_base::dump_hierarchical_name.
	 */
	bool	show_definition_owner;
	/**
		Whether or not to show namespace owners.  
	 */
	bool	show_namespace_owner;
	/**
		Whether or not the global namespace should print 
		its leading "::".  
	 */
	bool	show_leading_scope;

	dump_flags();
private:
	explicit
	dump_flags(const bool _show_def_owner,
		const bool _show_ns_owner, 
		const bool _show_leading);
public:
	~dump_flags() { }

	/**
		Static const global default values.  
		(Shouldn't be used before the end of global initialization.)
		None of these should be referenced until main begins.  
	 */
	static const dump_flags			default_value;
	/**
		Another common value for printing.  
	 */
	static const dump_flags			no_definition_owner;
	/**
		Show owner is true, but show_leading_scope is false.
	 */
	static const dump_flags			no_leading_scope;
};	// end class dump_flags

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_DUMP_FLAGS_H__

