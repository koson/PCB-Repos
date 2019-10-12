#include "ax8052.h"
#include "libmfdbglink.h"

static volatile uint8_t __data fiforxwr;
static volatile uint8_t __data fiforxrd;
static volatile uint8_t __data fifotxwr;
static volatile uint8_t __data fifotxrd;

extern uint8_t __xdata dbglink_rxbuffer[];
extern uint8_t __xdata dbglink_txbuffer[];

#if !defined(SDCC)
extern const uint8_t __code dbglink_rxbuffer_size[];
extern const uint8_t __code dbglink_txbuffer_size[];
#endif

#if defined(SDCC)

static __reentrantb void dbglink_iocore(void) __reentrant __naked;

void dbglink_irq(void) __interrupt(21) __naked
{
	__asm;
	push	acc
	push	psw
	push	_DPS
	push	dpl
	push	dph
	push	b
	mov	_DPS,#0
	mov	psw,#0
	lcall	_dbglink_iocore
	pop	b
	pop	dph
	pop	dpl
	pop	_DPS
	pop	psw
	pop	acc
	reti
	__endasm;
}

uint8_t dbglink_poll(void) __reentrant __naked
{
	__asm;
	mov	a,#0x80
	anl	a,_IE
	rl	a
	mov	b,a
	clr	_EA
	lcall	_dbglink_iocore
	mov	a,b
	clr	c
	rrc	a
	mov	_EA,c
	mov	dpl,a
	ret

	.area HOME     (CODE)
	.area DBGLINK0 (CODE)
	.area DBGLINK1 (CODE)
	.area DBGLINK2 (CODE)
	.area DBGLINK3 (CODE)
	.area DBGLINK4 (CODE)
	.area DBGLINK5 (CODE)

	.area DBGLINK0 (CODE)
	__endasm;
}

static __reentrantb void dbglink_iocore(void) __reentrant __naked
{
	__asm;
	ar2=0x02
	ar3=0x03
	ar4=0x04
	ar5=0x05
	ar6=0x06
	ar7=0x07
	ar0=0x00
	ar1=0x01

	mov	a,_DBGLNKSTAT
	jnb	acc.0,iocnorx
	mov	dptr,#_dbglink_rxbuffer
	mov	a,_fiforxwr
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	mov	a,#_dbglink_buffer_negsize+3-00010$
	movc	a,@a+pc
00010$:	add	a,_fiforxwr
	jc	00001$
	mov	a,_fiforxwr
	inc	a
00001$:	cjne	a,_fiforxrd,00000$
	anl	_DBGLNKSTAT,#~0x40
	sjmp	iocnorx
00000$:	mov	_fiforxwr,a
	mov	a,_DBGLNKBUF
	movx	@dptr,a
	orl	_DBGLNKSTAT,#0x40
	setb	_B_1
iocnorx:
	mov	a,_DBGLNKSTAT
	jnb	acc.2,iocnotx
	mov	a,_fifotxrd
	cjne	a,_fifotxwr,00000$
	anl	_DBGLNKSTAT,#~0x80
	sjmp	iocnotx
00000$:	mov	dptr,#_dbglink_txbuffer
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	movx	a,@dptr
	mov	_DBGLNKBUF,a
	mov	a,#_dbglink_buffer_negsize+1-00010$
	movc	a,@a+pc
00010$:	add	a,_fifotxrd
	jc	00001$
	mov	a,_fifotxrd
	inc	a
00001$:	mov	_fifotxrd,a
	orl	_DBGLNKSTAT,#0x80
	setb	_B_2
iocnotx:
	ret
	__endasm;
}

void dbglink_rxadvance(uint8_t idx) __reentrant __naked
{
	idx;
	__asm;
	mov	a,#_dbglink_buffer_negsize+2-00003$
	movc	a,@a+pc
00003$:	xch	a,dpl
	jz	00000$
	add	a,_fiforxrd
	jnc	00002$
	add	a,dpl
	sjmp	00001$
00002$:	xch	a,dpl
	add	a,dpl
	jc	00001$
	mov	a,dpl
00001$:	mov	_fiforxrd,a
	orl	_DBGLNKSTAT,#0x40
00000$:	ret
	__endasm;
}

void dbglink_txadvance(uint8_t idx) __reentrant __naked
{
	idx;
	__asm;
	mov	a,#_dbglink_buffer_negsize-00003$
	movc	a,@a+pc
00003$:	xch	a,dpl
	jz	00000$
	add	a,_fifotxwr
	jnc	00002$
	add	a,dpl
	sjmp	00001$
00002$:	xch	a,dpl
	add	a,dpl
	jc	00001$
	mov	a,dpl
00001$:	mov	_fifotxwr,a
	orl	_DBGLNKSTAT,#0x80
00000$:	ret
	__endasm;
}

__reentrantb const uint8_t __xdata *dbglink_rxbufptr(uint8_t idx) __reentrant __naked
{
	idx;
	__asm;
	mov	a,#_dbglink_buffer_negsize+2-00003$
	movc	a,@a+pc
00003$:	xch	a,dpl
	add	a,_fiforxrd
	jnc	00002$
	add	a,dpl
	sjmp	00001$
00002$:	xch	a,dpl
	add	a,dpl
	jc	00001$
	mov	a,dpl
00001$:	mov	dptr,#_dbglink_rxbuffer
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	ret
	__endasm;
}

__reentrantb uint8_t __xdata *dbglink_txbufptr(uint8_t idx) __reentrant __naked
{
	idx;
	__asm;
	mov	a,#_dbglink_buffer_negsize-00003$
	movc	a,@a+pc
00003$:	xch	a,dpl
	add	a,_fifotxwr
	jnc	00002$
	add	a,dpl
	sjmp	00001$
00002$:	xch	a,dpl
	add	a,dpl
	jc	00001$
	mov	a,dpl
00001$:	mov	dptr,#_dbglink_txbuffer
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	ret

_dbglink_buffer_negsize:
	.area DBGLINK3 (CODE)
	__endasm;
}

uint8_t dbglink_txfreelinear(void) __reentrant __naked
{
	__asm;
	mov	a,_fifotxrd
	setb	c
	subb	a,_fifotxwr
	jnc	00000$
	mov	a,_fifotxrd
	add	a,#0xff
	cpl	c
	mov	a,#_dbglink_buffer_size-00001$
	movc	a,@a+pc
00001$:	subb	a,_fifotxwr
00000$:	mov	dpl,a
	ret
	__endasm;
}

uint8_t dbglink_txfree(void) __reentrant __naked
{
	__asm;
	mov	a,_fifotxrd
	setb	c
	subb	a,_fifotxwr
	mov	dpl,a
	jnc	00000$
	mov	a,#_dbglink_buffer_size-00001$
	movc	a,@a+pc
00001$:	add	a,dpl
	mov	dpl,a
00000$:	ret
	__endasm;
}

uint8_t dbglink_rxcountlinear(void) __reentrant __naked
{
	__asm;
	mov	a,_fiforxwr
	clr	c
	subb	a,_fiforxrd
	jnc	00000$
	mov	a,#_dbglink_buffer_size+1-00001$
	movc	a,@a+pc
00001$:	clr	c
	subb	a,_fiforxrd
00000$:	mov	dpl,a
	ret
	__endasm;
}

uint8_t dbglink_rxcount(void) __reentrant __naked
{
	__asm;
	mov	a,_fiforxwr
	clr	c
	subb	a,_fiforxrd
	mov	dpl,a
	jnc	00000$
	mov	a,#_dbglink_buffer_size+1-00001$
	movc	a,@a+pc
00001$:	add	a,dpl
	mov	dpl,a
00000$:	ret
	__endasm;
}

uint8_t dbglink_txbuffersize(void) __reentrant __naked
{
	__asm;
	mov	a,#_dbglink_buffer_size-00000$
	movc	a,@a+pc
00000$:	dec	a
	mov	dpl,a
	ret
	__endasm;
}

uint8_t dbglink_rxbuffersize(void) __reentrant __naked
{
	__asm;
	mov	a,#_dbglink_buffer_size+1-00000$
	movc	a,@a+pc
00000$:	dec	a
	mov	dpl,a
	ret

_dbglink_buffer_size:
	.area CSEG    (CODE)
	__endasm;
}

