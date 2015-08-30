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
// libssh itself
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
  ssh_init();

  if (!sess) {
    caml_failwith("Couldn't allocate ssh session");
  }
  return (value)sess;
}

CAMLprim value libssh_ml_ssh_close(value a_session)
{
  // This needs to be way more comprehensive and check for
  // way more things, for now let's now use.
  CAMLparam1(a_session);

  ssh_session sess = (ssh_session)a_session;
  ssh_disconnect(sess);
  ssh_free(sess);
  CAMLreturn(Val_unit);
}

void check_result(int r, ssh_session this_session)
{
  if (r != SSH_OK) {
    ssh_free(this_session);
    caml_failwith(ssh_get_error(this_session));
  }
}

CAMLprim value libssh_ml_ssh_connect(value opts, value this_ssh_session)
{
  CAMLparam2(opts, this_ssh_session);
  CAMLlocal4(hostname_val, username_val, port_val, log_level_val);

  char *hostname, *username;
  int port, log_level;
  ssh_session this_sess;

  this_sess = (ssh_session)this_ssh_session;
  hostname_val = Field(opts, 0);
  username_val = Field(opts, 1);
  port_val = Field(opts, 2);
  log_level_val = Field(opts, 3);


  hostname = String_val(hostname_val);
  username = String_val(username_val);
  port = Int_val(port_val);
  log_level = Int_val(log_level_val);

  printf("Level: %d, Host_name:%s, username: %s, on Port: %d\n",
  	 log_level,
  	 hostname,
  	 username,
	 port);

  check_result(ssh_options_set(this_sess,
  			       SSH_OPTIONS_HOST,
  			       hostname),
  	       this_sess);

  check_result(ssh_options_set(this_sess,
  			       SSH_OPTIONS_LOG_VERBOSITY,
  			       &log_level),
  	       this_sess);

  check_result(ssh_options_set(this_sess,
  			       SSH_OPTIONS_USER,
  			       username),
  	       this_sess);

  check_result(ssh_connect(this_sess), this_sess);
  CAMLreturn(Val_unit);
}

/* CAMLprim value libssh_ml_ssh_server_create(value unit) */
/* { */

/* } */
