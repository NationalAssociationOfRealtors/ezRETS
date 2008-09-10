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

#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

#include "RegisterFuncs.h"

using std::endl;
using std::cout;

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("install,i", "install the ezRETS driver")
        ("remove,r", "remove the ezRETS driver")
        ("destructive,d", "remove ezRETS destructively");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch (std::exception& e)
    {
        cout << "ERROR: " << e.what() << endl;
        cout << desc;
        exit(1);
    }
    po::notify(vm);
        

    bool ezInstalled = isEzInstalled();

    if (vm.count("help"))
    {
        cout << desc;
    }

    if (vm.count("install"))
    {
        if (ezInstalled)
        {
            cout << "ezRETS already installed" << endl;
        }
        else
        {
            registr();
        }
    }

    if (vm.count("remove"))
    {
        if (ezInstalled)
        {
            if (vm.count("destructive"))
            {
                unregister(true);
            }
            else
            {
                unregister(false);
            }
        }
        else
        {
            cout << "ezRETS not installed" << endl;
        }
    }

    exit(0);
}
