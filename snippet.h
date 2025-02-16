				{

// #define _TEST_ARG(arg, ...) printf("%s\n", #arg); TEST_ARG(__VA_ARGS__);
// #define TEST_ARG(arg, ...) _TEST_ARG(__VA_ARGS__);
// #define TEST(...) TEST_ARG(__VA_ARGS__)
// TEST(a = 1, b = 2);

#if 0
// Make a FOREACH macro
#define CALL_1(FUNCTION, X) FUNCTION(X) 
#define CALL_2(FUNCTION, X, ...) FUNCTION(X)CALL_1(FUNCTION, __VA_ARGS__)
#define CALL_3(FUNCTION, X, ...) FUNCTION(X)CALL_2(FUNCTION, __VA_ARGS__)
#define CALL_4(FUNCTION, X, ...) FUNCTION(X)CALL_3(FUNCTION, __VA_ARGS__)
#define CALL_5(FUNCTION, X, ...) FUNCTION(X)CALL_4(FUNCTION, __VA_ARGS__)

#define GET_MACRO(_1,_2,_3,_4,_5, NAME, ...) NAME 
#define FOR_EACH(FUNCTION, ...) GET_MACRO(__VA_ARGS__, CALL_5, CALL_4, CALL_3, CALL_2, CALL_1)(FUNCTION, __VA_ARGS__)

#define PRINT_ARG(arg) printf("%s\n", #arg);
#define PRINT(...) FOR_EACH(PRINT_ARG, __VA_ARGS__)

PRINT(a = "first", b = "second", c = "third")

#else // -----------------------------------------------------

#define __NARG__(...)  __NARG_I_(__VA_ARGS__, __RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)

#define CALL_PAIR_2(ACTION, X, Y)      ACTION(X, Y) 
#define CALL_PAIR_4(ACTION, X, Y, ...) ACTION(X, Y)CALL_PAIR_2(ACTION, __VA_ARGS__)

#define GET_MACRO(_1,_2,_3,_4,_5, NAME, ...) NAME 
#define FOR_EACH(ACTION, ...) _VFUNC(CALL_PAIR_, __NARG__(__VA_ARGS__))(ACTION, __VA_ARGS__)

#define PRINT_ARG_PAIR(key, value) printf("%s = %s\n", #key, #value);

FOR_EACH(PRINT_ARG_PAIR, a, b, c, d);

#endif

					printf("Before: %d\n", guiGetLayout()->max.y);
					for (auto backupLayout = guiGetLayoutCopy(); backupLayout.run_statement; backupLayout.run_statement = !backupLayout.run_statement, guiSetLayout(backupLayout))
					{
						guiGetLayout()->max.y = 99;
						printf("Inside: %d\n", guiGetLayout()->max.y);
					}
					printf("After: %d\n", guiGetLayout()->max.y);
				}