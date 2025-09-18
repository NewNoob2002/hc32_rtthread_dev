/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */

#include <stdint.h>
#include <rtthread.h>
#include "delay.h"
#include "sysclock.h"

#define EXAMPLE_PERIPH_WE                                                     \
    (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | \
     LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

char rt_hw_console_getchar(void);
void rt_hw_console_output(const char *str);
void io_putc(int ch);
int io_getc(void);
void systick_handle();

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE (1024 * 8)
static uint32_t rt_heap[RT_HEAP_SIZE]; // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif
/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
    M4_PORT->PSPCR = 0x03;
	  clock_init();
    delay_init();

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
	    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();
    systick_ticks++;
    //		systick_handle();
    /* leave interrupt */
    rt_interrupt_leave();
}

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            io_putc(a);
        }
        io_putc(*(str + i));
    }
}

char rt_hw_console_getchar(void)
{
    int ch = -1;
    ch = io_getc();
    return ch;
}
