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

#include <iostream>
#include "DefinitionGenerator.h"
#include "DataSource.h"
#include "librets/RetsSession.h"

using std::cout;
using std::endl;
using odbcrets::DataSource;

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        cout << "Dammit!" << endl;
        exit(1);
    }
    
    DataSource ds(argv[1]);
    ds.MergeFromIni();

    cout << ds << endl;

    librets::RetsSessionPtr session = ds.CreateRetsSession();

    if (!ds.RetsLogin(session))
    {
        cout << "Couldn't log in!" << endl;
    }

    librets::RetsMetadata* metadata = session->GetMetadata();
    
    ezhelper::DefinitionGenerator foo(ds.GetStandardNames(), metadata);

    string result = foo.createHTML();
}
