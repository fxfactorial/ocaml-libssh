open Common

module Client = struct

  type log_level =
    | SSH_LOG_NOLOG
    | SSH_LOG_WARNING
    | SSH_LOG_PROTOCOL
    | SSH_LOG_PACKET
    | SSH_LOG_FUNCTIONS

  type auth =
    | Auto
    | Interactive

  type options = { host: string;
                   username : string;
                   port : int;
                   log_level : log_level;
                   auth : auth; }

  external connect : options -> ssh_session -> unit = "libssh_ml_ssh_connect"

  external exec : command:string -> ssh_session -> string = "libssh_ml_ssh_exec"

  external unsafe_scp :
    string ->
    string ->
    ssh_session ->
    unit = "libssh_ml_ssh_scp"

  let with_session f opts =
    let handle = init () in
    connect opts handle;
    f handle;
    close handle

  let scp ~src_path ~dest_path h =
    if not @@ Sys.file_exists src_path then failwith "This file doesn't exist";
    unsafe_scp src_path dest_path h;
    print_endline "copied"

end
