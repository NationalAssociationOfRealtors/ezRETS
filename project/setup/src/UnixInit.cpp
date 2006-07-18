#include "wx/init.h"

namespace odbcrets
{
void initEzSetup(void) __attribute__((constructor));
void cleanEzSetup(void) __attribute__((destructor));
}

void odbcrets::initEzSetup(void)
{
    int argc = 0;
    char **argv = NULL;
    wxEntryStart(argc, argv);
}

void odbcrets::cleanEzSetup(void)
{
    wxEntryCleanup();
}
