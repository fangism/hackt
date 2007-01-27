%module tortoise
%{
/* Includes the header in the wrapper code */
#include "tortoise.h"
%}

%ignore logo::tortoise_init_window(void);
 
/* Parse the header file to generate wrappers */
%include "tortoise.h"

