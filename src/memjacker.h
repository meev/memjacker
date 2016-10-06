#ifndef _MEMJACKER_H
#define _MEMJACKER_H

#define HOOK_FIND_CALLING_CONVENTION		0x0

enum hook_architecture_e
{
	HOOK_ARCHITECTURE_I386 = 0,
	HOOK_ARCHITECTURE_X86_64,
};

enum hook_safety_level_e
{
	HOOK_SAFETY_CLEAN = 0,
};

#endif
