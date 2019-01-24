#include <inc/env.h>
#include <inc/pmap.h>
#include <inc/printf.h>
#include <drivers/leds.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *  Search through 'envs' for a runnable environment ,
 *  in circular fashion starting after the previously running env,
 *  and switch to the first such environment found.
 *
 * 
 *  The variable which is for counting should be defined as 'static'.
 */
//when occur next exception break,carry out this function again. 
void sched_yield(void)
{
	static int pos = -1;
	while(1){
		pos = (pos+1)%NENV;
		if(envs[pos].env_status==ENV_RUNNABLE){
			printf("envs %x pos %d &envs[pos] %x\n", envs, pos, &envs[pos]);
			env_run(&envs[pos]);
		}
	}

}

void mytest_irq()
{
	printf("irq\n");
}