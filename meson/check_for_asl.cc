#include <cinttypes>

static_assert((std::int16_t(-1) >> 1) == -1);
static_assert((std::int32_t(-1) >> 1) == -1);