uint8_t dbglink_rxpeek(uint8_t idx) __reentrant __naked
{
	idx;
	__asm;
	lcall	_dbglink_rxbufptr
	movx	a,@dptr
	mov	dpl,a
	ret
	__endasm;
}

void dbglink_txpoke(uint8_t idx, uint8_t ch) __reentrant __naked
{
	idx;
	ch;
	__asm;
	push	ar0
	mov	a,sp
	add	a,#-3
	mov	r0,a
_dbglink_txpoke_hexentry:
	lcall	_dbglink_txbufptr
	mov	a,@r0
	movx	@dptr,a
	pop	ar0
	ret
	__endasm;
}

void dbglink_txpokehex(uint8_t idx, uint8_t ch) __reentrant __naked
{
	idx;
	ch;
	__asm;
	push	ar0
	mov	a,sp
	add	a,#-3
	mov	r0,a
	mov	a,@r0
	anl	a,#0x0F
	add	a,#256-10
	jnc	00000$
	add	a,#'A-'9-1
00000$:	add	a,#10+'0	; '
	mov	@r0,a
	ljmp	_dbglink_txpoke_hexentry
	__endasm;
}

uint8_t dbglink_txidle(void) __reentrant __naked
{
	__asm;
	mov	a,_DBGLNKSTAT
	anl	a,#0x84
	cjne	a,#0x04,00000$
	mov	dpl,#1
	ret
00000$:
	mov	dpl,#0
	ret
	__endasm;
}

static void wtimer_cansleep_dummy(void) __naked
{
	__asm
	.area HOME      (CODE)
	.area WTCANSLP0 (CODE)
	.area WTCANSLP1 (CODE)
	.area WTCANSLP2 (CODE)

	.area WTCANSLP1 (CODE)
	lcall	_dbglink_txidle
	mov	a,dpl
	jnz	00000$
	ret
00000$:
	.area CSEG      (CODE)
	__endasm;
}

#elif defined __CX51__ || defined __C51__

static __reentrantb void dbglink_iocore(void) __reentrant __naked;

void dbglink_irq(void) interrupt 21
{
#pragma asm
	push	acc
	push	psw
	push	DPS
	push	dpl
	push	dph
	push	b
	mov	DPS,#0
	mov	psw,#0
	lcall	_dbglink_iocore
	pop	b
	pop	dph
	pop	dpl
	pop	DPS
	pop	psw
	pop	acc
#pragma endasm
}

uint8_t dbglink_poll(void) __reentrant
{
#pragma asm
	mov	a,#0x80
	anl	a,IE
	rl	a
	mov	b,a
	clr	EA
	lcall	_dbglink_iocore
	mov	a,b
	clr	c
	rrc	a
	mov	EA,c
	mov	r7,a
#pragma endasm
}

static __reentrantb void dbglink_iocore(void) __reentrant __naked
{
	dbglink_rxbuffer[0];
	dbglink_rxbuffer_size[0];
	dbglink_txbuffer[0];
	dbglink_txbuffer_size[0];
#pragma asm
;ar2	equ	0x02
;ar3	equ	0x03
;ar4	equ	0x04
;ar5	equ	0x05
;ar6	equ	0x06
;ar7	equ	0x07
;ar0	equ	0x00
;ar1	equ	0x01

_dbglink_iocore:
	mov	a,DBGLNKSTAT
	jnb	acc.0,iocnorx
	clr	a
	mov	dptr,#dbglink_rxbuffer_size+2
	movc	a,@a+dptr
	push	acc
	mov	dptr,#dbglink_rxbuffer
	mov	a,fiforxwr
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	pop	acc
	add	a,fiforxwr
	jc	ioc1
	mov	a,fiforxwr
	inc	a
ioc1:	cjne	a,fiforxrd,ioc0
	anl	DBGLNKSTAT,#~0x40
	sjmp	iocnorx
ioc0:	mov	fiforxwr,a
	mov	a,DBGLNKBUF
	movx	@dptr,a
	orl	DBGLNKSTAT,#0x40
	setb	B_1
iocnorx:
	mov	a,DBGLNKSTAT
	jnb	acc.2,iocnotx
	mov	a,fifotxrd
	cjne	a,fifotxwr,ioc2
	anl	DBGLNKSTAT,#~0x80
	sjmp	iocnotx
ioc2:	mov	dptr,#dbglink_txbuffer
	add	a,dpl
	mov	dpl,a
	clr	a
	addc	a,dph
	mov	dph,a
	movx	a,@dptr
	mov	DBGLNKBUF,a
	clr	a
	mov	dptr,#dbglink_txbuffer_size+2
	movc	a,@a+dptr
	add	a,fifotxrd
	jc	ioc3
	mov	a,fifotxrd
	inc	a
ioc3:	mov	fifotxrd,a
	orl	DBGLNKSTAT,#0x80
	setb	B_2
iocnotx:
#pragma endasm
}

