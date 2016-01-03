// C Standard stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
// OCaml declarations
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/custom.h>
// libssh itself
#include <libssh/libssh.h>
#include <libssh/sftp.h>

#define BUFFERSIZE 256

struct result { int status; char *output; };

void clean_up_ssh_memory (value a_session)
{
  CAMLparam1(a_session);
  printf("Finished\n");
  /* ssh_session sess = (ssh_session)a_session; */
  /* ssh_disconnect(sess); */
  /* ssh_free(sess); */
  // Max: Is this a leak of ssh_session?
  CAMLnoreturn;
}

static struct custom_operations ssh_custom_ops = {
  .identifier = "ssh_custom_ops",
  .finalize = clean_up_ssh_memory,
  .compare = NULL,
  .hash = NULL,
  .serialize = NULL,
  .deserialize = NULL
};

CAMLprim value libssh_ml_version(void)
{
  return caml_copy_string(SSH_STRINGIFY(LIBSSH_VERSION));
}

CAMLprim value libssh_ml_ssh_init(void)
{
  CAMLparam0();
  CAMLlocal1(ssh_ml_handle);

  ssh_session this_sess = ssh_new();

  if (!this_sess) {
    caml_failwith("Couldn't allocate ssh session");
  }
  // Last two values are for garbage collection tweaked, best to leave
  // as just 0, 1
  ssh_ml_handle = caml_alloc_custom(&ssh_custom_ops, sizeof(&this_sess), 0, 1);
  memcpy(Data_custom_val(ssh_ml_handle), this_sess, sizeof(&this_sess));
  CAMLreturn(ssh_ml_handle);
}


void check_result(int r, ssh_session this_session)
{
  if (r != SSH_OK) {
    ssh_disconnect(this_session);
    ssh_free(this_session);
    printf("About to fail hard, error code: %d\n", r);
    caml_failwith(ssh_get_error(this_session));
  }
}

static void verify_server(ssh_session this_sess)
{
  // Duh this needs to be better
  switch (ssh_is_server_known(this_sess)) {
  case SSH_SERVER_KNOWN_OK:
    break;
  default:
    printf("Otherwise\n");
  }
}

static struct result exec_remote_command(char *this_command, ssh_session session)
{
  ssh_channel channel;
  int rc;
  char buffer[BUFFERSIZE];
  char *output;
  int nbytes;
  int nread = 0;
  int outputsize;

  channel = ssh_channel_new(session);
  if (channel == NULL)
    return (struct result){SSH_ERROR, NULL};
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    ssh_channel_free(channel);
    return (struct result){rc, NULL};
  }
  rc = ssh_channel_request_exec(channel, this_command);
  if (rc != SSH_OK) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return (struct result){rc, NULL};
  }
  output = caml_stat_alloc(BUFFERSIZE);
  outputsize = BUFFERSIZE;
  while ((nbytes = ssh_channel_read(channel, buffer, BUFFERSIZE, 0)) > 0) {
    if ((nread + nbytes) > outputsize) {
      output = caml_stat_resize(output, outputsize + BUFFERSIZE);
      outputsize += BUFFERSIZE;
    }
    strncpy((output + nread), buffer, nbytes);
    nread += nbytes;
  }
  if (nbytes < 0) {
    caml_stat_free(output);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return (struct result){SSH_ERROR, NULL};
  }

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);

  output = caml_stat_resize(output, nread + 1);
  output[nread] = '\0';
  return (struct result){SSH_OK, output};
}

CAMLprim value libssh_ml_ssh_exec(value command_val, value sess_val)
{
  CAMLparam2(command_val, sess_val);
  CAMLlocal1(output_val);

  char *command;
  size_t len;
  ssh_session this_sess;

  len = caml_string_length(command_val);
  command = caml_strdup(String_val(command_val));
  if (strlen(command) != len) {
    caml_stat_free(command);
    caml_failwith("Problem copying string from OCaml to C");
  }
  this_sess = (ssh_session)Data_custom_val(sess_val);

  struct result this_result = exec_remote_command(command, this_sess);
  output_val = caml_copy_string(this_result.output);
  free(this_result.output);
  CAMLreturn(output_val);
}

