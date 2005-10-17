/*
 * Copyright (C) 2005 National Association of REALTORS(R)
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
#ifdef __WIN__
#include "DllMain.h"
#include <wx/wx.h>
#include <wx/msw/private.h>

static void initWx(HINSTANCE instance)
{
    int argc = 0;
    char ** argv = NULL;
    wxSetInstance(instance);
    wxEntryStart(argc, argv);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID lpvReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            initWx(instance);
            break;

        case DLL_PROCESS_DETACH:
            wxEntryCleanup();
            break;
    }
    return TRUE;
}
#endif
