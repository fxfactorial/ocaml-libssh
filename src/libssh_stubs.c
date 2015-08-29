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
  CAMLlocal1(raw);

  ssh_session sess = ssh_new();
  if (!sess) {
    printf("couldn't allocate, what\n");
    caml_failwith("Couldn't allocate ssh session");
  }
  raw = caml_alloc(sizeof(ssh_session), Abstract_tag);
  raw = (value)sess;
  CAMLreturn(raw);
}

CAMLprim value libssh_ml_ssh_close(value a_session)
{
  CAMLparam1(a_session);

  ssh_session sess = (ssh_session)Data_custom_val(a_session);
  /* printf("Casted, loc: %p, %d\n", sess, sess); */
  // Currently seg faulting....so something is wrong.
  /* ssh_free(sess); */

  CAMLreturn(Val_unit);
}
