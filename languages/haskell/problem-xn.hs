#!/usr/bin/env runhaskell

module Main where

solve n 1 = 1
solve n x = d + 10 * (solve n (x `div` 10 + d * n))
            where d = x `mod` 10

main = do
	print (solve 2 2)
	print (solve 3 3)
	print (solve 4 4)
	print (solve 5 5)
	print (solve 6 6)
	print (solve 7 7)
	print (solve 8 8)
	print (solve 9 9)
