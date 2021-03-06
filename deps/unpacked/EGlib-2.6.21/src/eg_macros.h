/* ========================================================================= */
/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza and Marcos Goycoolea.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 * */
/* ========================================================================= */
/** @defgroup EGmacros General Macros
 * global macros and types for EGlib
 *
 * @version 0.9.2 
 * @par History:
 * - 2013-04-30
 * 						- Add EGaGetCElem EGaGetElem EGaGetPos to get array-based pointers and
 * 						positions
 * - 2012-10-12
 * 						- Add TESTGD that jumps automatically to CLEANUP
 * - 2011-12-06
 * 						- Add generic signal handler and necesary macros
 * - 2011-05-16
 * 						- Add WARNIF(xxx) to warn (on screen) non-zero return values
 * - 2010-08-31
 * 						- Add EGcallD(xxx) short hand to EGcall(rval,CLEANUP,xxx)
 * - 2010-04-30
 * 						- Add IFMESSAGE messaging macro
 * - 2010-02-20
 * 						- Add EGcall macro to resume tracing calls.
 * - 2009-07-21
 * 						- Change macro-variables to __name convention to avoid name
 * 						clashes
 * - 2008-08-29
 * 						- Add EG_RETURN
 * - 2008-07-24
 * 						- Add TESTGL
 * - 2007-12-07
 * 						- Add FTESTG and FTEST, that always test the parameters
 * 							regardless of the debug level
 * - 2007-01-19
 * 						- Delete EGosGetOffset
 * - 2006-09-28
 * 						- Add function that display basic process information, including
 * 						version and date of compilation of EGlib
 * - 2005-12-19
 * 						- Add float128 support ussing SoftFloat.
 * - 2005-10-28
 * 						- Add some status definitions for algorithms.
 * - 2005-06-14
 * 						- Add strdup definition, just for cleanliness when compiling
 * - 2005-05-23
 * 						- Add EGcontainerOf
 * - 2005-05-03
 * 						- Add typeof definition;
 * - 2004-07-14
 * 						- Add GNU_MP_Z GNU_MP_F and GNU_MP_Q to the type definitions.
 * - 2004-07-12
 * 						- Add EGRAT_TYPE to the type definitions.
 * - 2004-03-17
 * 						- Add TESTG that if recives something that is nonzero print an
 * 							user message and the go to the given location.
 * - 2004-02-05
 * 						- Add CHECKRVALG that checks a return value, display a mesage,
 * 							and then perform a goto.
 * - 2003-12-01
 * 						- Add definition of a 'copy' function and its MP version.
 * - 2003-11-20
 * 						- Add PTRTEST that check if a pointer points to the first 64Kb of
 * 							memory internal memory. Althought such situation may happend 
 * 							(if we work in kernel-related stuff), it is usually an error 
 * 							when we try to access such a memory.
 * - 2003-09-08
 * 						- Add ADVTESTL
 * - 2003-07-10
 * 						- Add MESSAGEF, ADVCHECKRVAL
 * - 2003-07-02
 * 						- Add EGosGetData, EGosSetData, EGosGetOffset
 * - 2003-06-16
 * 						- Add EXITL macro
 * - 2003-06-06 
 * 						- Add TESTL macro to test conditions but only when the debug
 * 							level is at least some value
 * - 2003-05-22 
 * 						- Add EXITRVAL
 * - 2003-05-15 
 * 						- Add CHECKRVAL MESSAGE and WARNING macros.
 * - 2003-05-08 
 * 						- Add support for variadic macros for EXIT and TEST
 * 						- Define EGRAND_MAX for SUN and LINUX acordingly, this is becouse 
 * 							for some reason the value of RAND_MAX in SUN is not as 
 * 							specified in stdlib.h but rather 1 << 31 - 1. Still I am not 
 * 							sure about the maximum value of rand() on sun... will fix that 
 * 							later on to.
 * 						- Add a mesage macro, it only print if the debug level is as high
 * 							as required by the first field. Again the definition is
 * 							variadric and if the debug level is 0 we reduce the macro to 
 * 							the empty instruction.
 *
 * */
/**  @{ */
/** @file 
 * */
/* ========================================================================= */

#ifndef __EG_MACROS_H__
#define __EG_MACROS_H__
#include "eg_config.h"

