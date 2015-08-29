#require "ssh"

let () =
  print_string "SSH version is: "; Ssh.version () |> print_endline;
  let a_session = Ssh.init () in

  Ssh.close a_session;
  print_endline "Test finished"
