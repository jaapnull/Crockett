#pragma once

// Assert as a macro so the __line__ etc keeps working
#include <assert.h>
#define gAssert(inAssertion) do{if(!(inAssertion)) __debugbreak(); }while(false)
#define gStaticAssert(e) typedef char __C_ASSERT__[(e)?1:-1]