__reentrantb void dbglink_rxadvance(uint8_t idx) __reentrant
{
	idx;
#pragma asm
	clr	a
	mov	dptr,#dbglink_rxbuffer_size+1
	movc	a,@a+dptr
	xch	a,r7
	jz	rxad0
	add	a,fiforxrd
	jnc	rxad2
	add	a,r7
	sjmp	rxad1
rxad2:	xch	a,r7
	add	a,r7
	jc	rxad1
	mov	a,r7
rxad1:	mov	fiforxrd,a
	orl	DBGLNKSTAT,#0x40
rxad0:
#pragma endasm
}

__reentrantb void dbglink_txadvance(uint8_t idx) __reentrant
{
	idx;
#pragma asm
	clr	a
	mov	dptr,#dbglink_txbuffer_size+1
	movc	a,@a+dptr
	xch	a,r7
	jz	txad0
	add	a,fifotxwr
	jnc	txad2
	add	a,r7
	sjmp	txad1
txad2:	xch	a,r7
	add	a,r7
	jc	txad1
	mov	a,r7
txad1:	mov	fifotxwr,a
	orl	DBGLNKSTAT,#0x80
txad0:
#pragma endasm
}

__reentrantb const uint8_t __xdata *dbglink_rxbufptr(uint8_t idx) __reentrant
{
	idx;
#pragma asm
_dbglink_rxbufptr:
	clr	a
	mov	dptr,#dbglink_rxbuffer_size+1
	movc	a,@a+dptr
	xch	a,r7
	add	a,fiforxrd
	jnc	rxp2
	add	a,r7
	sjmp	rxp1
rxp2:	xch	a,r7
	add	a,r7
	jc	rxp1
	mov	a,r7
rxp1:	mov	dptr,#dbglink_rxbuffer
	add	a,dpl
	mov	r7,a
	clr	a
	addc	a,dph
	mov	r6,a
#pragma endasm
}

__reentrantb uint8_t __xdata *dbglink_txbufptr(uint8_t idx) __reentrant
{
	idx;
#pragma asm
_dbglink_txbufptr:
	clr	a
	mov	dptr,#dbglink_txbuffer_size+1
	movc	a,@a+dptr
	xch	a,r7
	add	a,fifotxwr
	jnc	txp2
	add	a,r7
	sjmp	txp1
txp2:	xch	a,r7
	add	a,r7
	jc	txp1
	mov	a,r7
txp1:	mov	dptr,#dbglink_txbuffer
	add	a,dpl
	mov	r7,a
	clr	a
	addc	a,dph
	mov	r6,a
#pragma endasm
}

__reentrantb uint8_t dbglink_txfreelinear(void) __reentrant
{
#pragma asm
	mov	a,fifotxrd
	setb	c
	subb	a,fifotxwr
	jnc	txfrl0
	mov	a,fifotxrd
	add	a,#0xff
	cpl	c
	clr	a
	mov	dptr,#dbglink_txbuffer_size
	movc	a,@a+dptr
	subb	a,fifotxwr
txfrl0:	mov	r7,a
#pragma endasm
}

__reentrantb uint8_t dbglink_txfree(void) __reentrant
{
#pragma asm
	mov	a,fifotxrd
	setb	c
	subb	a,fifotxwr
	mov	r7,a
	jnc	txfr0
	clr	a
	mov	dptr,#dbglink_txbuffer_size
	movc	a,@a+dptr
	add	a,r7
	mov	r7,a
txfr0:
#pragma endasm
}

