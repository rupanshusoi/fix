#include "c-flatware.h"
#include "api.h"
#include "asm-flatware.h"
#include "filesys.h"

#include <stdarg.h>
#include <stdbool.h>

typedef char __attribute__( ( address_space( 10 ) ) ) * externref;

// XXX make sure this is false before committing
static bool trace = false;
static int32_t trace_offset = 0;

// for each value, first pass the width (T32 or T64), then pass the value
// to specify the end, pass TEND
// this is important because we need to know the size of each value when getting it with `va_arg`
// e.g. TRACE( T32, 0xdeadbeef, T64, 0xdeadeeffeedface, TEND )
//      TRACE( TEND ) for no values passed
#define TRACE( ... )                                                                                               \
  if ( trace )                                                                                                     \
  print_trace( __FUNCTION__, __VA_ARGS__ )

typedef enum trace_val_size
{
  TEND,
  T32,
  T64
} trace_val_size;

// must be <= 16
#define TRACE_RADIX 10

static void write_trace( const char* str, int32_t len )
{
  flatware_memory_to_rw_2( trace_offset, str, len );
  trace_offset += len;
}

static int32_t strlen( const char* str )
{
  int32_t len = 0;
  while ( *str ) {
    len++;
    str++;
  }
  return len;
}

static void write_int( uint64_t val )
{
  const char nums[] = "0123456789abcdef";
  uint64_t div = 1;

  while ( val / div >= TRACE_RADIX ) {
    div *= TRACE_RADIX;
  }

  while ( div != 1 ) {
    write_trace( &nums[val / div], 1 );
    val %= div;
    div /= TRACE_RADIX;
  }
  write_trace( &nums[val], 1 );
}

static void print_trace( const char* f_name, ... )
{
  const char chars[] = "\n( ), -";
  va_list vargs;
  trace_val_size v;

  // newline
  write_trace( &chars[0], 1 );

  // function name
  write_trace( f_name, strlen( f_name ) );

  // opening parenthesis
  write_trace( &chars[1], 2 );

  va_start( vargs, f_name );

  // passed values
  v = va_arg( vargs, trace_val_size );
  while ( 1 ) {
    int64_t val;

    if ( v == T32 )
      val = va_arg( vargs, int32_t );
    else if ( v == T64 )
      val = va_arg( vargs, int64_t );
    else
      break;

    if ( val < 0 ) {
      write_trace( &chars[6], 1 );
      val = -val;
    }
    write_int( (uint64_t)val );

    v = va_arg( vargs, trace_val_size );
    if ( v != TEND ) {
      write_trace( &chars[4], 2 );
    }
  }

  va_end( vargs );

  // closing parenthesis
  write_trace( &chars[2], 2 );
}

typedef struct filedesc
{
  int32_t offset;
  int32_t size;
  bool open;
  char pad[3];
} filedesc;

enum
{
  STDIN,
  STDOUT,
  STDERR,
  WORKINGDIR
};

static filedesc fds[16] = {
  { .open = true, .size = -1, .offset = 0 }, // STDIN
  { .open = true, .size = -1, .offset = 0 }, // STDOUT
  { .open = true, .size = -1, .offset = 0 }, // STDERR
  { .open = true, .size = -1, .offset = 0 }, // WORKINGDIR
};

extern void memory_copy_program( int32_t, const void*, int32_t )
  __attribute( ( import_module( "asm-flatware" ), import_name( "memory_copy_program" ) ) );
extern int32_t get_program_i32( int32_t )
  __attribute( ( import_module( "asm-flatware" ), import_name( "get_program_i32" ) ) );
extern void set_program_i32( int32_t, int32_t )
  __attribute( ( import_module( "asm-flatware" ), import_name( "set_program_i32" ) ) );

extern void run_start( void ) __attribute( ( import_module( "asm-flatware" ), import_name( "run-start" ) ) );
__attribute( ( noreturn ) ) void flatware_exit( void )
  __attribute( ( import_module( "asm-flatware" ), import_name( "exit" ) ) );

externref fixpoint_apply( externref encode ) __attribute( ( export_name( "_fixpoint_apply" ) ) );

