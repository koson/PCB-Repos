#include "ax8052.h"
#include "wtimer.h"

__reentrantb uint32_t wtimer1_curtime(void) __reentrant
{
	uint32_t r;
	uint8_t __autodata iesave = IE_1;
    uint8_t __autodata iesave_radio = IE_4;
	IE_4 = 0;
	IE_1 = 0;
	wtimer1_update();
	r = wtimer_state[1].time.cur;
	IE_1 = iesave;
	IE_4 = iesave_radio;
	return r;
}