__reentrantb uint8_t dbglink_rxcountlinear(void) __reentrant
{
#pragma asm
	mov	a,fiforxwr
	clr	c
	subb	a,fiforxrd
	mov	r7,a
	jnc	rxcl0
	clr	a
	mov	dptr,#dbglink_rxbuffer_size
	movc	a,@a+dptr
	add	a,r7
rxcl0:	mov	r7,a

#pragma endasm
}

__reentrantb uint8_t dbglink_rxcount(void) __reentrant
{
#pragma asm
	mov	a,fiforxwr
	clr	c
	subb	a,fiforxrd
	mov	r7,a
	jnc	rxc0
	clr	a
	mov	dptr,#dbglink_rxbuffer_size
	movc	a,@a+dptr
	add	a,r7
	mov	r7,a
rxc0:
#pragma endasm
}

__reentrantb uint8_t dbglink_txbuffersize(void) __reentrant
{
	return dbglink_txbuffer_size[0]-1;
}

__reentrantb uint8_t dbglink_rxbuffersize(void) __reentrant
{
	return dbglink_rxbuffer_size[0]-1;
}

__reentrantb uint8_t dbglink_rxpeek(uint8_t idx) __reentrant
{
	idx;
#pragma asm
	lcall	_dbglink_rxbufptr
	mov	dpl,r7
	mov	dph,r6
	movx	a,@dptr
	mov	r7,a
#pragma endasm
}

__reentrantb void dbglink_txpoke(uint8_t idx, uint8_t ch) __reentrant
{
	idx;
	ch;
#pragma asm
_dbglink_txpokehex_entry:
	lcall	_dbglink_txbufptr
	mov	dpl,r7
	mov	dph,r6
	mov	a,r5
	movx	@dptr,a
#pragma endasm
}

__reentrantb void dbglink_txpokehex(uint8_t idx, uint8_t ch) __reentrant
{
	idx;
	ch;
#pragma asm
	mov	a,r5
	anl	a,#0x0F
	add	a,#256-10
	jnc	txph0
	add	a,#'A'-'9'-1
txph0:	add	a,#10+'0'
	mov	r5,a
	ljmp	_dbglink_txpokehex_entry
#pragma endasm
}

__reentrantb uint8_t dbglink_txidle(void) __reentrant
{
#pragma asm
	mov	a,DBGLNKSTAT
	anl	a,#0x84
	mov	r7,#0
	cjne	a,#0x04,txnotidle
	mov	r7,#1
txnotidle:
#pragma endasm
}

#elif defined __ICC8051__

static __reentrantb uint8_t dbglink_iocore(void) __reentrant;

#pragma vector=0xab
__interrupt void dbglink_irq(void)
{
	uint8_t __autodata dpssave = DPS;
	DPS = 0;
	dbglink_iocore();
	DPS = dpssave;
}

__reentrantb uint8_t dbglink_poll(void) __reentrant
{
	uint8_t flg;
	uint8_t irq = IE & 0x80;
	EA = 0;
	flg = dbglink_iocore();
	IE |= irq;
	return flg;
}

static __reentrantb uint8_t dbglink_iocore(void) __reentrant
{
	uint8_t flg = 0;
	if (DBGLNKSTAT & 0x01) {
		uint8_t wp = fiforxwr + 1;
		uint8_t sz = dbglink_rxbuffer_size[0];
		if (wp >= sz)
			wp -= sz;
		if (wp != fiforxrd) {
			dbglink_rxbuffer[fiforxwr] = DBGLNKBUF;
			fiforxwr = wp;
			DBGLNKSTAT |= 0x40;
			flg |= 1;
		} else {
			DBGLNKSTAT &= (uint8_t)~0x40;
		}
	}
	if (DBGLNKSTAT & 0x04) {
		if (fifotxrd != fifotxwr) {
			uint8_t rp = fifotxrd + 1;
			uint8_t sz = dbglink_txbuffer_size[0];
			DBGLNKBUF = dbglink_txbuffer[fifotxrd];
			if (rp >= sz)
				rp -= sz;
			fifotxrd = rp;
			DBGLNKSTAT |= 0x80;
			flg |= 2;
		} else {
			DBGLNKSTAT &= (uint8_t)~0x80;
		}
	}
	return flg;
}

