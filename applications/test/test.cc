extern "C" {
#include "../util/fixpoint_util.h"
}
#include <string>

using namespace std;

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: count (for now)
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  fixpoint_unsafe_io("Applying!", 20);

  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  int count = get_i32_ro_mem_0(0);

  if (count)
  {
    grow_rw_table_0(3, get_ro_table_0(0));
    set_rw_table_0(0, get_ro_table_0(0));
    set_rw_table_0(1, get_ro_table_0(1));
    set_rw_table_0(2, create_blob_i32(count - 1));
    return create_thunk(create_tree_rw_table_0(3));
  }

  return create_blob_i32(0);
}