/* ========================================================================= */
/** @brief return the offset of a member inside a structure.
 * @param type the type of the containing structure.
 * @param member the name of the member that we are interested in compute the
 * offset.
 * @return the number of bytes between the member and the beginning of the
 * structure. */
#define EGoffsetOf(__type,__member) ((size_t) &((__type *)0)->__member)

/* ========================================================================= */
/** @brief given a pointer to a member of a structure, return the pointer to
 * the head of the structure. (idea taken from Linux Kernel).
 * @param __ptr pointer to the member of the containing structure.
 * @param __type name type of the containing structure.
 * @param __member name of the given member in the containing structure.
 * @return pointer to the containing structure.
 * */
#define EGcontainerOf(__ptr,__type,__member) ({\
	typeof(((__type *)0)->__member) *const __EGcOf_ptr = (__ptr);\
	(__type *)( (char*)__EGcOf_ptr - ((size_t) &((__type *)0)->__member));})

/* ========================================================================= */
/** @name Code Location Utility:
 * this are utility macros to print information about where we are.
 * @note For some reason __func__ don't work correctly for sun's cc in inline
 * functions, so we don't use in SUN architecture */
/*  @{  */
#define __EG_PRINTLOCF__(/*@null@*/__F)  fprintf(((__F==((FILE*)0))?stderr:__F),", in %s (%s:%d)\n",__func__,__FILE__,__LINE__)
#define __EG_PRINTLOC__      __EG_PRINTLOCF__(stderr)
#define __EG_PRINTLOC2F__(/*@null@*/__F) fprintf(((__F==((FILE*)0))?stderr:__F),"in %s (%s:%d)\n",__func__,__FILE__,__LINE__)
#define __EG_PRINTLOC2__     __EG_PRINTLOC2F__(stderr)
/*  @}  */
/* ========================================================================= */
/** @brief if the given display level is over the given treshold, display the
 * given message and location information */
#define IFMESSAGE(__display,...) do{\
	if((__display)>0){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;}}while(0)

/* ========================================================================= */
/** @brief if return value is non-zero warn on screen */
#define WARNIF(__L__) do{\
	const int __Wval__= (__L__);\
	if(__Wval__){fprintf(stderr,"WARNING: In %s (%s:%d) "#__L__" = %d\n",__func__,__FILE__,__LINE__,__Wval__);}}while(0)
#if DEBUG>=1
/* ========================================================================= */
/** @brief This macro is to print error messages and to return with value one
 * from the current function, it also print the file and line where this 
 * happend, but the condition is looked only if the debug level is at least __L
 * */
#define EXITL(__L,__A,...) ({\
	if(__L<=DEBUG){\
	if(__A){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		_exit(1);}}})

/* ========================================================================= */
/** @brief This macro is to print error messages and to return with value one 
 * from the current function, it also print the file and line where this 
 * happend, but the condition is looked only if the debug level is at least __L 
 * */
#define TESTL(__L,__A,...) ({\
	if(__L<=DEBUG){\
	if(__A){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		return 1;}}})

/* ========================================================================= */
/** @brief this macro check if the value of a pointer is not bellow the first 
 * 64Kb, if so it return the given value */
#define PTRTEST(__PTR,__RVAL) {\
	if(__PTR) ADVTESTL(0,((size_t)(__PTR)) < (1U<<16),__RVAL, \
								 "%s=%p is not a valid pointer",\
									#__PTR, (void*)(__PTR));}

/* ========================================================================= */
/** @brief This macro is to print error messages and jump to the given point
 * in the code, it also print the file and line where this happend */
#define TESTGL(__L,__A,__B,...) ({\
	if((__L)<=DEBUG && (__A)){\
		fprintf(stderr,"ERROR: " __VA_ARGS__);\
		__EG_PRINTLOC__;\
		goto __B;}})

/* ========================================================================= */
/** @brief return value macro, if the value is non-zero, write to standard
 * error the returning code and where this happen */
#define EG_RETURN(__A) {\
	const int __RVAL__ = (__A);\
	if(__RVAL__){\
		fprintf(stderr,"rval %d",__RVAL__);\
		__EG_PRINTLOC__;}\
	return __RVAL__;}
