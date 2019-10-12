#include "ax8052.h"
#include "wtimer.h"

__reentrantb void wtimer_add_callback(struct wtimer_callback __xdata *desc) __reentrant
{
        uint8_t __autodata iesave = IE_1;
        uint8_t __autodata iesave_radio = IE_4;
        IE_4 = 0;
        IE_1 = 0;
        wtimer_addcb_core(desc);
        IE_1 = iesave;
        IE_4 = iesave_radio;
}
