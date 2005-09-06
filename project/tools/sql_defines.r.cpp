#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <map>
#include <boost/lexical_cast.hpp>

using namespace std;
namespace b = boost;

typedef map<string, int> DefineMap;

void printConstant(string constant, const DefineMap & defines)
{
  DefineMap::const_iterator i = defines.find(constant);
  if (i == defines.end())
  {
    cout << constant << ": Not found" << endl;
  }

  cout << constant << ": " << i->second << endl;
}

void printConstants(int value, const DefineMap & defines)
{
  DefineMap::const_iterator i;
  for (i = defines.begin(); i != defines.end(); i++)
  {
    if (i->second != value)
      continue;
    cout << i->first << ": " << i->second << endl;
  }
}

void printAll(const DefineMap & defines)
{
  DefineMap::const_iterator i;
  for (i = defines.begin(); i != defines.end(); i++)
  {
    cout << i->first << ": " << i->second << endl;
  }
}

int main(int argc, const char * argv[])
{
  DefineMap defines;
  
  <%- for define in defines -%>
  defines["<%= define %>"] = <%= define %>;
  <%- end -%>

  if (argc == 2)
  {
    try
    {
      printConstants(b::lexical_cast<int>(argv[1]), defines);
    }
    catch (b::bad_lexical_cast &)
    {
      printConstant(argv[1], defines);
    }
  }
  else
  {
    printAll(defines);
  }

  return 0;
}
