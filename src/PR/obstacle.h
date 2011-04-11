/**
	\file "PR/obstacle.h"
	$Id: obstacle.h,v 1.1.2.1 2011/04/11 18:38:37 fang Exp $
 */

#ifndef	__HAC_PR_OBSTACLE_H__
#define	__HAC_PR_OBSTACLE_H__

#include "PR/numerics.h"

namespace PR {

/**
	Obstacles can only be rectangular for now.
 */
struct obstacle {
	position_type			corner[2];
};	// end struct obstacle

}	// end namespace PR

#endif	// __HAC_PR_OBSTACLE_H__

