#require "libssh"

let () =
  Ssh.version () |> print_endline