/* ========================================================================= */
/** @brief This macro is to print error messages and jump to the given point
 * in the code, it also print the file and line where this happend */
#define TESTG(__A,__B,...) ({\
	if(__A){\
		fprintf(stderr,"ERROR: " __VA_ARGS__);\
		__EG_PRINTLOC__;\
		goto __B;}})
/* ========================================================================= */
/** @brief This macro is to print error messages and jump to the CLEANUP label
 * in the current function, it also print the file and line where this happend */
#define TESTGD(__A,...) ({\
	if(__A){\
		fprintf(stderr,"ERROR: " __VA_ARGS__);\
		__EG_PRINTLOC__;\
		goto CLEANUP;}})
/* ========================================================================= */
/** @brief This macro is to print error messages and to return with value one 
 * from the current function, it also print the file and line where this 
 * happend */
#define TEST(__A,...) ({\
	if(__A){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		return 1;}})

/* ========================================================================= */
/** @brief This macro print messages to the screen when the debug level is as 
 * big as the first parameter, if the debug level is zero we eliminate the 
 * code and reduce it to the empty instruction. */
#define MESSAGEF(__A,__F,...) ({\
	if(__A <= DEBUG ){\
		fprintf(((__F==0)?stderr:__F),__VA_ARGS__);\
		__EG_PRINTLOCF__(__F);}})

/* ========================================================================= */
/** @brief This macro print messages to the screen when the debug level is as 
 * big as the first parameter, if the debug level is zero we eliminate the 
 * code and reduce it to the empty instruction. */
#define MESSAGE(__A,...) ({\
	if(__A <= DEBUG ){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;}})
#if VERBOSE_LEVEL >= 1
/* ========================================================================= */
/** @brief This macro print messages to the screen when the verbose level is as 
 * big as the first parameter, if the verbose level is zero we eliminate the 
 * code and reduce it to the empty instruction. */
#define OUTPUT(__A,...) ({\
	if(__A <= VERBOSE_LEVEL ){\
		fprintf(stderr,__VA_ARGS__);}})
#else
#define OUTPUT(__A,...) ;
#endif

/* ========================================================================= */
/** @brief This macro print messages to the screen when the condition __A is 
 * true .if the debug level is one we don't print any warning message. if 
 * the debug level is zero we eliminate the code and reduce it to the empty 
 * instruction. */
#define WARNINGL(__L,__A,...) ({\
	if((__A)&&(DEBUG>=__L)){\
		fprintf(stderr,"WARNING: ");\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;}})

#else
#define TESTL(__L,__A,...) ;
#define EXITL(__L,__A,...) ;
#define TEST(__A,...) ;
#define TESTG(__A,__B,...) ;
#define TESTGL(__A,__B,...) ;
#define MESSAGE(__A,...) ;
#define MESSAGEF(__A,__F,...) ;
#define WARNINGL(__L,__A,...) ;
#define EG_RETURN(__A) do{return (__A);}while(0)
#define PTRTEST(__PTR,__RVAL) ;
#endif

/* ========================================================================= */
/** @brief This macro is to print error messages and to return with value one 
 * from the current function, it also print the file and line where this 
 * happend */
#define FTEST(__A,...) ({\
	if(__A){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		return 1;}})

/* ========================================================================= */
/** @brief This macro is to print error messages and to return with value one 
 * from the current function, it also print the file and line where this 
 * happend */
#define FTESTG(__A,__B,...) ({\
	if(__A){\
		fprintf(stderr,"ERROR: " __VA_ARGS__);\
		__EG_PRINTLOC__;\
		goto __B;}})

/* ========================================================================= */
/** @brief This macro print messages to the screen when the condition __A is 
 * true. */
#define WARNING(__A,...) ({if(__A){\
		fprintf(stderr,"WARNING: ");\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;}})

/* ========================================================================= */
/** @brief this macro test if a value is non zero, if it is it print where is 
 * it and exit EXIT_FAILURE. The idea is to use it to check return values of functions, 
 * and the calling function can't return a status, and then we are forced to 
 * exit. */
#define EXITRVAL(__A) ({\
	if(__A){\
		__EG_PRINTLOC2__;\
		exit(EXIT_FAILURE);}})

