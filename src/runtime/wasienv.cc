#include <iostream>

#include "timing_helper.hh"
#include "wasienv.hh"
#include "runtimestorage.hh"

using namespace std;

namespace wasi
{
  void attach_input( uint32_t input_index, uint32_t mem_index )
  {
    auto & invocation = id_to_inv_.at( invocation_id_ );
    invocation.attach_input( input_index, mem_index );
  }

  void attach_output( uint32_t output_index, uint32_t mem_index, uint32_t output_type )
  {
    auto & invocation = id_to_inv_.at( invocation_id_ );
    invocation.attach_output( output_index, mem_index, output_type );
  }
   
  uint32_t get_int( uint32_t mem_index, uint32_t ofst )
  {
    auto & invocation = id_to_inv_.at( invocation_id_ );
    return invocation.get_int( mem_index, ofst );
  }

  void store_int( uint32_t mem_index, uint32_t ofst )
  {
    auto & invocation = id_to_inv_.at( invocation_id_ );
    invocation.store_int( mem_index, ofst );
  }
}
