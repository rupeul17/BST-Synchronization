/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student : 32131727 윤지원, 32121826 서영준
*
*   lab2_bonus.c :
*       - spin lock aessembly code.
*
*   Implement spin lock in assembly level. 
*/


#include <sys/types.h>
#include <pthread.h>

#include "lab2_sync_types.h"

#define LOCK_PREFIX "lock\n\t"

/* 
 * Assembly level implementation example.  
 *
 * atomic_add
 *  @param int *value   : variable which will be added by add_val
 *  @param int add_val  : variable which will be added to *value
 */
void atomic_add( int * value, int add_val) {

    asm volatile(
            LOCK_PREFIX
            "addl %1, %0      \n\t"
            : "+m"(*value)
            : "ir"(add_val)
            : /*no clobber-list*/
            );
}

void atomic_sub( int * value, int dec_val) {

    asm volatile(
            LOCK_PREFIX
            "subl %1, %0      \n\t"
            : "+m"(*value)
            : "ir"(dec_val)
            : /*no clobber-list*/
            );
}

void atomic_inc( int * value) {

    asm volatile(
            LOCK_PREFIX
            "incl %0        \n\t"
            : "+m"(*value)
            : /* no input */
            : /*no clobber-list*/
            );
}

void atomic_dec( int * value) {

    asm volatile(
            LOCK_PREFIX
            "decl %0        \n\t"
            : "+m"(*value)
            : /* no input */
            : /*no clobber-list*/
            );
}

/* 
 * TODO - lab2 bonus
 *  Implement a function which acts like spin_lock.
 *
 *  @param int *lock        : lock variable's current status. 
 */

// spin lock을 c 파일 내에서 asm volatile 함수를 사용하여 assembly어로 구현한 함수
/*
함수가 인자로 lock 변수를 받으면 명령어 xchg(Exchange data)를 사용하여 eax와 [gate]의 값을 교환하고 명령어 test로 lock의 상태를 판별한다.
[gate]가 0인 경우에는 아무도 lock을 걸지 않은 상황으로 현재 thread가 락을 걸게 되는 상황이 된다. [gate]의 값이 명령어 xchg에 의해 항상 1이 들어가 있는 레지스터 eax와  교환이 이루어져 [gate]은 1로 바뀌고 락이 걸린 상태를 나타내게 된다.
이후 다른 thread가 들어오려 해도 [gate]의 값이 1이라 xchg의 교환이 무의미해지고 lock을 보장하게 된다.
함수 void spin_unlock()은 [gate]의 값을 0으로 만들어 lock이 해제된 상태로 만들어준다.
*/
void lab2_spin_lock(int volatile *lock)
{
	asm volatile(
	"jmp check\n"
	"wait:\n"
	"pause\n"
	"check:\n"
	"mov %[gate], %%eax\n"
	"lock xchg %%eax, %[lock]\n"
	"test %%eax, %%eax\n"
	"jnz wait\n"
	: [lock] "=m" (*lock)
	: [gate] "r" (1)
	: "eax"
	);
}
/* 
 * TODO - lab2 bonus
 *  Implement a function which acts likes spin_unlock.
 *
 *  @param int *lock        : lock variable's current status. 
 */
void lab2_spin_unlock(int volatile *lock)
{
	asm volatile(
	"mov %[ungate], %[lock]\n"
	: [lock] "=m" (*lock)
	: [ungate] "r" (0)
	);
}