/* ========================================================================= */
/** @brief This macro is to print error messages and exit the program with 
 * code one from the current function, it also print the file and line where 
 * this happend */
#define EXIT(__A,...) ({if(__A){\
		fprintf(stderr,"EXIT: ");\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		exit(EXIT_FAILURE);}})

/* ========================================================================= */
/** @brief this macro test if a value is non zero, if it is it print where is 
 * it and return __B. The idea is to use it to check return values of functions 
 * */
#define ADVCHECKRVAL(__A,__B) ({\
	if(__A){\
		__EG_PRINTLOC2__;\
		return __B;}})

/* ========================================================================= */
/** @brief This macro test a condition '__A' when the debug level used at 
 * compile time is at least '__L'. If the condition is true, it print the 
 * message and return the '__RVAL' value. */
#define ADVTESTL(__L,__A,__RVAL,...) ({\
	if((DEBUG>=__L)&&(__A)){\
		fprintf(stderr,__VA_ARGS__);\
		__EG_PRINTLOC__;\
		return __RVAL;}})

/* ========================================================================= */
/** @brief this macro test if a value is non zero, if it is it print where is 
 * it and return 1. The idea is to use it to check return values of functions 
 * */
#define CHECKRVAL(__A) ({\
	if(__A){\
		__EG_PRINTLOC2__;\
		return __A;}})

/* ========================================================================= */
/** @brief, if a non-zero value is given as an argument, check the errno stored
 * in the system, print the related message, and return the non-zero given
 * parameter, otherwise, do nothing.
 * @param __value if non-zero check systems errors, and return this value
 * */
#define TESTERRNOIF(__value) do{\
	if(__value){\
		const int __EGserrno = errno;\
		fprintf(stderr,"failed with errno %d, %s\n",__EGserrno, strerror(__EGserrno));\
		__EG_PRINTLOC2__;\
		return __value;}}while(0)
/* ========================================================================= */
/** @brief this function, if the input is non zero, print a message of 
 * function, file and line and then goto the second parameter */
#define CHECKRVALG(__A,__B) do{if(__A){__EG_PRINTLOC2__;goto __B;}}while(0)

/* ========================================================================= */
/** @brief retrieve the data of type '__TYPE' in the structure '__DATA' that is 
 * located in the offset '__OFFS'. */
#define EGosGetData(__DATA,__OFFS,__TYPE) (*((__TYPE*)(((char*)__DATA)+__OFFS)))

/* ========================================================================= */
/** @brief set the data of type '__TYPE' in the structure '__DATA' that is 
 * located in the offset '__OFFS' to the value 'val'. */
#define EGosSetData(__DATA,__OFFS,__TYPE,val) (EGosGetData(__DATA,__OFFS,__TYPE)=val)

/* ========================================================================= */
/** @brief Defione copy functions, these functions
 * return copy of objects but with independent storage space, there are two
 * versions, one that require a memory pool from where to look for memory, and
 * another where we don't care about that.... the place from where the memory
 * was asked for depend on the function, se the function definition for 
 * details.
 * Note that if the is no more memory available the function should call
 * exit(EXIT_FAILURE).
 * This is only intended as a readibility help */
typedef void *(*EGcopy_f) (void *p);

/* ========================================================================= */
/** @brief Define a null copy function */
#define nullCopy ((EGcopy_f)0)

/* ========================================================================= */
/** @name Algorithms Return Status
 * Here we define some general status for algorithms, the exact meaning should
 * be sought in the actual algorithm definition, but the definitions here
 * provide a first overview of their meaning. */
/*  @{  */
/** @brief the algorithm finish successfully. */
#define EG_ALGSTAT_SUCCESS 0
/** @brief the algorithm could only partially finish */
#define EG_ALGSTAT_PARTIAL 1
/** @brief the algorithm stop because of some numerical problem */
#define EG_ALGSTAT_NUMERROR 2
/** @brief the algorithm stop because of some unforeseen error */
#define EG_ALGSTAT_ERROR 3
/* @} */

/* ========================================================================= */
/** @name Mathematical Constants 
 * Here we define some mathematical constants needed in some parts of the code
 * that are of general use */
/* @{ */
/** @brief definition of \f$\pi\f$ as a constant, suitable for quad-IEEE
 * operations. */
