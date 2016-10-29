-module (fact).

-export ([fac/1, start/0]).

fac(1) ->
	1;
fac(N) ->
	N * fac(N - 1).

start() ->
	io:format ("10! = ~w~n", [fac (10)]),
	io:format ("20! = ~w~n", [fac (20)]),
	io:format ("30! = ~w~n", [fac (30)]),
	io:format ("40! = ~w~n", [fac (40)]),
	io:format ("50! = ~w~n", [fac (50)]).