CAMLprim value libssh_ml_ssh_connect(value opts, value sess_val)
{
  CAMLparam2(opts, sess_val);
  CAMLlocal5(hostname_val, username_val, port_val, log_level_val, auth_val);

  char *hostname, *username, *password;
  int port, log_level, auth;
  size_t len;
  ssh_session this_sess;

  this_sess = (ssh_session)Data_custom_val(sess_val);
  hostname_val = Field(opts, 0);
  username_val = Field(opts, 1);
  port_val = Field(opts, 2);
  log_level_val = Field(opts, 3);
  auth_val = Field(opts, 4);

  len = caml_string_length(hostname_val);
  hostname = caml_strdup(String_val(hostname_val));

  if (strlen(hostname) != len) {
    caml_stat_free(hostname);
    caml_failwith("Problem copying string from OCaml to C");
  } else len = 0;

  username = caml_strdup(String_val(username_val));
  len = caml_string_length(username_val);

  if (strlen(username) != len) {
    caml_stat_free(hostname);
    caml_stat_free(username);
    caml_failwith("Problem copying string from OCaml to C");
  } else len = 0;

  port = Int_val(port_val);
  log_level = Int_val(log_level_val);
  auth = Int_val(auth_val);

  check_result(ssh_options_set(this_sess, SSH_OPTIONS_HOST, hostname),
  	       this_sess);

  check_result(ssh_options_set(this_sess, SSH_OPTIONS_LOG_VERBOSITY, &log_level),
  	       this_sess);

  check_result(ssh_options_set(this_sess, SSH_OPTIONS_USER, username),
  	       this_sess);

  check_result(ssh_connect(this_sess), this_sess);
  verify_server(this_sess);
  free(hostname);
  free(username);
  switch (auth) {
  case 0:
    check_result(ssh_userauth_publickey_auto(this_sess, NULL, NULL), this_sess);
    break;
  case 1:
    password = getpass("Enter Password: ");
    if (ssh_userauth_password(this_sess, username, password) != SSH_AUTH_SUCCESS) {
      printf("Error: %s\n", ssh_get_error(this_sess));
    }
    free(password);
  }

  CAMLreturn(Val_unit);
}

CAMLprim value libssh_ml_remote_shell(value produce, value consume, value sess_val)
{
  CAMLparam3(produce, consume, sess_val);
  CAMLlocal1(exec_this);

  ssh_session this_sess = (ssh_session)Data_custom_val(sess_val);
  exec_this = caml_callback(produce, Val_unit);
  size_t len = caml_string_length(exec_this);
  char *copied = caml_strdup(String_val(exec_this));
  if (strlen(copied) != len) {
    caml_stat_free(copied);
    caml_failwith("Problem copying string from OCaml to C");
  }

  struct result r = exec_remote_command(copied, this_sess);

  caml_callback(consume, caml_copy_string(r.output));
  caml_stat_free(r.output);
  free(copied);
  CAMLreturn(Val_unit);
}

static ssh_scp prepare(ssh_session sess)
{
  ssh_scp scp;
  int result_code;

  scp = ssh_scp_new(sess, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, ".");
  if(!scp) {
    caml_failwith(ssh_get_error(sess));
  }
  result_code = ssh_scp_init(scp);
  check_result(result_code, sess);
  return scp;
}

CAMLprim value libssh_ml_ssh_scp(value src_path,
				 value dest_path,
				 value sess)
{
  CAMLparam3(src_path, dest_path, sess);
  size_t len = 0;
  int result_code = 0;
  char *s_path, *d_path;
  ssh_session this_sess;

  len = caml_string_length(src_path);
  s_path = caml_strdup(String_val(src_path));

  if (strlen(s_path) != len) {
    caml_stat_free(s_path);
    caml_failwith("Problem copying string from OCaml to C");
  } else len = 0;

  // Max: what is this string for? is it dead code?
  len = caml_string_length(dest_path);
  d_path = caml_strdup(String_val(dest_path));

  if (strlen(d_path) != len) {
    caml_stat_free(s_path);
    caml_stat_free(d_path);
    caml_failwith("Problem copying string from OCaml to C");
  } else len = 0;

  this_sess = (ssh_session)sess;
  ssh_scp this_scp = prepare(this_sess);
  struct stat file_info;
  if (stat(s_path, &file_info) != 0) {
    caml_stat_free(s_path);
    caml_stat_free(d_path);
    // Max: should this_scp also be released/finalized here?
    caml_failwith("Cannot get needed file information for scp");
  }

  result_code = ssh_scp_push_file(this_scp,
				  s_path,
				  file_info.st_size,
				  666);

  caml_stat_free(s_path);
  caml_stat_free(d_path);
  // Max: should this_scp also be released/finalized here?

  CAMLreturn(Val_unit);
}
