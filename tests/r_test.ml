(* 1) Might need the uri library
   2) Make brew do equivalent of opam pin *)

(* let () = *)
(*   ["ulimit -c unlimited"; *)
(*    "rm -rf /cores/*"] *)
(*   |> List.iter (fun c -> Sys.command c |> ignore) *)
open Ssh.Client

let prog () =
  (* print_string "SSH version is: "; Ssh.version () |> print_endline; *)
  let a_session = Ssh.init () in
  (* Remote Debian machine *)
  let opts = Ssh.Client.({ host = "edgar.haus";
                           log_level = SSH_LOG_NOLOG;
                           port = 22;
                           username = "gar";
                           auth = Auto; })
  in
  Ssh.Client.connect opts a_session;

  let g = Command((fun () -> input_line stdin), a_session) in
  let counter = ref 0 in
  let rec forever () =
    g >>= fun (result, sess) ->
    print_endline result; flush_all ();
    counter := !counter + 1;
    (if !counter < 3 then forever () else Ssh.Client.Finished sess)
  in
  forever ()

let () = ignore @@ prog ()
