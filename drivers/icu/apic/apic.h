/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef CPU_APIC_H_
#define CPU_APIC_H_

/** x86 apic mapped memory registers */
#define APIC_REG_LAPIC_ID       0x0020      
#define APIC_REG_LAPIC_VERSION  0x0030      
#define APIC_REG_CONTEXT_PRIO   0x0080  
#define APIC_REG_ARBITR_PRIO    0x0090  
#define APIC_REG_PROCESSOR_PRIO 0x00a0  
#define APIC_REG_EOI            0x00b0  
#define APIC_REG_RES00C0        0x00c0  
#define APIC_REG_LOGICAL_DEST   0x00d0  
#define APIC_REG_DEST_FORMAT    0x00e0  
#define APIC_REG_SPURIOUS_INT   0x00f0  
#define APIC_REG_ISR            0x0100  //< in service register 
#define APIC_REG_TMR            0x0180  //< trigger mode register 
#define APIC_REG_IRR            0x0200  //< interrupt request register 
#define APIC_REG_ERROR          0x0280  //< error status register 
#define APIC_REG_ICR_0_31       0x0300  //< interrupt command register 
#define APIC_REG_ICR_32_63      0x0310  //< interrupt command register 
#define APIC_REG_LVT_TIMER      0x0320  //< LVT Timer Register 
#define APIC_REG_LVT_THERMAL    0x0330  //< LVT Thermal Sensor Register 
#define APIC_REG_LVT_PERF       0x0340  //< LVT Performance Monitoring Counters 
#define APIC_REG_LVT_LINT0      0x0350  //< LVT LINT0 Register 
#define APIC_REG_LVT_LINT1      0x0360  //< LVT LINT1 Register 
#define APIC_REG_LVT_ERROR      0x0370  //< LVT Error Register 
#define APIC_REG_TIMER_INIT     0x0380  //< Initial Count Register 
#define APIC_REG_TIMER_CUR      0x0390  //< Current Count Register 
#define APIC_REG_TIMER_DIV      0x03e0  //< Divide Configuration Register 

#define APIC_LVT_TIMER_PERIODIC (1<<17) //< timer is periodic(1) or one-shot(0)
#define APIC_LVT_MASKED         (1<<16) //< irq is masked(1)
#define APIC_LVT_TRIG_LEVEL     (1<<15) //< trigger mode level(1) edge(0)
#define APIC_LVT_REMOTE_IRR     (1<<14)
#define APIC_LVT_PIN_POLARITY   (1<<13)
#define APIC_LVT_SEND_PENDING   (1<<12) //< delivery status idle(0) pending(1)
#define APIC_LVT_DELIVERY_FIXED 0x000   //< delivery mode is fixed
#define APIC_LVT_DELIVERY_SMI   0x200   //< delivery mode is SMI
#define APIC_LVT_DELIVERY_NMI   0x400   //< delivery mode is NMI
#define APIC_LVT_DELIVERY_EXT   0x700   //< delivery mode is external (vector given by 8259)
#define APIC_LVT_DELIVERY_INIT  0x500   //< delivery mode is INIT
#define APIC_LVT_VECTOR_MASK    0x0ff   //< vector is lower byte

#define APIC_ESR_SEND_CKSUM     (1<<0)
#define APIC_ESR_RCV_CKSUM      (1<<1)
#define APIC_ESR_SEND_ACCEPT    (1<<2)
#define APIC_ESR_RCV_ACCEPT     (1<<3)
#define APIC_ESR_SEND_ILL       (1<<5)
#define APIC_ESR_RCV_ILL        (1<<6)
#define APIC_ESR_ILL_REG        (1<<7)

#define APIC_TIMER_DIV_1        0xd
#define APIC_TIMER_DIV_2        0x0
#define APIC_TIMER_DIV_4        0x1
#define APIC_TIMER_DIV_8        0x2
#define APIC_TIMER_DIV_16       0x3
#define APIC_TIMER_DIV_32       0x8
#define APIC_TIMER_DIV_64       0xb
#define APIC_TIMER_DIV_128      0xc

#define APIC_ICR_DST_NORMAL     (0<<18) //< no shorthand
#define APIC_ICR_DST_SELF       (1<<18) //< send to self
#define APIC_ICR_DST_ALL        (2<<18) //< send to all including self
#define APIC_ICR_DST_ALL_NOSELF (3<<18) //< send to all excluding self

#define APIC_ICR_TRIG_LEVEL     (1<<15) //< trigger mode level(1) edge(0)
#define APIC_ICR_LEVEL_ASSERT   (1<<14) //< level assert(1) de-assert(0)
#define APIC_ICR_SEND_PENDING   (1<<12) //< delivery status idle(0) pending(1)
#define APIC_ICR_LOGICAL_DST    (1<<11) //< logical destination(1) physical(0)
#define APIC_ICR_DELIVERY_FIXED 0x000   //< delivery mode is fixed
#define APIC_ICR_DELIVERY_LOW   0x100   //< delivery mode is Lowest priority
#define APIC_ICR_DELIVERY_SMI   0x200   //< delivery mode is SMI
#define APIC_ICR_DELIVERY_NMI   0x400   //< delivery mode is NMI
#define APIC_ICR_DELIVERY_EXT   0x700   //< delivery mode is external (vector given by 8259)
#define APIC_ICR_DELIVERY_INIT  0x500   //< delivery mode is INIT
#define APIC_ICR_DELIVERY_START 0x600   //< delivery mode is Start Up
#define APIC_ICR_VECTOR_MASK    0x0ff   //< vector is lower byte

#define APIC_SPUR_APIC_ENABLE   0x100
#define APIC_SPUR_VECTOR_MASK   0x0ff

#endif

