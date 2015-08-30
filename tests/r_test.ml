(* Might need the uri library *)

open Ssh.Client

let () = Sys.command "ulimit -c unlimited" |> ignore

let () =
  print_string "SSH version is: "; Ssh.version () |> print_endline;
  let a_session = Ssh.init () in

  let opts = {host = "localhost"; log_level = SSH_LOG_PROTOCOL; port = 22} in
  connect opts a_session;

  (* Ssh.close a_session; *)
  print_endline "Test finished";print_newline ()

(* let () = *)
(*   let opts = {host = "edgar.haus"; log_level = SSH_LOG_PROTOCOL; port = 22} in *)
(*   with_session (fun _ -> ()) opts; *)
(*   print_endline "With session finished" *)
