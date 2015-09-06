(** OCaml bindings to libssh, both Client and Server side
    functionality provided *)

(** Abstract type for an ssh session*)
type ssh_session

(** libssh's version *)
val version : unit -> string

(** Create a fresh ssh_session *)
val init : unit -> ssh_session

(** Close a ssh_session *)
val close : ssh_session -> unit

(** Client side of SSH *)
module Client : sig

  (** Different log levels in increasing order of verbosity *)
  type log_level =
    | SSH_LOG_NOLOG     (** No logging at all *)
    | SSH_LOG_WARNING   (** Only warnings *)
    | SSH_LOG_PROTOCOL  (** High level protocol information *)
    | SSH_LOG_PACKET    (** Lower level protocol infomations, packet level *)
    | SSH_LOG_FUNCTIONS (** Every function path *)

  (** Different kinds of authentication accepted by libssh *)
  type auth =
    | Auto        (** Authenticate using the Ssh agent, assuming its running *)
    | Interactive (** Type in the password on the command line*)

  (** Options needed when connecting over ssh *)
  type options = { host: string;
                   username : string;
                   port : int;
                   log_level : log_level;
                   auth : auth; }

  (** Connect and authenticate a ssh connection *)
  val connect : options -> ssh_session -> unit

  (** Execute a remote command, get result as a string *)
  val exec : command:string -> ssh_session -> string

  val with_session : (ssh_session -> 'a) -> options -> unit

  val scp : src_path:string -> dest_path:string -> ssh_session -> unit

end
