#ifndef PTR_MADNESS_H
#define PTR_MADNESS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if SIZEOF_VOIDP == 8
#define PTRINT unsigned long long
#else
#define PTRINT unsigned int
#endif

/*
 *  The following copyright covers the code below based on templates.
 *  I think we're going to replace it with the code above.  (Which
 *  needs to have the standard NAR header on it.)
 *
 *  Copyright (C) 2007 David Terrell, no rights reserved.
 */
// namespace dbt
// {

// template<int size>
// struct ptrsize
// {
// };

// template<> struct ptrsize<4>
// {
//         typedef int int_t;
//         typedef unsigned int uint_t;
// };

// template<> struct ptrsize<8>
// {
//         typedef long long int_t;
//         typedef unsigned long long uint_t;
// };

// typedef ptrsize<sizeof(void*)>::int_t ptrint_t;
// typedef ptrsize<sizeof(void*)>::uint_t ptruint_t;

// }

#endif /* PTR_MADNESS_H */
/* Local Variables: */
/* mode: c++ */
/* End: */