__reentrantb void dbglink_rxadvance(uint8_t idx) __reentrant
{
	uint8_t rd;
	uint8_t sz;
	if (!idx)
		return;
	rd = fiforxrd;
	idx += rd;
	sz = dbglink_rxbuffer_size[0];
	if (idx < rd || idx >= sz)
		idx -= sz;
	fiforxrd = idx;
	DBGLNKSTAT |= 0x40;
}

__reentrantb void dbglink_txadvance(uint8_t idx) __reentrant
{
	uint8_t wr;
	uint8_t sz;
	if (!idx)
		return;
	wr = fifotxwr;
	idx += wr;
	sz = dbglink_txbuffer_size[0];
	if (idx < wr || idx >= sz)
		idx -= sz;
	fifotxwr = idx;
	DBGLNKSTAT |= 0x80;
}

__reentrantb const uint8_t __xdata *dbglink_rxbufptr(uint8_t idx) __reentrant
{
	uint8_t rd = fiforxrd;
	uint8_t sz = dbglink_rxbuffer_size[0];
	idx += rd;
	if (idx < rd || idx >= sz)
		idx -= sz;
	return &dbglink_rxbuffer[idx];
}

__reentrantb uint8_t __xdata *dbglink_txbufptr(uint8_t idx) __reentrant
{
	uint8_t wr = fifotxwr;
	uint8_t sz = dbglink_txbuffer_size[0];
	idx += wr;
	if (idx < wr || idx >= sz)
		idx -= sz;
	return &dbglink_txbuffer[idx];
}

__reentrantb uint8_t dbglink_txfreelinear(void) __reentrant
{
	uint8_t rd = fifotxrd;
	uint8_t wr = fifotxwr;
	if (rd <= wr) {
		uint8_t r = dbglink_txbuffer_size[0] - wr;
		if (!rd)
			--r;
		return r;
	}
	return rd - wr - 1;
}

__reentrantb uint8_t dbglink_txfree(void) __reentrant
{
	uint8_t rd = fifotxrd;
	uint8_t wr = fifotxwr;
	uint8_t r = rd - wr;
	if (rd <= wr)
		r += dbglink_txbuffer_size[0];
	--r;
	return r;
}

__reentrantb uint8_t dbglink_rxcountlinear(void) __reentrant
{
	uint8_t rd = fiforxrd;
	uint8_t wr = fiforxwr;
	if (wr < rd)
		return dbglink_rxbuffer_size[0] - rd;
	return wr - rd;
}

__reentrantb uint8_t dbglink_rxcount(void) __reentrant
{
	uint8_t rd = fiforxrd;
	uint8_t wr = fiforxwr;
	uint8_t r = wr - rd;
	if (wr < rd)
		r += dbglink_rxbuffer_size[0];
	return r;
}

__reentrantb uint8_t dbglink_txbuffersize(void) __reentrant
{
	return dbglink_txbuffer_size[0]-1;
}

__reentrantb uint8_t dbglink_rxbuffersize(void) __reentrant
{
	return dbglink_rxbuffer_size[0]-1;
}

__reentrantb uint8_t dbglink_rxpeek(uint8_t idx) __reentrant
{
	const uint8_t __xdata *bp = dbglink_rxbufptr(idx);
	return *bp;
}

__reentrantb void dbglink_txpoke(uint8_t idx, uint8_t ch) __reentrant
{
	uint8_t __xdata *bp = dbglink_txbufptr(idx);
	*bp = ch;
}

__reentrantb void dbglink_txpokehex(uint8_t idx, uint8_t ch) __reentrant
{
	ch &= 0x0F;
	if (ch >= 10)
		ch += 'A' - '9' - 1;
	ch += '0';
	dbglink_txpoke(idx, ch);
}

__reentrantb uint8_t dbglink_txidle(void) __reentrant
{
	if ((DBGLNKSTAT & 0x84) == 0x04)
		return 1;
	return 0;
}

#else

#error "Compiler unsupported"

#endif

__reentrantb void dbglink_init(void) __reentrant
{
	fiforxwr = fiforxrd = fifotxwr = fifotxrd = 0;
	DBGLNKSTAT = 0x40;
	E2IE |= 0x40;
}
