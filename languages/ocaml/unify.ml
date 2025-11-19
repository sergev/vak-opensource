(* unify.ml
 * Translation of unification algorithm from Scheme to OCaml
 * Original: "The Scheme Programming Language, 2ed" by R. Kent Dybvig
 *)

(* Step 1. Initialization
 *          * Start with a substitution σ₀ = ∅.
 *          * Let k = 0.
 *          * Continue to Step 2.
 * Step 2. Check Whether Unification Is Complete
 *          * Apply the current substitution σₖ to the entire set A, producing Aσₖ.
 *          * If all resulting expressions in Aσₖ are identical, the algorithm stops
 *            and returns σₖ as the most general unifier.
 *          * Otherwise proceed to Step 3.
 * Step 3. Find a Disagreement
 *          * Identify the first position where two expressions in Aσₖ differ.
 *          * Let the disagreeing subexpressions be p and q.
 * Step 4. Process the Disagreement
 *         Depending on the form of p and q:
 *          1. If one is a variable and it does not occur inside the other term
 *             (occurs-check in modern terminology):
 *              * Construct a substitution that maps that variable to the other term.
 *              * Compose this substitution with the current one to produce Aσk+1.
 *              * Increment k; return to Step 2.
 *          2. If both are variables but different:
 *              * Substitute one variable with the other, compose as above, and repeat Step 2.
 *          3. If both are compound expressions with different functors or different arity:
 *              * Unification fails; the set has no unifier, and the algorithm terminates unsuccessfully.
 *          4. If both are compound expressions with the same functor and arity:
 *              * Add their corresponding arguments to the set of expressions being unified.
 *              * Return to Step 2.
 *)

type term =
  | Variable of string
  | Compound of string * term list

exception Unification_failure of string

(* occurs returns true if and only if u occurs in v *)
let rec occurs u = function
  | Variable _ -> false
  | Compound (_, args) ->
    let rec check_list = function
      | [] -> false
      | hd :: tl ->
        hd = u || occurs u hd || check_list tl
    in
    check_list args

(* sigma returns a new substitution procedure extending s by
 * the substitution of u with v, where u is a Variable term *)
let sigma u v s =
  match u with
  | Variable u_name ->
    let rec apply_subst x =
      match s x with
      | Variable x' when x' = u_name -> v
      | Variable x' -> Variable x'
      | Compound (f, args) -> Compound (f, List.map apply_subst args)
    in
    apply_subst
  | Compound _ -> raise (Unification_failure "sigma: first argument must be a variable")

(* try_subst tries to substitute u for v but may require a
 * full unification if (s u) is not a variable, and it may
 * fail if it sees that u occurs in v. *)
let rec try_subst u v s =
  match s u with
  | Variable u' as u_subst ->
    (match s v with
     | Variable v' as v_subst ->
       if u_subst = v_subst then
         s
       else if occurs u_subst v_subst then
         raise (Unification_failure "cycle")
       else
         sigma u_subst v_subst s
     | Compound _ as v_subst ->
       if occurs u_subst v_subst then
         raise (Unification_failure "cycle")
       else
         sigma u_subst v_subst s)
  | Compound _ as u_subst ->
    uni u_subst (s v) s

(* uni attempts to unify u and v with a substitution s.
 * Returns the resulting substitution or raises Unification_failure. *)
and uni u v s =
  match u, v with
  | Variable _, _ -> try_subst u v s
  | _, Variable _ -> try_subst v u s
  | Compound (f1, args1), Compound (f2, args2) ->
    if f1 = f2 && List.length args1 = List.length args2 then
      let rec unify_args u_args v_args s =
        match u_args, v_args with
        | [], [] -> s
        | u_hd :: u_tl, v_hd :: v_tl ->
          let s' = uni u_hd v_hd s in
          unify_args u_tl v_tl s'
        | _ -> raise (Unification_failure "clash")
      in
      unify_args args1 args2 s
    else
      raise (Unification_failure "clash")

