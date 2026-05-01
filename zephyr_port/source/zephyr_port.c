#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include "zephyr_port.h"

static unsigned int irq_key;

void platform_enter_critical()
{
    irq_key = irq_lock();
}

void platform_exit_critical()
{
    irq_unlock(irq_key);
}

static void (*timer_callback)(void) = NULL;

void platform_timer_set_cb(void (*new_fp)(void))
{
    timer_callback = new_fp;
}

// Handler appelé quand le timer Zephyr expire
void zephyr_timer_handler(struct k_timer *dummy)
{
    if (timer_callback)
    {
        timer_callback();
    }
}

K_TIMER_DEFINE(nanostack_timer, zephyr_timer_handler, NULL);

void platform_timer_start(uint16_t slots)
{
    // Nanostack demande un timer de 'slots' millisecondes
    k_timer_start(&nanostack_timer, K_MSEC(slots), K_NO_WAIT);
}

uint16_t platform_timer_get_remaining_slots(void)
{
    return (uint16_t)k_timer_remaining_get(&nanostack_timer);
}

// --- 3. Signal de l'ordonnanceur ---
void eventOS_scheduler_signal(void)
{
    // Dans un portage simple, on peut laisser vide.
    // Dans un portage multi-thread, on utiliserait k_sem_give pour réveiller le thread Nanostack.
}