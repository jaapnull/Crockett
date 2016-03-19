#pragma once

// Assert as a macro so the __line__ etc keeps working
#include <assert.h>
#define gAssert(inAssertion) assert(inAssertion)
#define gStaticAssert(e) typedef char __C_ASSERT__[(e)?1:-1]
