(* 1) Might need the uri library
   2) Make brew do equivalent of opam pin *)

open Ssh.Client

let () =
  ["ulimit -c unlimited";
   "rm -rf /cores/*"]
  |> List.iter (fun c -> Sys.command c |> ignore)

let () =
  print_string "SSH version is: "; Ssh.version () |> print_endline;
  let a_session = Ssh.init () in
  (* Remote Debian machine *)
  let opts = { host = "edgar.haus";
               log_level = SSH_LOG_NOLOG;
               port = 22;
               username = "gar";
               command = "uname -a";
               auth = Auto; }
  in
  connect opts a_session;

  Ssh.close a_session;
  print_endline "Test finished"; print_newline ()

(* let () = *)
(*   let opts = {host = "edgar.haus"; log_level = SSH_LOG_PROTOCOL; port = 22} in *)
(*   with_session (fun _ -> ()) opts; *)
(*   print_endline "With session finished" *)
