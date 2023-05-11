extern "C" {
#include "../util/fixpoint_util.h"
}
#include <string>
#include <format>

using namespace std;

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: arg
   encode[3]: addblob.wasm
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  int arg = get_i32_ro_mem_0(0);

  char _s[20];
  sprintf(_s, "Fix %d", arg);
  string s = string(_s);
  fixpoint_unsafe_io(s.c_str(), s.length());

  if (arg == 0 || arg == 1)
    return create_blob_i32(1);

  // Add(Fib(n - 1), Fib(n - 2))
  grow_rw_table_0(4, get_ro_table_0(0));
  set_rw_table_0(0, get_ro_table_0(0));
  set_rw_table_0(1, get_ro_table_0(3));

  grow_rw_table_1(4, get_ro_table_0(0));
  set_rw_table_1(0, get_ro_table_0(0));
  set_rw_table_1(1, get_ro_table_0(1));
  set_rw_table_1(2, create_blob_i32(arg - 1));
  set_rw_table_1(3, get_ro_table_0(3));

  grow_rw_table_2(4, get_ro_table_0(0));
  set_rw_table_2(0, get_ro_table_0(0));
  set_rw_table_2(1, get_ro_table_0(1));
  set_rw_table_2(2, create_blob_i32(arg - 2));
  set_rw_table_2(3, get_ro_table_0(3));

  set_rw_table_0(2, create_thunk(create_tree_rw_table_1(4)));
  set_rw_table_0(3, create_thunk(create_tree_rw_table_2(4)));

  return create_thunk(create_tree_rw_table_0(4));
}
