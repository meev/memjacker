#include <stdio.h>
#include <stdlib.h>
#include "memjacker.hpp"

class memjacker
{
public:
	memjacker(int new_func) { hk_func = new_func; };

	/*
	 *  This will be the pointer to our detour/hook
	 * You need to take care of calling convention.
	*/
	int hk_func;

	/*
	 * This will be the address of original_func_addr + jmp_offset + 5
	*/
	int out;

	/*
	 * setup_hook_by_call(int *call_addr);
	 * Modifies memory where a call operation is done, to call
	 * a custom function.
	 * 
	 * int *call_addr: Pointer to call operation in memory
	*/
	int setup_hook_by_call(int *call_addr) {};

	/*
	 * setup_hook_detour(int *original_func_addr, int jmp_offset);
	 * Modifies memory to detour a function into a custom function 
	 * while forwarding arguments to the function.
	 * 
	 * int *original_func_addr: Pointer to the very start of a function
	 * int jmp_offset: Offset into function where we do the detour
	*/
	int setup_hook_detour(int *original_func_addr, int jmp_offset) {};
	
	__attribute__((naked)) void detour()
	{
		asm("call hk_func");

		// Figure something out to make dynamic tail operations
		asm("push ebp");
		asm("mov ebp, esp");
		asm("jmp out");
	}

	/*
	 * setup_hook_vtable(int *vtable, int index);
	 * Change pointer in a vtable to a custom functions pointer
	 * 
	 * int *vtable: Pointer to vtable entry
	 * int index: Index in vtable
	*/
	int setup_hook_vtable(int *vtable, int index) {};
};

void function_1()
{
	printf("Inside function 1\n");
}

int main(int argc, char **argv, char **envp)
{
	printf("function_1: %X\n", function_1);
	memjacker *jacked = new memjacker((int)function_1);
	return 0;
}
