(* 1) Might need the uri library
   2) Make brew do equivalent of opam pin *)

(* let () = *)
(*   ["ulimit -c unlimited"; *)
(*    "rm -rf /cores/*"] *)
(*   |> List.iter (fun c -> Sys.command c |> ignore) *)

open Ssh.Client

let () =
  print_string "SSH version is: "; Ssh.version () |> print_endline;
  let a_session = Ssh.create () in
  (* Remote Debian machine *)
  let opts = Ssh.Client.({ host = "edgar.haus";
                           log_level = SSH_LOG_NOLOG;
                           port = 22;
                           username = "gar";
                           auth = Auto; })
  in
  Ssh.Client.connect opts a_session;

  (* a_session |> Ssh.Client.exec ~command:"uname -a" |> print_endline; *)
  (* Ssh.Client.scp "scratch.c" "/home/gar/copied_scratch.c" a_session; *)
  (* a_session |> Ssh.Client.exec ~command:"cat scratch.c" |> print_endline *)

  ()
