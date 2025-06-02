let process_escapes str =
    let len = String.length str in
    let buf = Buffer.create len in
    let rec loop i =
        if i >= len then Buffer.contents buf
        else if str.[i] <> '\\' then (
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
    let process = if enable_escapes then process_escapes else fun x -> x in
    let output = String.concat " " (List.map process args) in
    if no_newline then print_string output
    else print_endline output

let main () =
    let no_newline = ref false in
    let enable_escapes = ref false in
    let args = ref [] in
    let speclist = [
        ("-n", Arg.Set no_newline, "do not output the trailing newline");
        ("-e", Arg.Set enable_escapes, "enable interpretation of backslash escapes");
    ] in
    Arg.parse speclist (fun arg -> args := arg :: !args) "Usage: echo [-n] [-e] [string ...]";
    echo !no_newline !enable_escapes (List.rev !args)

let () = main ()
