#!/usr/bin/env runhugs
--
-- Вычисление простых чисел методом решета Эратосфена.
--
module Main where

primes = sieve [2..]
	 where
		sieve (x:xs) = x : sieve (filter ((/= 0).(`mod` x)) xs)

main = do
	print (take 200 primes)
