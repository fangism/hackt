/**
	\file "main/ipple.cc"
	Main program for Fang's Interactive Physics-driven Placement Engine
	$Id: ipple.cc,v 1.1.2.1 2011/04/11 18:38:44 fang Exp $
 */

#include <iostream>
#include "PR/placement_engine.h"
#include "PR/pr-command.h"

using PR::CommandRegistry;

int
main(const int argc, char* argv[]) {
	// parse options
	PR::placement_engine P(PR::PR_DIMENSIONS);	// fixed: 3D
	CommandRegistry::prompt = "fipple> ";
//	const CommandRegistry::readline_init __rl__;
	const bool interactive = true;	// temporary
	const int ret = CommandRegistry::interpret(P, std::cin, interactive);
	if (ret) {
		return 1;
	}
	return 0;
}

