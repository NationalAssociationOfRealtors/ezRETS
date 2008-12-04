/*
 * Copyright (C) 2005-2008 National Association of REALTORS(R)
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "DataTranslator.h"
#include "RetsSTMT.h"
#include "RetsDBC.h"
#include "DataSource.h"

using namespace odbcrets;

DataTranslator* DataTranslator::factory(RetsSTMT* stmt)
{
    DataTranslator* dataTranslator;

    if (stmt)
    {
        if (stmt->mDbc->mDataSource.GetIgnoreMetadataType())
        {
            dataTranslator = new CharOnlyDataTranslator();
        }
        else
        {
            dataTranslator = new NativeDataTranslator(
                stmt->mDbc->mDataSource.GetTreatDecimalAsString() ?
                NativeDataTranslator::DECIMAL_AS_STRING : 0);
        }
    }
    else
    {
        dataTranslator = new NativeDataTranslator();
    }

    return dataTranslator;
}
