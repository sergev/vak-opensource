#!/usr/bin/env runhaskell

module Main where

solution k = [ (a `div` d, b `div` d, n) | n <- [k+1 ..],
	       let { a = n - k;
		     b = n*k - 1;
		     d = gcd a b },
	       d > k ]

main = do
	print (take 5 (solution 2))
	print (take 5 (solution 3))
	print (take 5 (solution 4))
	print (take 5 (solution 5))
