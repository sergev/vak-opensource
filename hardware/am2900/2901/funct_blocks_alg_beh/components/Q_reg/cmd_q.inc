monitor -n Smon  active  E/*/*/*/*/*/*'sig
monitor -n Smon  active  E/*/*/*/*/*'sig
monitor -n Smon  active  E/*/*/*/*'sig
monitor -n Smon  active  E/*/*/*'sig
monitor -n Smon  active  E/*/*'sig
monitor -n Smon  active  E/*'sig
monitor -n Vmon  read  E/*/*/*/*/*/*'var
monitor -n Vmon  read  E/*/*/*/*/*'var
monitor -n Vmon  read  E/*/*/*/*'var
monitor -n Vmon  read  E/*/*/*'var
monitor -n Vmon  read  E/*/*'var
monitor -n Vmon  read  E/*'var
monitor -n Vmon  write  E/*/*/*/*/*/*'var
monitor -n Vmon  write  E/*/*/*/*/*'var
monitor -n Vmon  write  E/*/*/*/*'var
monitor -n Vmon  write  E/*/*/*'var
monitor -n Vmon  write  E/*/*'var
monitor -n Vmon  write  E/*'var

trace E/*/*/*/*/*/*'var
trace E/*/*/*/*/*'var
trace E/*/*/*/*'var
trace E/*/*/*'var
trace E/*/*'var
trace E/*'var

trace E/*/*/*/*/*/*'sig
trace E/*/*/*/*/*'sig
trace E/*/*/*/*'sig
trace E/*/*/*'sig
trace E/*/*'sig
trace E /*'sig

open run.out
logtime -e run.out

list > run.out
run 40 > run.out

!cat run.out

quit






