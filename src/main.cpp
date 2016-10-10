#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/mman.h>

#include "memjacker.hpp"

int g_hk_func = 0;
int g_original_func = 0;

__attribute__((naked)) void detour()
{
	asm("call g_hk_func");

	// Figure something out to make dynamic tail operations
	asm("push ebp");
	asm("mov ebp, esp");
	asm("sub esp, 0x8");
	asm("jmp g_original_func + 5"); 
}

class memjacker
{
public:
	memjacker(int new_func) { hk_func = new_func; };

	/*
	 * This will be the address to our detour/hook function
	 * You need to take care of calling convention yourself!
	*/
	int hk_func;

	/*
	 * This will be the address of the original function
	 * Mainly used in detours, and also returned upon hooks
	 * so they can be called manually with a typedef. 
	*/
	int original_func;

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
	int setup_hook_detour(void *original_func_addr, int jmp_offset)
	{
		// No access to write  here, get some..
		//mmap(original_func_addr, 0xF, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_PRIVATE, 1, 0);
		int page_size = getpagesize();
		void *aligned_addr = original_func_addr - ((unsigned int)original_func_addr % page_size);
		if(mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
		{
			printf("Error mprotecting your shit..\n");
		}
		else
		{
			for(unsigned char i = 0; i < 10; i++)
			{
				unsigned char *instruction = (unsigned char*)original_func_addr+i;
				printf("Instruction %08x: %02x\n", instruction, *instruction);
			}
			void *detour_addr = (void*)detour;
			char byte1 = ((unsigned char*)(&detour_addr))[0];
			char byte2 = ((unsigned char*)(&detour_addr))[1];
			char byte3 = ((unsigned char*)(&detour_addr))[2];
			char byte4 = ((unsigned char*)(&detour_addr))[3];

			printf("%x, %x, %x, %x\n", byte1, byte2, byte3, byte4);
			char jmp_code[] = { 0xEA, byte1, byte2, byte3, byte4 };
			
			memcpy(original_func_addr, jmp_code, 5);
			for(unsigned char i = 0; i < 10; i++)
			{
				unsigned char *instruction = (unsigned char*)original_func_addr+i;
				printf("Modified instructions %08x: %02x\n", instruction, *instruction);
			}
		}

		mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
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

void detoured_function_1()
{
	printf("Call inside detoured function 1\n");
}

void function_1()
{
	printf("Call inside function 1\n");
}

int main(int argc, char **argv, char **envp)
{
	void *function_1_addr = (void*)function_1;
	void *detoured_function_1_addr = (void*)detoured_function_1;

	g_original_func = (int)function_1_addr;
	g_hk_func = (int)detoured_function_1_addr;

	printf("function_1: %X\n", function_1_addr);
	printf("detoured_function_1: %X\n", detoured_function_1_addr);
	printf("\n");

	memjacker *jacked = new memjacker((int)detoured_function_1_addr);
	jacked->setup_hook_detour(function_1_addr, 0);
	function_1();
	return 0;
}
