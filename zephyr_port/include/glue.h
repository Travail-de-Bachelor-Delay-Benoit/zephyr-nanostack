#ifndef _GLUE_H_
#define _GLUE_H_

#include "zephyr_port.h"

#include <mbedtls/build_info.h>
#include <mbedtls/sha256.h>
#include <mbedtls/aes.h>
#include <mbedtls/md.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <string.h>
#include <strings.h>

#define device ns_device

#endif