_Noreturn void proc_exit( int32_t rval )
{
  TRACE( T32, rval, TEND );
  set_rw_table_0( 0, create_blob_i32( rval ) );
  flatware_exit();
}

int32_t fd_close( int32_t fd )
{
  TRACE( T32, fd, TEND );
  return 0;
}

int32_t fd_fdstat_get( int32_t fd, int32_t retptr0 )
{
  TRACE( T32, fd, T32, retptr0, TEND );
  return 0;
}

int32_t fd_seek( int32_t fd, int64_t offset, int32_t whence, int32_t retptr0 )
{
  TRACE( T32, fd, T64, offset, T32, whence, T32, retptr0, TEND );
  return 0;
}

// fd_read copies from memory (ro mem 1) to program memory
// need to match signature from wasi-libc
int32_t fd_read( int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0 )
{
  int32_t iobuf_offset, iobuf_len;
  int32_t file_remaining;
  int32_t size_to_read;
  int32_t total_read = 0;

  TRACE( T32, fd, T32, iovs, T32, iovs_len, T32, retptr0, TEND );

  if ( fd != 4 )
    return __WASI_ERRNO_BADF;

  for ( int32_t i = 0; i < iovs_len; i++ ) {
    file_remaining = fds[fd].size - fds[fd].offset;
    if ( file_remaining == 0 )
      break;

    iobuf_offset = get_program_i32( iovs + i * 8 );
    iobuf_len = get_program_i32( iovs + 4 + i * 8 );

    size_to_read = iobuf_len < file_remaining ? iobuf_len : file_remaining;
    ro_1_to_program_memory( iobuf_offset, fds[fd].offset, size_to_read );
    fds[fd].offset += size_to_read;
    total_read += size_to_read;
  }

  memory_copy_program( retptr0, &total_read, sizeof( total_read ) );
  return 0;
}

int32_t fd_write( int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0 )
{
  int32_t iobuf_offset, iobuf_len;
  int32_t total_written = 0;

  TRACE( T32, fd, T32, iovs, T32, iovs_len, T32, retptr0, TEND );

  if ( fd != STDOUT )
    return __WASI_ERRNO_BADF;

  for ( int32_t i = 0; i < iovs_len; i++ ) {
    iobuf_offset = get_program_i32( iovs + i * 8 );
    iobuf_len = get_program_i32( iovs + 4 + i * 8 );

    program_memory_to_rw_1( fds[STDOUT].offset, iobuf_offset, iobuf_len );
    fds[STDOUT].offset += iobuf_len;
    total_written += iobuf_len;
  }

  memory_copy_program( retptr0, &total_written, sizeof( total_written ) );
  return 0;
}

int32_t fd_fdstat_set_flags( int32_t fd, int32_t fdflags )
{
  TRACE( T32, fd, T32, fdflags, TEND );
  return 0;
}

/**
 * Collect information of preopened file descriptors. Called with fd >= 3 (0, 1, 2 reserved for stdin, stdout and
 * stderror). Return BADF if fd is not preopened.
 *
 * working directory preopened at fd = 3
 */
int32_t fd_prestat_get( int32_t fd, int32_t retptr0 )
{
  TRACE( T32, fd, T32, retptr0, TEND );

  // STDIN, STDOUT, STDERR
  if ( fd < 3 ) {
    return __WASI_ERRNO_PERM;
  }

  // file descriptors not preopened
  if ( fd > 3 ) {
    return __WASI_ERRNO_BADF;
  }

  // retptr offset in program memory where struct should be stored
  // TODO: fd == 3: working directory

  if ( fd == WORKINGDIR ) {
    __wasi_prestat_t ps;
    ps.tag = __WASI_PREOPENTYPE_DIR;
    ps.u.dir.pr_name_len = 2;

    memory_copy_program( retptr0, &ps, sizeof( ps ) );
    return 0;
  }

  return __WASI_ERRNO_BADF;
}

int32_t fd_prestat_dir_name( int32_t fd, int32_t path, int32_t path_len )
{
  char str[] = ".";

  TRACE( T32, fd, T32, path, T32, path_len, TEND );

  if ( fd != WORKINGDIR ) {
    return __WASI_ERRNO_PERM;
  }

  memory_copy_program( path, str, path_len );
  return 0;
}

