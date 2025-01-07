#pragma once

#ifdef _DEBUG
    const char* get_filename(const char* path);
    #define __FILENAME__ get_filename(__FILE__)

    //###############################################################################################################################
    #define _JP_MSG_TAG "-->"
    #define _JP_DEBUG_PRINT_PREFIX_FORMAT "%s   [%s]   "

    //###############################################################################################################################
    #define _JP_DEBUG_PRINT_VAR_PRE\
        { \
            char buff[1024];\
            sprintf(buff, "%s %s:%i, %s()", _JP_MSG_TAG, __FILENAME__, __LINE__, __FUNCTION__); \

    #define _JP_DEBUG_PRINT_VAR_POST\
            sprintf(buff, "%s\n", buff); \
            OutputDebugString(buff); \
        }

    //###############################################################################################################################
    #define _JP_DEBUG_PRINT_VAR1(_prefix, _format, _var)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format, buff, _prefix, #_var, _var);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR1_TIMES2(_prefix, _format, _var)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format, buff, _prefix, #_var, _var, _var);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR2(_prefix, _format, _var1, _var2)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format##", %s="##_format, buff, _prefix, #_var1, _var1, #_var2, _var2);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR2_TIMES2(_prefix, _format, _var1, _var2)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format##", %s="##_format, buff, _prefix, #_var1, _var1, _var1, #_var2, _var2, _var2);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR3(_prefix, _format, _var1, _var2, _var3)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format##", %s="##_format##", %s="##_format, buff, _prefix, #_var1, _var1, #_var2, _var2, #_var3, _var3);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR3_TIMES2(_prefix, _format, _var1, _var2, _var3)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format##", %s="##_format##", %s="##_format, buff, _prefix, #_var1, _var1, _var1, #_var2, _var2, _var2, #_var3, _var3, _var3);\
        _JP_DEBUG_PRINT_VAR_POST

    #define _JP_DEBUG_PRINT_VAR4_TIMES2(_prefix, _format, _var1, _var2, _var3, _var4)\
        _JP_DEBUG_PRINT_VAR_PRE \
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s="##_format##", %s="##_format##", %s="##_format##", %s="##_format, buff, _prefix, #_var1, _var1, _var1, #_var2, _var2, _var2, #_var3, _var3, _var3, #_var4, _var4, _var4);\
        _JP_DEBUG_PRINT_VAR_POST

    //###############################################################################################################################
    #define JP_DEBUG_PRINT_TAG(_tag)\
        _JP_DEBUG_PRINT_VAR_PRE\
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT, buff, _tag);\
        _JP_DEBUG_PRINT_VAR_POST

    #define JP_DEBUG_PRINT_STR(_prefix, _str)\
        _JP_DEBUG_PRINT_VAR_PRE\
        sprintf(buff, _JP_DEBUG_PRINT_PREFIX_FORMAT##"%s", buff, _prefix, _str);\
        _JP_DEBUG_PRINT_VAR_POST

    #define JP_DEBUG_PRINT_STR_COND(_prefix, _cond)\
        if(_cond) {\
            JP_DEBUG_PRINT_STR(_prefix, #_cond);\
        }

    #define JP_DEBUG_PRINT_VAR_STR(_prefix, _var)\
        _JP_DEBUG_PRINT_VAR1(_prefix, "'%s'", _var)

    #define JP_DEBUG_PRINT_VAR_STR2(_prefix, _var1, _var2)\
        _JP_DEBUG_PRINT_VAR2(_prefix, "'%s'", _var1, _var2)

    #define JP_DEBUG_PRINT_VAR_INT(_prefix, _var)\
        _JP_DEBUG_PRINT_VAR1(_prefix, "%i", _var)

    #define JP_DEBUG_PRINT_VAR_INT_COND(_prefix, _var, _cond)\
        if(_cond) {\
            _JP_DEBUG_PRINT_VAR1(_prefix, "%i ("###_cond##")", _var)\
        }

    #define JP_DEBUG_PRINT_VAR_INT2(_prefix, _var1, _var2)\
        _JP_DEBUG_PRINT_VAR2(_prefix, "%i", _var1, _var2)
    
    #define JP_DEBUG_PRINT_VAR_INT3(_prefix, _var1, _var2, _var3)\
        _JP_DEBUG_PRINT_VAR3(_prefix, "%i", _var1, _var2, _var3)

    #define JP_DEBUG_PRINT_VAR_UINT(_prefix, _var)\
        _JP_DEBUG_PRINT_VAR1(_prefix, "%u", _var)

    #define JP_DEBUG_PRINT_VAR_HEX(_prefix, _var)\
        _JP_DEBUG_PRINT_VAR1_TIMES2(_prefix, "0x%08x (%u)", _var)

    #define JP_DEBUG_PRINT_VAR_HEX2(_prefix, _var1, _var2)\
        _JP_DEBUG_PRINT_VAR2_TIMES2(_prefix, "0x%08x (%u)", _var1, _var2)

    #define JP_DEBUG_PRINT_VAR_HEX3(_prefix, _var1, _var2, _var3)\
        _JP_DEBUG_PRINT_VAR3_TIMES2(_prefix, "0x%08x (%u)", _var1, _var2, _var3)

    #define JP_DEBUG_PRINT_VAR_HEX4(_prefix, _var1, _var2, _var3, _var4)\
        _JP_DEBUG_PRINT_VAR4_TIMES2(_prefix, "0x%08x (%u)", _var1, _var2, _var3, _var4)

    #define JP_DEBUG_PRINT_VAR_INT(_prefix, _var)\
        _JP_DEBUG_PRINT_VAR1(_prefix, "%d", _var)
#else //_DEBUG
    //###############################################################################################################################
    #define _JP_DEBUG_PRINT_VAR1(_prefix, _format, _var) {}`
    #define _JP_DEBUG_PRINT_VAR1_TIMES2(_prefix, _format, _var) {}
    #define _JP_DEBUG_PRINT_VAR2(_prefix, _format, _var1, _var2) {}
    #define _JP_DEBUG_PRINT_VAR2_TIMES2(_prefix, _format, _var1, _var2) {}
    #define _JP_DEBUG_PRINT_VAR3_TIMES2(_prefix, _format, _var1, _var2, _var3) {}
    #define _JP_DEBUG_PRINT_VAR4_TIMES2(_prefix, _format, _var1, _var2, _var3, _var4) {}

    //###############################################################################################################################
    #define JP_DEBUG_PRINT_TAG(_tag) {}
    #define JP_DEBUG_PRINT_STR(_prefix, _str) {}
    #define JP_DEBUG_PRINT_STR_COND(_prefix, _cond) {}
    #define JP_DEBUG_PRINT_VAR_STR(_prefix, _var) {}
    #define JP_DEBUG_PRINT_VAR_STR2(_prefix, _var1, _var2) {}
    #define JP_DEBUG_PRINT_VAR_INT(_prefix, _var) {}
    #define JP_DEBUG_PRINT_VAR_INT_COND(_prefix, _var, _cond) {}
    #define JP_DEBUG_PRINT_VAR_INT2(_prefix, _var1, _var2)
    #define JP_DEBUG_PRINT_VAR_UINT(_prefix, _var) {}
    #define JP_DEBUG_PRINT_VAR_HEX(_prefix, _var) {}
    #define JP_DEBUG_PRINT_VAR_HEX2(_prefix, _var1, _var2) {}
    #define JP_DEBUG_PRINT_VAR_HEX3(_prefix, _var1, _var2, _var3) {}
    #define JP_DEBUG_PRINT_VAR_HEX4(_prefix, _var1, _var2, _var3, _var4) {}

#endif //_DEBUG
