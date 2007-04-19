/*
 * Copyright (C) 2007 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */

#ifndef REGISTERFUNCS_H
#define REGISTERFUNCS_H

#include "ezrets.h"

#ifdef HAVE_IODBC
#include <iodbcinst.h>
#else
#include <odbcinst.h>
#endif

#ifdef __WIN__
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <string>
#include <vector>

typedef std::vector<std::string> StrVector;

DLLEXPORT StrVector drivers2vector(LPCSTR drivers);
DLLEXPORT bool isEzInstalled();
DLLEXPORT void unregister(bool remove_dsn=false);
DLLEXPORT void registr();

#endif /* REGISTERFUNCS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
