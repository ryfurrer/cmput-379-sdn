#include "util.h"

void trimWhitespace(string & cmd) {
  //trim whitespace
  unsigned int i = 0;
  while (i < cmd.length() && !std::isalpha(cmd.at(i)))
    i++;
  unsigned int j = cmd.length() - 1;
  while (j > i && !std::isalpha(cmd.at(j)))
    j--;
  cmd = cmd.substr(i, j - i + 1);
}
