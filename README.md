This is an OCaml binding to the C library, [libssh](https://www.libssh.org). 

All code is kept under the `Ssh` module with `Ssh.Client` implementing
client side ssh and `Ssh.Server` providing the server side (although
that isn't implemented yet, client seems more important at the moment).

# Examples

Here is a simple usage at the moment.

```ocaml
let () = 
  let a_session = Ssh.init () in
  (* Remote Debian machine *)
  let opts = Ssh.Client.({ host = "some.machine.with.ssh";
                           log_level = SSH_LOG_NOLOG;
                           port = 22;
                           username = "some.user.name";
                           auth = Auto; })
  in
  Ssh.Client.connect opts a_session;
  a_session |> Ssh.Client.exec ~command:"uname -a" |> print_endline;
  Ssh.close a_session
```

Right now you can either use Auto or Interactive for the
authentication, the latter having `libssh` ask you on the command line
for password authentication.

# Goals

My goals with this library
1.  Make it stable
2.  Do real concurrent `ssh` copying and command execution.
