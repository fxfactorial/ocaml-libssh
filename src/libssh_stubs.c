// C Standard stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
// OCaml declarations
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>

#include <libssh/libssh.h>

CAMLprim value libssh_ml_version(value unit)
{
  CAMLparam1(unit);
  CAMLlocal1(ml_v_string);

  ml_v_string = caml_copy_string(SSH_STRINGIFY(LIBSSH_VERSION));
  CAMLreturn(ml_v_string);
}