int32_t fd_advise( int32_t fd, int64_t offset, int64_t len, int32_t advice )
{
  TRACE( T32, fd, T64, offset, T64, len, T32, advice, TEND );
  return 0;
}

int32_t fd_allocate( int32_t fd, int64_t offset, int64_t len )
{
  TRACE( T32, fd, T64, offset, T64, len, TEND );
  return 0;
}

int32_t fd_datasync( int32_t fd )
{
  TRACE( T32, fd, TEND );
  return 0;
}

int32_t fd_filestat_get( int32_t fd, int32_t retptr0 )
{
  TRACE( T32, fd, T32, retptr0, TEND );
  return 0;
}

int32_t fd_filestat_set_size( int32_t fd, int64_t size )
{
  TRACE( T32, fd, T64, size, TEND );
  return 0;
}

int32_t fd_filestat_set_times( int32_t fd, int64_t atim, int64_t mtim, int32_t fst_flags )
{
  TRACE( T32, fd, T64, atim, T64, mtim, T32, fst_flags, TEND );
  return 0;
}

int32_t fd_pread( int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset, int32_t retptr0 )
{
  TRACE( T32, fd, T32, iovs, T32, iovs_len, T64, offset, T32, retptr0, TEND );
  return 0;
}

int32_t fd_pwrite( int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset, int32_t retptr0 )
{
  TRACE( T32, fd, T32, iovs, T32, iovs_len, T64, offset, T32, retptr0, TEND );
  return 0;
}

int32_t fd_readdir( int32_t fd, int32_t buf, int32_t buf_len, int64_t cookie, int32_t retptr0 )
{
  TRACE( T32, fd, T32, buf, T32, buf_len, T64, cookie, T32, retptr0, TEND );
  return 0;
}

int32_t fd_sync( int32_t fd )
{
  TRACE( T32, fd, TEND );
  return 0;
}

int32_t fd_tell( int32_t fd, int32_t retptr0 )
{
  TRACE( T32, fd, T32, retptr0, TEND );
  return 0;
}

int32_t path_create_directory( int32_t fd, int32_t path, int32_t path_len )
{
  TRACE( T32, fd, T32, path, T32, path_len, TEND );
  return 0;
}

int32_t path_filestat_get( int32_t fd, int32_t flags, int32_t path, int32_t path_len, int32_t retptr0 )
{
  TRACE( T32, fd, T32, flags, T32, path, T32, path_len, T32, retptr0, TEND );
  return 0;
}

int32_t path_link( int32_t old_fd,
                   int32_t old_flags,
                   int32_t old_path,
                   int32_t old_path_len,
                   int32_t new_fd,
                   int32_t new_path,
                   int32_t new_path_len )
{
  TRACE( T32,
         old_fd,
         T32,
         old_flags,
         T32,
         old_path,
         T32,
         old_path_len,
         T32,
         new_fd,
         T32,
         new_path,
         T32,
         new_path_len,
         TEND );
  return 0;
}

int32_t path_readlink( int32_t fd, int32_t path, int32_t path_len, int32_t buf, int32_t buf_len, int32_t retptr0 )
{
  TRACE( T32, fd, T32, path, T32, path_len, T32, buf, T32, buf_len, T32, retptr0, TEND );
  return 0;
}

int32_t path_remove_directory( int32_t fd, int32_t path, int32_t path_len )
{
  TRACE( T32, fd, T32, path, T32, path_len, TEND );
  return 0;
}

int32_t path_rename( int32_t fd,
                     int32_t old_path,
                     int32_t old_path_len,
                     int32_t new_fd,
                     int32_t new_path,
                     int32_t new_path_len )
{
  TRACE( T32, fd, T32, old_path, T32, old_path_len, T32, new_fd, T32, new_path, T32, new_path_len, TEND );
  return 0;
}

int32_t path_symlink( int32_t old_path, int32_t old_path_len, int32_t fd, int32_t new_path, int32_t new_path_len )
{
  TRACE( T32, old_path, T32, old_path_len, T32, fd, T32, new_path, T32, new_path_len, TEND );
  return 0;
}

