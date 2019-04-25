#include <cc/int_types.h>

static_assert(sizeof(cc::u8) == 1, "cc::u8 must be 1 byte in size.");
static_assert(sizeof(cc::u16) == 2, "cc::u16 must be 2 bytes in size.");
static_assert(sizeof(cc::u32) == 4, "cc::u32 must be 4 bytes in size.");
