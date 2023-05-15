(module
  (import "fixpoint_storage" "rw_mem_0" (memory $rw_mem_0 0))
  (import "fixpoint_storage" "rw_mem_1" (memory $rw_mem_1 0))
  (import "fixpoint_storage" "rw_mem_2" (memory $rw_mem_2 0))
  (import "fixpoint_storage" "ro_mem_0" (memory $ro_mem_0 0))
  (import "fixpoint_storage" "ro_mem_1" (memory $ro_mem_1 0))
  (import "fixpoint_storage" "ro_mem_2" (memory $ro_mem_2 0))
  (import "fixpoint_storage" "ro_mem_3" (memory $ro_mem_3 0)) 

  (import "test" "memory" (memory $program_mem 0))

  (func (export "ro_mem_0_to_program_memory") (param $program_offset i32) (param $ro_offset i32) (param $len i32)
    (memory.copy $ro_mem_0 $program_mem
    (local.get $program_offset)
    (local.get $ro_offset)
    (local.get $len))
  )
  (func (export "program_memory_to_rw_0") (param $rw_offset i32) (param $program_offset i32) (param $len i32)
    (memory.copy $program_mem $rw_mem_0
    (local.get $rw_offset)
    (local.get $program_offset)
    (local.get $len))
  )
)

