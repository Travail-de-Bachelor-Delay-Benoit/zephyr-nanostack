#include "sample.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mon_module, LOG_LEVEL_DBG);

void ma_fonction_module(void)
{
    LOG_INF("Bonjour ! Je suis la Nanostack !");
}