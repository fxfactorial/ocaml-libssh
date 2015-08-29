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

CAMLprim value libssh_ml_ssh_init(value unit)
{
  CAMLparam1(unit);

  ssh_session sess = ssh_new();
  if (!sess) {
    caml_failwith("Couldn't allocate ssh session");
  }
  CAMLreturn((value)sess);
}

CAMLprim value libssh_ml_ssh_close(value a_session)
{
  CAMLparam1(a_session);

  ssh_session sess = (ssh_session)a_session;
  ssh_free(sess);
  CAMLreturn(Val_unit);
}
