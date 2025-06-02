open Base

let process_escapes str =
    let len = String.length str in
    let buf = Buffer.create len in
    let rec loop i =
        if i >= len then Buffer.contents buf
        else if Char.(str.[i] <> '\\') then (
            Buffer.add_char buf str.[i];
            loop (i + 1)
        ) else if i + 1 < len then (
            match str.[i + 1] with
            | 'n' -> Buffer.add_char buf '\n'; loop (i + 2)
            | 't' -> Buffer.add_char buf '\t'; loop (i + 2)
            | '\\' -> Buffer.add_char buf '\\'; loop (i + 2)
            | _ -> Buffer.add_char buf str.[i]; loop (i + 1)
        ) else (
            Buffer.add_char buf str.[i];
            loop (i + 1)
        )
    in
    loop 0

let echo no_newline enable_escapes args =
    let process = if enable_escapes then process_escapes else Fn.id in
    let output = String.concat ~sep:" " (List.map args ~f:process) in
    if no_newline then
        Out_channel.output_string Out_channel.stdout output
    else
        Out_channel.output_string Out_channel.stdout (output ^ "\n")

let parse_args argv =
    let no_newline = ref false in
    let enable_escapes = ref false in
    let args = ref [] in
    let rec parse = function
        | [] -> ()
        | "-n" :: rest -> no_newline := true; parse rest
        | "-e" :: rest -> enable_escapes := true; parse rest
        | arg :: rest -> args := arg :: !args; parse rest
    in
    parse (List.tl_exn (Array.to_list argv));
    (!no_newline, !enable_escapes, List.rev !args)

let main () =
    let no_newline, enable_escapes, args = parse_args (Sys.get_argv ()) in
    echo no_newline enable_escapes args

let () = main ()
