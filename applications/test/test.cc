extern "C" {
#include "../util/fixpoint_util.h"
}

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "program_memory_to_rw_0" ) ) );

#include <string>
#include <vector>
#include <cereal/archives/binary.hpp>

using namespace std;

struct State
{
  int x;

  template<Archive ar>
  void serialize(Archive & archive)
  {
    archive( x );
  }
};

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: serialized state
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);

  attach_blob_ro_mem_0(get_ro_table_0(2));
  size_t len = byte_size_ro_mem_0();
  char* buf = (char*)malloc((unsigned long) len);
  ro_mem_0_to_program_memory((int32_t)buf, 0, len);

  fixpoint_unsafe_io(buf, len);

  if (1)
  {
    const char* msg = "Testing!";
    grow_rw_table_0(3, get_ro_table_0(0));
    set_rw_table_0(0, get_ro_table_0(0));
    set_rw_table_0(1, get_ro_table_0(1));

    grow_rw_mem_0_pages(1);
    program_memory_to_rw_0(0, (int32_t)msg, 10);
    set_rw_table_0(2, create_blob_rw_mem_0(10));
    // set_rw_table_0(2, create_blob_i32(123));
    return create_thunk(create_tree_rw_table_0(3));
  }
  else
  {
    return create_blob_i32(22);
  }

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

  // return create_blob_i32(88);
}
