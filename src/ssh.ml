type ssh_session

external version : unit -> string = "libssh_ml_version"
external init : unit -> ssh_session = "libssh_ml_ssh_init"
external close : ssh_session -> unit = "libssh_ml_ssh_close"