int32_t path_unlink_file( int32_t fd, int32_t path, int32_t path_len )
{
  TRACE( T32, fd, T32, path, T32, path_len, TEND );
  return 0;
}

int32_t args_sizes_get( int32_t num_argument_ptr, int32_t size_argument_ptr )
{
  int32_t num = size_ro_table_0() - 2;
  int32_t size = 0;

  TRACE( T32, num_argument_ptr, T32, size_argument_ptr, TEND );

  memory_copy_program( num_argument_ptr, &num, 4 );

  // Actual arguments
  for ( int32_t i = 2; i < size_ro_table_0(); i++ ) {
    attach_blob_ro_mem_0( get_ro_table_0( i ) );
    size += size_ro_mem_0();
  }

  memory_copy_program( size_argument_ptr, &size, 4 );
  return 0;
}

int32_t args_get( int32_t argv_ptr, int32_t argv_buf_ptr )
{
  int32_t size;
  int32_t addr = argv_buf_ptr;

  TRACE( T32, argv_ptr, T32, argv_buf_ptr, TEND );

  for ( int32_t i = 2; i < size_ro_table_0(); i++ ) {
    attach_blob_ro_mem_0( get_ro_table_0( i ) );
    size = size_ro_mem_0();
    memory_copy_program( argv_ptr + ( i - 2 ) * 4, &addr, 4 );
    ro_0_to_program_memory( addr, 0, size );
    addr += size;
  }

  return 0;
}

int32_t environ_sizes_get( int32_t retptr0, int32_t retptr1 )
{
  TRACE( T32, retptr0, T32, retptr1, TEND );
  return 0;
}

int32_t environ_get( int32_t environ, int32_t environ_buf )
{
  TRACE( T32, environ, T32, environ_buf, TEND );
  return 0;
}

int32_t path_open( int32_t fd,
                   int32_t dirflags,
                   int32_t path,
                   int32_t path_len,
                   int32_t oflags,
                   int64_t fs_rights_base,
                   int64_t fs_rights_inheriting,
                   int32_t fdflags,
                   int32_t retptr0 )
{
  __wasi_fd_t retfd;
  externref fs = get_ro_table_0( 2 );

  TRACE( T32,
         fd,
         T32,
         dirflags,
         T32,
         path,
         T32,
         path_len,
         T32,
         oflags,
         T64,
         fs_rights_base,
         T64,
         fs_rights_inheriting,
         T32,
         fdflags,
         T32,
         retptr0,
         TEND );

  attach_blob_ro_mem_1( fs );

  retfd = 4;
  fds[retfd].offset = 0;
  fds[retfd].size = size_ro_mem_1() - 1;
  fds[retfd].open = true;

  memory_copy_program( retptr0, &retfd, sizeof( retfd ) );
  return 0;
}

int32_t clock_res_get( int32_t id, int32_t retptr0 )
{
  TRACE( T32, id, T32, retptr0, TEND );
  return 0;
}

int32_t clock_time_get( int32_t id, int64_t precision, int32_t retptr0 )
{
  TRACE( T32, id, T64, precision, T32, retptr0, TEND );
  return 0;
}

int32_t poll_oneoff( int32_t in, int32_t out, int32_t nsubscriptions, int32_t retptr0 )
{
  TRACE( T32, in, T32, out, T32, nsubscriptions, T32, retptr0, TEND );
  return 0;
}

int32_t sched_yield( void )
{
  TRACE( TEND );
  return 0;
}

int32_t random_get( int32_t buf, int32_t buf_len )
{
  TRACE( T32, buf, T32, buf_len, TEND );
  return 0;
}

externref fixpoint_apply( externref encode )
{
  set_rw_table_0( 0, create_blob_i32( 0 ) );

  attach_tree_ro_table_0( encode );

  // TODO set `trace` here based on encode environment variables

  run_start();

  set_rw_table_0( 1, create_blob_rw_mem_1( fds[1].offset ) );

  set_rw_table_0( 2, create_blob_rw_mem_2( trace_offset ) );

  return create_tree_rw_table_0( 2 );
}

externref get_ro_table( int32_t table_index, int32_t index )
{
  return get_ro_table_functions[table_index]( index );
}