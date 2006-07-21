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
#ifndef EZRETS_FWD_H
#define EZRETS_FWD_H

#include <boost/shared_ptr.hpp>
#include <vector>

namespace odbcrets
{
class EzLogger;
typedef boost::shared_ptr<EzLogger> EzLoggerPtr;

class ResultSet;
typedef boost::shared_ptr<ResultSet> ResultSetPtr;

class RetsSTMT;
class RetsENV;
class RetsDBC;

class MetadataView;
typedef boost::shared_ptr<MetadataView> MetadataViewPtr;

class Query;
typedef boost::shared_ptr<Query> QueryPtr;

class SqlStateException;

class DataStreamInfo;

class DataTranslator;
typedef boost::shared_ptr<DataTranslator> DataTranslatorPtr;

class AppRowDesc;
class AppParamDesc;

class Column;
typedef boost::shared_ptr<Column> ColumnPtr;
typedef std::vector<ColumnPtr> ColumnVector;
typedef boost::shared_ptr<ColumnVector> ColumnVectorPtr;

class Error;
typedef boost::shared_ptr<Error> ErrorPtr;
}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
