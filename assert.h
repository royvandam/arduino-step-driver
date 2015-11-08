#pragma once

#ifdef WITH_ASSERT
    /**
     * @brief   The assert_param macro is used to check function input parameters.
     * @param   expression: If the expression is false, it calls the assert_failed
     *          function which reports the name of the source file and the source
     *          line number of the call that failed. If the expression is true, it
     *          does nothing.
     */
    #define assert(expression) ((expression) \
        ? (void)0                            \
        : assert_failed(__FILE__, __LINE__))

    void assert_failed(char const* file, int line);
#else
    #define assert_param(expression) ((void)0)
#endif