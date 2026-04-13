#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#include <stdint.h>

typedef uint8_t (*critical_section_enter_fn)(void *ctx);
typedef void (*critical_section_exit_fn)(void *ctx, uint8_t state);

typedef struct {
    critical_section_enter_fn enter;
    critical_section_exit_fn exit;
    void *ctx;
} critical_section_ops_t;

#endif