#define EG_M_PI 3.1415926535897932384626433832795029L
/* @} */

/* ========================================================================= */
/** @brief Call macro. The idea is to replace the following call:
 * rval = myfunction(mypar);
 * CHECKRVALG(rval,mygoto);
 * with the call
 * EGcall(rval,mygoto,myfunction(mypar));
 * this should help simplify calls and clean-up the code
 * @note each parameter is evaluated once, thus ensuring correct evaluation of
 * parameters, even if they are an expresion. */
#define EGcall(__rval__,__cleanup__,__myfunc__) do{const int __EGrval__=__myfunc__;(__rval__)=__EGrval__;TESTG(__EGrval__,__cleanup__,"Function " #__myfunc__ " failed with code %d ",__EGrval__);}while(0)
/* ========================================================================= */
/** @brief call macro with default arguments, it assumes that rval is an
 * integer variable to be used to store return value, and CLEANUP is a valid
 * label for an exit point in the code */
#define EGcallD(__myfunc2__) EGcall(rval,CLEANUP,__myfunc2__)
/* ========================================================================= */
/** @brief Display information about the library and the running process */
void EGlib_info(void);
/* ========================================================================= */
/** @brief print versioning info of the library */
void EGlib_version(void);
/* ========================================================================= */
/** @brief needed global jump-control variable */
extern jmp_buf __EGljmp;
/* ========================================================================= */
/** @brief a generic signal handler, it can handle SIGXCPU, SIGINT and SIGSEGV
 * signals by displaying a proper indication to stderr; note that a call to
 * #EGsigSetjmp should be performed at the very beggining of the main function
 * for this to be robust.
 * When receiving a SIGXCPU, SIGINT signal, the function report the signal and jump-back
 * to the setjmp position.
 * When receiving a SIGINT signal, the function report the signal and jump-back
 * @param s the signal number received
 * */
void EGsighandler(int s);
/* ========================================================================= */
/** @brief use #EGsighandler for SIGXCPU, SIGINT and SIGSEGV as signal handler.
 * */
void __EGsigSetSignal(void);
/* ========================================================================= */
/** @brief set the jump point, and the sginal handler.
 * it must receive a label where to jump, and where
 * to save the returning status, typically this will be the clean-up section 
 * of the main function and the status variable in the mian program.
 * @param __status__ the returning status of the call, zero when set, non-zero
 * when comming back from a long-jump.
 * @param __LABEL__ where to jump when returning from a long-jump. */
#define EGsigSet(__status__,__LABEL__) do{MESSAGE(0,"setjmp here");if((__status__=setjmp(__EGljmp))){IFMESSAGE(1,"Back from signal handler, status %d",__status__); goto __LABEL__;}__EGsigSetSignal();}while(0)
/* ========================================================================= */
/** @brief set memory and run-time limits (soft and hard); if the current
 * limits are bellow the porposed limits, it will warn on screen, but will not
 * fail. Note that this function will set RLMIT_CORE to zero.
 * @param max_rtime maximum running time.
 * @param mem_limit maximum memory allowed for the process, this include
 * RLIMIT_AS and RLIMIT_DATA.
 * */
void EGsetLimits(double max_rtime, unsigned long memlimit);
/* ========================================================================= */
/** @brief given a constant array-base, an element size, and a position, 
 * return the pointer to the appropiate element */
#define EGaGetCElem(__EGabase__,__EGasz__,__EGaelem__) ((const char*)(((const char*)(__EGabase__))+((const size_t)(__EGasz__))*((const size_t)(__EGaelem__))))
/* ========================================================================= */
/** @brief given an array-base, an element size, and a position, return the
 * pointer to the appropiate element */
#define EGaGetElem(__EGabase__,__EGasz__,__EGaelem__) ((char*)(((char*)(__EGabase__))+((const size_t)(__EGasz__))*((const size_t)(__EGaelem__))))
/* ========================================================================= */
/** @brief given an array base, an element size, and a pointer, return the
 * position of the element on the array */
#define EGaGetPos(__EGabase__,__EGasz__,__EGaptr__) ((((char*)(__EGaptr__))-((char*)(__EGabase__)))/((const size_t)(__EGasz__)))
/* ========================================================================= */
/**  @}  */
/* end of eg_macros.h */
#endif