(* unify shows one possible interface to uni, where the initial
 * substitution is the identity procedure, the initial success
 * returns the unified term, and failure raises an exception. *)
let unify u v =
  let id_subst x = x in
  let s = uni u v id_subst in
  s u

(* Pretty-print a term *)
let rec string_of_term = function
  | Variable x -> x
  | Compound (f, []) -> "(" ^ f ^ ")"
  | Compound (f, args) ->
    "(" ^ f ^ " " ^ String.concat " " (List.map string_of_term args) ^ ")"

(* Run examples and print results *)
let run_example name u v expected_result expected_exception =
  Printf.printf "Example: %s\n" name;
  Printf.printf "  unify %s %s\n" (string_of_term u) (string_of_term v);
  Printf.printf "  ➡️  ";
  try
    let result = unify u v in
    Printf.printf "%s\n" (string_of_term result);
    (match expected_result with
     | Some exp ->
       if result = exp then
         Printf.printf "  ✅ Correct!\n"
       else
         Printf.printf "  ❌ Expected: %s\n" (string_of_term exp)
     | None -> ());
    (match expected_exception with
     | Some _ -> Printf.printf "  ❌ Expected exception but got result\n"
     | None -> ())
  with
  | Unification_failure msg ->
    Printf.printf "\"%s\"\n" msg;
    (match expected_exception with
     | Some exp_msg when exp_msg = msg ->
       Printf.printf "  ✅ Correct!\n"
     | Some exp_msg ->
       Printf.printf "  ❌ Expected exception: \"%s\"\n" exp_msg
     | None -> Printf.printf "  ❌ Unexpected exception\n");
    (match expected_result with
     | Some _ -> Printf.printf "  ❌ Expected result but got exception\n"
     | None -> ())
  | e ->
    Printf.printf "Unexpected error: %s\n" (Printexc.to_string e);
  Printf.printf "\n"

let () =
  (* Example 1: unify (Variable "x") (Variable "y") ↦ Variable "y" *)
  run_example "1"
    (Variable "x") (Variable "y")
    (Some (Variable "y")) None;

  (* Example 2: unify (f x y) (g x y) ↦ "clash" *)
  run_example "2"
    (Compound ("f", [Variable "x"; Variable "y"]))
    (Compound ("g", [Variable "x"; Variable "y"]))
    None (Some "clash");

  (* Example 3: unify (f x (h)) (f (h) y) ↦ (f (h) (h)) *)
  run_example "3"
    (Compound ("f", [Variable "x"; Compound ("h", [])]))
    (Compound ("f", [Compound ("h", []); Variable "y"]))
    (Some (Compound ("f", [Compound ("h", []); Compound ("h", [])]))) None;

  (* Example 4: unify (f (g x) y) (f y x) ↦ "cycle" *)
  run_example "4"
    (Compound ("f", [Compound ("g", [Variable "x"]); Variable "y"]))
    (Compound ("f", [Variable "y"; Variable "x"]))
    None (Some "cycle");

  (* Example 5: unify (f (g x) y) (f y (g x)) ↦ (f (g x) (g x)) *)
  run_example "5"
    (Compound ("f", [Compound ("g", [Variable "x"]); Variable "y"]))
    (Compound ("f", [Variable "y"; Compound ("g", [Variable "x"])]))
    (Some (Compound ("f", [Compound ("g", [Variable "x"]); Compound ("g", [Variable "x"])]))) None;

  (* Example 6: unify (f (g x) y) (f y z) ↦ (f (g x) (g x)) *)
  run_example "6"
    (Compound ("f", [Compound ("g", [Variable "x"]); Variable "y"]))
    (Compound ("f", [Variable "y"; Variable "z"]))
    (Some (Compound ("f", [Compound ("g", [Variable "x"]); Compound ("g", [Variable "x"])]))) None;
