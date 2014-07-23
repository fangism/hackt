/**
	\file "sim/chpsim/Command.hh"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: Command.hh,v 1.3 2007/02/14 04:57:25 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_COMMAND_HH__
#define	__HAC_SIM_CHPSIM_COMMAND_HH__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "sim/command_base.hh"
#include "sim/command_category.hh"
#include "sim/command_registry.hh"

namespace HAC {
namespace SIM {
template <class> class Help;

namespace CHPSIM {
class State;
using std::string;
using util::string_list;
using std::string;
using std::ostream;

//=============================================================================
typedef	SIM::Command<State>		Command;
typedef	command_category<Command>	CommandCategory;
typedef	command_registry<Command>	CommandRegistry;

//=============================================================================

/**
	Declares a command class.  
 */
#define	DECLARE_CHPSIM_COMMAND_CLASS(class_name)			\
struct class_name {                                                     \
public:                                                                 \
	static const char		name[];				\
	static const char		brief[];			\
	static CommandCategory&		(*category)(void);		\
	static int	main(State&, const string_list&);		\
	static void	usage(ostream&);				\
private:								\
	static const size_t		receipt_id;			\
};

/**
	Helpful class.  
 */
typedef	Help<State>			Help;

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_COMMAND_HH__

