type ssh_session

external version : unit -> string = "libssh_ml_version"

external create : unit -> ssh_session = "libssh_ml_ssh_init"
