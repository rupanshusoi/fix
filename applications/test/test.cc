extern "C" {
#include "../util/fixpoint_util.h"
}

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "ro_mem_0_to_program_memory" ) ) );

#include <string>
#include <vector>
#include <cereal/archives/binary.hpp>

using namespace std;

vector<string> lex(const string& src)
{
  vector<string> tokens;
  auto it = src.begin();
  int counter = 0;
  while (1)
  {
    if (it == src.end()) break;
    else if (*it == '(' || *it == ')')
    {
      string tmp(1, *it);
      tokens.push_back(tmp);
      it++;
      counter++;
    }
    else
    {
      auto idx = src.find(' ', it - src.begin());      
      tokens.push_back(src.substr(it - src.begin(), idx));
      // it += idx + (it - src.begin());
      it += idx - counter;
      counter = idx;
    }
  } 
  return tokens;
}

void test_lex(vector<string>& tokens)
{
  for (auto& token: tokens)
  {
    fixpoint_unsafe_io(token.c_str(), token.size());
    fixpoint_unsafe_io(" & ", 3);
  }
}

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: arg
   encode[3]: addblob.wasm
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);
  // attach_blob_ro_mem_0(get_ro_table_0(2));
  // int arg = get_i32_ro_mem_0(0);

  attach_blob_ro_mem_0(get_ro_table_0(2));
  size_t len = byte_size_ro_mem_0();
  char* buf = (char*)malloc((unsigned long) len);
  ro_mem_0_to_program_memory((int32_t)buf, 0, len);
  fixpoint_unsafe_io(buf, 100);

  // char _s[20];
  // sprintf(_s, "Fib %d", arg);
  // string s = string(_s);
  // fixpoint_unsafe_io(s.c_str(), s.length());

  // if (arg == 0 || arg == 1)
  //   return create_blob_i32(1);

  // // Add(Fib(n - 1), Fib(n - 2))
  // grow_rw_table_0(4, get_ro_table_0(0));
  // set_rw_table_0(0, get_ro_table_0(0));
  // set_rw_table_0(1, get_ro_table_0(3));

  // grow_rw_table_1(4, get_ro_table_0(0));
  // set_rw_table_1(0, get_ro_table_0(0));
  // set_rw_table_1(1, get_ro_table_0(1));
  // set_rw_table_1(2, create_blob_i32(arg - 1));
  // set_rw_table_1(3, get_ro_table_0(3));

  // grow_rw_table_2(4, get_ro_table_0(0));
  // set_rw_table_2(0, get_ro_table_0(0));
  // set_rw_table_2(1, get_ro_table_0(1));
  // set_rw_table_2(2, create_blob_i32(arg - 2));
  // set_rw_table_2(3, get_ro_table_0(3));

  // set_rw_table_0(2, create_thunk(create_tree_rw_table_1(4)));
  // set_rw_table_0(3, create_thunk(create_tree_rw_table_2(4)));

  return create_blob_i32(88);
}
