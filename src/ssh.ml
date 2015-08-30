(** OCaml bindings to libssh, both Client and Server side
    functionality provided *)

(** Abstract type for an ssh session*)
type ssh_session

(** libssh's version *)
external version : unit -> string = "libssh_ml_version"
(** Create a fresh ssh_session *)
external init : unit -> ssh_session = "libssh_ml_ssh_init"
(** Close a ssh_session *)
external close : ssh_session -> unit = "libssh_ml_ssh_close"

(** Client side of ssh *)
module Client = struct

  type log_level =
    | SSH_LOG_NOLOG     (** No logging at all *)
    | SSH_LOG_WARNING   (** Only warnings *)
    | SSH_LOG_PROTOCOL  (** High level protocol information *)
    | SSH_LOG_PACKET    (** Lower level protocol infomations, packet level *)
    | SSH_LOG_FUNCTIONS (** Every function path *)

  type auth =
    | Auto        (** Authenticate using the Ssh agent, assuming its running *)
    | Interactive (** Type in the password on the command line*)

  type options = { host: string;
                   username : string;
                   port : int;
                   log_level : log_level;
                   auth : auth; }

  (** Connect and authenticate a ssh connection *)
  external connect : options -> ssh_session -> unit = "libssh_ml_ssh_connect"
  (** Execute a remote command, get result as a string *)
  external exec : command:string -> ssh_session -> string = "libssh_ml_ssh_exec"

  let with_session f opts =
    let handle = init () in
    connect opts handle;
    f handle;
    close handle

end

(** Server side of ssh *)
module Server = struct


end

