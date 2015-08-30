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
    ssh_disconnect(this_session);
    ssh_free(this_session);
    caml_failwith(ssh_get_error(this_session));
  }
}

static void verify_server(ssh_session this_sess)
{
  switch (ssh_is_server_known(this_sess)) {
  case SSH_SERVER_KNOWN_OK:
    printf("Known server\n");
    break;
  default:
    printf("Otherwise\n");
  }
}

static int exec_remote_command(char *this_command, ssh_session session)
{
  ssh_channel channel;
  int rc;
  char buffer[256];
  unsigned int nbytes;
  channel = ssh_channel_new(session);
  if (channel == NULL)
    return SSH_ERROR;
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    ssh_channel_free(channel);
    return rc;
  }
  rc = ssh_channel_request_exec(channel, this_command);
  if (rc != SSH_OK) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0) {
    if (write(1, buffer, nbytes) != nbytes) {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }

  if (nbytes < 0) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }
  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}

CAMLprim value libssh_ml_ssh_connect(value opts, value this_ssh_session)
{
  CAMLparam2(opts, this_ssh_session);
  CAMLlocal5(hostname_val, username_val, port_val, log_level_val, command_val);

  char *hostname, *username, *password, *command;
  int port, log_level;
  ssh_session this_sess;

  this_sess = (ssh_session)this_ssh_session;
  hostname_val = Field(opts, 0);
  username_val = Field(opts, 1);
  port_val = Field(opts, 2);
  log_level_val = Field(opts, 3);
  command_val = Field(opts, 4);

  hostname = String_val(hostname_val);
  username = String_val(username_val);
  port = Int_val(port_val);
  log_level = Int_val(log_level_val);
  command = String_val(command_val);

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
  verify_server(this_sess);

  password = getpass("Enter Password: ");
  if (ssh_userauth_password(this_sess, username, password) != SSH_AUTH_SUCCESS) {
    printf("Error: %s\n", ssh_get_error(this_sess));
  }
  check_result(exec_remote_command(command, this_sess), this_sess);
  CAMLreturn(Val_unit);
}
