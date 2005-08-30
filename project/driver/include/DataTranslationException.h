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
#ifndef DATA_TRANSLATION_EXCEPTION
#define DATA_TRANSLATION_EXCEPTION

#include <exception>
#include <string>

namespace odbcrets
{
class DataTranslationException : public std::exception
{
  public:
    DataTranslationException(std::string message);

    virtual ~DataTranslationException() throw();

    virtual std::string GetMessage() const throw();

    virtual const char* what() const throw();

  private:
    std::string mMessage;
};

}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */

