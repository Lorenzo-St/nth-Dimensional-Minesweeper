/*********************************************************************
 * @file   cexprString.h
 * @brief  
 * 
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#pragma once

class cexprString
{
public:
  constexpr cexprString(const char* s) : string(s)
  {
    const char* l = s;
    while (*l != 0)
    {
      ++len;
      ++l;
    }
  }
  constexpr int Length() { return len; };
private:
  const char* string;
  int len = 0;
};