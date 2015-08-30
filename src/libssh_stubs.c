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

void check_result(int r)
{
	if (r != SSH_OK) {
		caml_failwith("Some error");
	}
}
CAMLprim value libssh_ml_ssh_connect(value opts, value this_ssh_session)
{
	CAMLparam2(opts, this_ssh_session);
	CAMLlocal2(host_name_val, log_level_val);

	ssh_session this_sess = (ssh_session)Data_custom_val(this_ssh_session);
	host_name_val = Field(opts, 0);
	char *host_name = String_val(host_name_val);

	log_level_val = Field(opts, 1);

	int log_level = Int_val(log_level_val);

	printf("Level: %d\n", log_level);
	check_result(ssh_options_set(this_sess, SSH_OPTIONS_HOST, host_name));
	// Not sure why this seg faults
	/* check_result(ssh_options_set(this_sess, */
	/* 			     SSH_OPTIONS_LOG_VERBOSITY, */
	/* 			     &log_level)); */
	check_result(ssh_options_set(this_sess,
				     SSH_OPTIONS_PORT,
				     &Field(opts, 3)));

	/* check_result(ssh_connect(this_sess)); */
	CAMLreturn(Val_unit);
}
