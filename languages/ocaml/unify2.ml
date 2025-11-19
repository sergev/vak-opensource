(* unify2.ml
 * Implementation of unification algorithm in OCaml from scratch.
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
  | Var of string
  | Const of string
  | Expr of term list

(* Substitution type: map from variable names to terms *)
module SubstMap = Map.Make(String)
type substitution = term SubstMap.t

(* Exception for unification failure *)
exception Unification_failure of string

(* Apply substitution σ to a term, recursively replacing variables *)
let rec apply_substitution (sigma : substitution) (t : term) : term =
  match t with
  | Var x -> (
      match SubstMap.find_opt x sigma with
      | Some t' -> apply_substitution sigma t' (* Apply recursively *)
      | None -> Var x
    )
  | Const c -> Const c
  | Expr ts -> Expr (List.map (apply_substitution sigma) ts)

(* Check if variable x occurs in term t *)
let rec occurs_check (x : string) (t : term) : bool =
  match t with
  | Var y -> x = y
  | Const _ -> false
  | Expr ts -> List.exists (occurs_check x) ts

(* Find the first position where two terms differ *)
let rec find_disagreement (t1 : term) (t2 : term) : term * term option =
  match (t1, t2) with
  | (Var x, Var y) when x = y -> (Var x, None)
  | (Var x, Var y) -> (Var x, Some (Var y))
  | (Const c1, Const c2) when c1 = c2 -> (Const c1, None)
  | (Const c1, Const c2) -> (Const c1, Some (Const c2))
  | (Expr ts1, Expr ts2) ->
      if List.length ts1 <> List.length ts2 then
        (Expr ts1, Some (Expr ts2))
      else if ts1 = [] && ts2 = [] then
        (Expr [], None)
      else
        find_disagreement_list ts1 ts2
  | (t1, t2) -> (t1, Some t2)

and find_disagreement_list (ts1 : term list) (ts2 : term list) : term * term option =
  match (ts1, ts2) with
  | ([], []) ->
      (* Both lists exhausted, no disagreement found *)
      (* Return a dummy term with None to indicate no disagreement *)
      (* This case should be handled by the caller, but we need to return something *)
      (Const "", None)
  | (t1 :: rest1, t2 :: rest2) -> (
      match find_disagreement t1 t2 with
      | (p, None) -> find_disagreement_list rest1 rest2
      | (p, Some q) -> (p, Some q)
    )
  | _ -> failwith "find_disagreement_list: lists have different lengths"

(* Compose two substitutions: σ₂ ∘ σ₁ means apply σ₁ first, then σ₂ *)
let compose_substitutions (sigma1 : substitution) (sigma2 : substitution) : substitution =
  (* Apply sigma2 to all values in sigma1 *)
  let sigma1_applied =
    SubstMap.fold
      (fun x t acc -> SubstMap.add x (apply_substitution sigma2 t) acc)
      sigma1
      SubstMap.empty
  in
  (* Merge sigma2 into sigma1_applied, preferring sigma2's bindings *)
  SubstMap.fold SubstMap.add sigma2 sigma1_applied

(* Check if all terms in a list are identical *)
let all_identical (terms : term list) : bool =
  match terms with
  | [] -> true
  | t :: rest -> List.for_all (fun t' -> find_disagreement t t' = (t, None)) rest

(* Find the first disagreement in a list of terms, returning the indices and disagreeing terms *)
let find_first_disagreement_in_list (terms : term list) : (int * int * term * term) option =
  let rec find_pair i = function
    | [] -> None
    | t1 :: rest ->
        let rec find_with_t1 j = function
          | [] -> find_pair (i + 1) rest
          | t2 :: rest2 -> (
              match find_disagreement t1 t2 with
              | (p, None) -> find_with_t1 (j + 1) rest2
              | (p, Some q) -> Some (i, j, p, q)
            )
        in
        find_with_t1 (i + 1) rest
  in
  find_pair 0 terms

(* Main unification function *)
let unify (terms : term list) : substitution =
  if terms = [] then SubstMap.empty
  else
    let rec unify_step (sigma : substitution) (remaining : term list) : substitution =
      (* Step 2: Apply substitution and check if complete *)
      let applied = List.map (apply_substitution sigma) remaining in
      if all_identical applied then
        sigma
      else
        (* Step 3: Find a disagreement *)
        match find_first_disagreement_in_list applied with
        | None -> sigma (* Should not happen if all_identical returned false *)
        | Some (i, j, p, q) -> (
            (* Step 4: Process the disagreement *)
            match (p, q) with
            | (Var x, Var y) when x <> y ->
                (* Case 2: Both are variables but different *)
                let new_sigma = SubstMap.singleton x (Var y) in
                let composed = compose_substitutions sigma new_sigma in
                unify_step composed remaining
            | (Var x, t) | (t, Var x) ->
                (* Case 1: One is a variable *)
                let other_term = if p = Var x then q else p in
                if occurs_check x other_term then
                  raise
                    (Unification_failure
                       ("Occurs check failed: variable " ^ x ^ " occurs in term"))
                else
                  let new_sigma = SubstMap.singleton x other_term in
                  let composed = compose_substitutions sigma new_sigma in
                  unify_step composed remaining
            | (Expr ts1, Expr ts2) ->
                (* Case 4: Both are compound expressions *)
                if List.length ts1 <> List.length ts2 then
                  raise (Unification_failure "Different arity in compound expressions")
                else
                  (* Get the original Expr terms from remaining at indices i and j *)
                  let get_at_index idx lst =
                    List.nth lst idx
                  in
                  let orig_expr1 = get_at_index i remaining in
                  let orig_expr2 = get_at_index j remaining in
                  (* Extract arguments from original terms *)
                  (* For Expr terms, all elements are treated as arguments to be unified *)
                  (* The functor is implicit - if first elements differ, we'd have found disagreement already *)
                  let (orig_args1, orig_args2) =
                    match (orig_expr1, orig_expr2) with
                    | (Expr args1, Expr args2) -> (args1, args2)
                    | _ -> failwith "unify: expected Expr terms"
                  in
                  (* Remove the two Expr terms at indices i and j, add their arguments *)
                  let rec remove_indices idx acc = function
                    | [] -> List.rev acc
                    | hd :: tl ->
                        if idx = i || idx = j then remove_indices (idx + 1) acc tl
                        else remove_indices (idx + 1) (hd :: acc) tl
                  in
                  let remaining_without_exprs = remove_indices 0 [] remaining in
                  (* Add corresponding arguments from original terms *)
                  (* If both have no arguments, they're already unified, just continue *)
                  if orig_args1 = [] && orig_args2 = [] then
                    unify_step sigma remaining_without_exprs
                  else
                    let arg_pairs = List.combine orig_args1 orig_args2 in
                    let new_remaining =
                      List.fold_right
                        (fun (t1, t2) acc -> t1 :: t2 :: acc)
                        arg_pairs
                        remaining_without_exprs
                    in
                    unify_step sigma new_remaining
            | (Const c1, Const c2) ->
                (* Different constants *)
                raise
                  (Unification_failure
                     ("Cannot unify different constants: " ^ c1 ^ " and " ^ c2))
            | (Const c, Expr _) | (Expr _, Const c) ->
                (* Constant vs compound *)
                raise (Unification_failure "Cannot unify constant with compound expression")
          )
    in
    unify_step SubstMap.empty terms

(* Helper function to print a term *)
let rec print_term (t : term) : unit =
  match t with
  | Var x -> Printf.printf "%s" x
  | Const c -> Printf.printf "%s" c
  | Expr ts ->
      print_string "(";
      let rec print_list = function
        | [] -> ()
        | [ t ] -> print_term t
        | t :: rest ->
            print_term t;
            print_string " ";
            print_list rest
      in
      print_list ts;
      print_string ")"

(* Helper function to print substitution *)
let print_substitution (sigma : substitution) : unit =
  let bindings =
    SubstMap.fold (fun x t acc -> (x, t) :: acc) sigma []
  in
  match bindings with
  | [] -> print_string "{}"
  | _ ->
      let rec print_bindings = function
        | [] -> ()
        | [ (x, t) ] ->
            Printf.printf "%s -> " x;
            print_term t
        | (x, t) :: rest ->
            Printf.printf "%s -> " x;
            print_term t;
            print_string ", ";
            print_bindings rest
      in
      print_string "{";
      print_bindings bindings;
      print_string "}"

(* Common routine to run an example *)
let run_example (num : int) (description : string) (terms : term list) (show_result : term option) : unit =
  Printf.printf "Example %d: %s = " num description;
  try
    let sigma = unify terms in
    print_substitution sigma;
    print_newline ();
    (match show_result with
    | Some result_term ->
        let unified_result = apply_substitution sigma result_term in
        print_string "  Result: ";
        print_term unified_result;
        print_newline ()
    | None -> ())
  with Unification_failure msg -> Printf.printf "failed (%s)\n" msg

(* Example 1: unify x y ↦ y *)
let example1 () =
  let x = Var "x" in
  let y = Var "y" in
  run_example 1 "unify x y" [ x; y ] None

(* Example 2: unify (f x y) (g x y) ↦ "clash" *)
let example2 () =
  let f_x_y = Expr [ Const "f"; Var "x"; Var "y" ] in
  let g_x_y = Expr [ Const "g"; Var "x"; Var "y" ] in
  run_example 2 "unify (f x y) (g x y)" [ f_x_y; g_x_y ] None

(* Example 3: unify (f x (h)) (f (h) y) ↦ (f (h) (h)) *)
let example3 () =
  let h = Expr [ Const "h" ] in
  let f_x_h = Expr [ Const "f"; Var "x"; h ] in
  let f_h_y = Expr [ Const "f"; h; Var "y" ] in
  run_example 3 "unify (f x (h)) (f (h) y)" [ f_x_h; f_h_y ] (Some f_x_h)

(* Example 4: unify (f (g x) y) (f y x) ↦ "cycle" *)
let example4 () =
  let g_x = Expr [ Const "g"; Var "x" ] in
  let f_gx_y = Expr [ Const "f"; g_x; Var "y" ] in
  let f_y_x = Expr [ Const "f"; Var "y"; Var "x" ] in
  run_example 4 "unify (f (g x) y) (f y x)" [ f_gx_y; f_y_x ] None

(* Example 5: unify (f (g x) y) (f y (g x)) ↦ (f (g x) (g x)) *)
let example5 () =
  let g_x = Expr [ Const "g"; Var "x" ] in
  let f_gx_y = Expr [ Const "f"; g_x; Var "y" ] in
  let f_y_gx = Expr [ Const "f"; Var "y"; g_x ] in
  run_example 5 "unify (f (g x) y) (f y (g x))" [ f_gx_y; f_y_gx ] (Some f_gx_y)

(* Example 6: unify (f (g x) y) (f y z) ↦ (f (g x) (g x)) *)
let example6 () =
  let g_x = Expr [ Const "g"; Var "x" ] in
  let f_gx_y = Expr [ Const "f"; g_x; Var "y" ] in
  let f_y_z = Expr [ Const "f"; Var "y"; Var "z" ] in
  run_example 6 "unify (f (g x) y) (f y z)" [ f_gx_y; f_y_z ] (Some f_gx_y)

(* Run all examples *)
let run_examples () =
  example1 ();
  example2 ();
  example3 ();
  example4 ();
  example5 ();
  example6 ()

(* Main entry point *)
let () = run_examples ()
