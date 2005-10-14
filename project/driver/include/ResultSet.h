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
#ifndef RESULTSET_H
#define RESULTSET_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include "librets.h"
#include "DataTranslator.h"
#include "Column.h"
#include "ezretsfwd.h"

namespace odbcrets
{
class ResultSet
{
  public:

    ResultSet(RetsSTMT* stmt);
    
    int rowCount();
    int columnCount();
    bool isEmpty();

    bool hasNext();

    std::string getColumnName(int col);

    void bindColumn(int col, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
                    SQLLEN BufferLength, SQLLEN *StrLenOrInd);

    void unbindColumns();

    void processNextRow();

    DataTranslator& getDataTranslator();

    EzLoggerPtr getLogger();

    void addColumn(std::string name, SQLSMALLINT DefaultType);
    void addColumn(std::string name, librets::MetadataTable* table);
    void addRow(librets::StringVectorPtr row);

    ColumnPtr getColumn(int col);

    ColumnVectorPtr getColumns();

    void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd);

    void setReportedRowCount(int count);

    RetsSTMT* getStmt() const;

  private:
    typedef std::vector<librets::StringVectorPtr> StringVectorVector;

    RetsSTMT* mStmt;
    bool mGotFirst;
    ColumnVectorPtr mColumns;
    StringVectorVector mResults;
    StringVectorVector::iterator mResultIterator;
    int mReportedRowCount;
};

typedef boost::shared_ptr<ResultSet> ResultSetPtr;

}


#endif /* RETSSTMTRESULTS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
