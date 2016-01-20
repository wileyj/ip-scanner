//regex.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <regex.h>
#include "global.h"

using namespace std;


int string_regex(std::string str, std::string pattern){
  int match = 0;
  regex_t reg;
  regmatch_t matches[1];
  regcomp(&reg,pattern.c_str(),REG_EXTENDED|REG_ICASE);
  if (regexec(&reg,str.c_str(),1,matches,0)==0) {
    match = 1;
  }
  regfree(&reg);
  str = "";
  pattern = "";
  return match ;
}
