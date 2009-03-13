/*
 * Copyright (C) 2006-2008 National Association of REALTORS(R)
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

#include "StatisticsMetadataQuery.h"
#include "ResultSet.h"
#include "DataTranslator.h"

using namespace odbcrets;

StatisticsMetadataQuery::StatisticsMetadataQuery(RetsSTMT* stmt)
    : Query(stmt)
{
}

SQLRETURN StatisticsMetadataQuery::execute()
{
    return SQL_SUCCESS;
}

void StatisticsMetadataQuery::prepareResultSet()
{
    DataTranslatorSPtr dt(DataTranslator::factory());
    mResultSet.reset(newResultSet(dt));
    
    mResultSet->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("NON_UNIQUE", SQL_SMALLINT);
    mResultSet->addColumn("INDEX_QUALIFIER", SQL_VARCHAR);
    mResultSet->addColumn("INDEX_NAME", SQL_VARCHAR);
    mResultSet->addColumn("TYPE", SQL_SMALLINT);
    mResultSet->addColumn("ORDINAL_POSITION", SQL_SMALLINT);
    mResultSet->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultSet->addColumn("ASC_OR_DESC", SQL_CHAR);
    mResultSet->addColumn("CARDINALITY", SQL_INTEGER);
    mResultSet->addColumn("PAGES", SQL_INTEGER);
    mResultSet->addColumn("FILTER_CONDITION", SQL_VARCHAR);
}

std::ostream & StatisticsMetadataQuery::print(std::ostream & out) const
{
    return out;
}
