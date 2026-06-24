#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>

void board_early_init_hook(void)
{
    printk("Board Initialized\n");
}

