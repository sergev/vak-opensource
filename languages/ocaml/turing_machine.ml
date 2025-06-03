(* Types for Turing machine *)
type symbol = Zero | One | Blank
type direction = Left | Right
type state = string
type transition = (state * symbol) * (state * symbol * direction)
type tape = { left: symbol list; head: symbol; right: symbol list }

(* Convert symbol to string for printing *)
let string_of_symbol = function
    | Zero -> "0"
    | One -> "1"
    | Blank -> "_"

(* Print the current tape and head position *)
let print_tape tape state =
    let left_str = List.map string_of_symbol tape.left |> String.concat "" in
    let right_str = List.map string_of_symbol tape.right |> String.concat "" in
    Printf.printf "[%s] %s (%s) %s\n" left_str (string_of_symbol tape.head) state right_str

(* Move the tape head *)
let move_head tape dir =
    match dir with
    | Right ->
      let new_right, new_head = match tape.right with
          | [] -> ([], Blank)
          | h :: t -> (t, h)
      in
      { left = tape.head :: tape.left; head = new_head; right = new_right }
    | Left ->
      let new_left, new_head = match tape.left with
          | [] -> ([], Blank)
          | h :: t -> (t, h)
      in
      { left = new_left; head = new_head; right = tape.head :: tape.right }

(* Step the Turing machine *)
let step tape state transitions =
    let current = (state, tape.head) in
    match List.assoc_opt current transitions with
    | None -> None (* No transition: halt *)
    | Some (new_state, new_symbol, dir) ->
      let new_tape = { tape with head = new_symbol } in
      let moved_tape = move_head new_tape dir in
      Some (moved_tape, new_state)

(* Run the Turing machine *)
let run_turing_machine tape start_state transitions accept_state reject_state =
    let rec run tape state =
        print_tape tape state;
        if state = accept_state then Printf.printf "Accepted\n"
        else if state = reject_state then Printf.printf "Rejected\n"
        else match step tape state transitions with
            | None -> Printf.printf "Halted (no transition)\n"
            | Some (new_tape, new_state) -> run new_tape new_state
    in
    run tape start_state

(* Example: A Turing machine that flips 0s to 1s and 1s to 0s until it reaches a blank *)
let example () =
    let transitions = [
        (("q0", Zero), ("q0", One, Right));
        (("q0", One), ("q0", Zero, Right));
        (("q0", Blank), ("qaccept", Blank, Right))
    ] in
    let tape = { left = []; head = Zero; right = [One; Zero; Blank] } in
    run_turing_machine tape "q0" transitions "qaccept" "qreject"

(* Run the example *)
let () = example ()
