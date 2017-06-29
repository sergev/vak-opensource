#!/bin/bash

A=$((1<<0))
B=$((1<<1))
C=$((1<<2))
D=$((1<<3))
E=$((1<<4))
F=$((1<<5))
G=$((1<<6))
H=$((1<<7))

function outLine
{
  typeset r=0 scan
  for scan
  do
    typeset l=${#scan}
    typeset line=0
    for ((p=0; p<l; p++))
    do
      line="$((line+${scan:$p:1}))"
    done
    for ((column=0; column<8; column++))
      do
        [[ $((line & (1<<column))) == 0 ]] && n=" " || n="#"
        raw[r]="${raw[r]}$n"
      done
      r=$((r+1))
    done
}

function outChar
{
    case "$1" in
    (" ") outLine "" "" "" "" "" "" "" "" ;;
    ("0") outLine "BCDEF" "AFG" "AEG" "ADG" "ACG" "ABG" "BCDEF" "" ;;
    ("1") outLine "F" "EF" "F" "F" "F" "F" "F" "" ;;
    ("2") outLine "BCDEF" "AG" "G" "CDEF" "B" "A" "ABCDEFG" "" ;;
    ("3") outLine "BCDEF" "AG" "G" "CDEF" "G" "AG" "BCDEF" "" ;;
    ("4") outLine "AF" "AF" "AF" "BCDEFG" "F" "F" "F" "" ;;
    ("5") outLine "ABCDEFG" "A" "A" "ABCDEF" "G" "AG" "BCDEF" "" ;;
    ("6") outLine "BCDEF" "A" "A" "BCDEF" "AG" "AG" "BCDEF" "" ;;
    ("7") outLine "BCDEFG" "G" "F" "E" "D" "C" "B" "" ;;
    ("8") outLine "BCDEF" "AG" "AG" "BCDEF" "AG" "AG" "BCDEF" "" ;;
    ("9") outLine "BCDEF" "AG" "AG" "BCDEF" "G" "G" "BCDEF" "" ;;
    ("a") outLine "" "" "BCDE" "F" "BCDEF" "AF" "BCDEG" "" ;;
    ("b") outLine "B" "B" "BCDEF" "BG" "BG" "BG" "ACDEF" "" ;;
    ("c") outLine "" "" "CDE" "BF" "A" "BF" "CDE" "" ;;
    ("d") outLine "F" "F" "BCDEF" "AF" "AF" "AF" "BCDEG" "" ;;
    ("e") outLine "" "" "BCDE" "AF" "ABCDEF" "A" "BCDE" "" ;;
    ("f") outLine "CDE" "B" "B" "ABCD" "B" "B" "B" "" ;;
    ("g") outLine "" "" "BCDEG" "AF" "AF" "BCDE" "F" "BCDE" ;;
    ("h") outLine "B" "B" "BCDE" "BF" "BF" "BF" "ABF" "" ;;
    ("i") outLine "C" "" "BC" "C" "C" "C" "ABCDE" "" ;;
    ("j") outLine "D" "" "CD" "D" "D" "D" "AD" "BC" ;;
    ("k") outLine "B" "BE" "BD" "BC" "BD" "BE" "ABEF" "" ;;
    ("l") outLine "AB" "B" "B" "B" "B" "B" "ABC" "" ;;
    ("m") outLine "" "" "ACEF" "ABDG" "ADG" "ADG" "ADG" "" ;;
    ("n") outLine "" "" "BDE" "BCF" "BF" "BF" "BF" "" ;;
    ("o") outLine "" "" "BCDE" "AF" "AF" "AF" "BCDE" "" ;;
    ("p") outLine "" "" "ABCDE" "BF" "BF" "BCDE" "B" "AB" ;;
    ("q") outLine "" "" "BCDEG" "AF" "AF" "BCDE" "F" "FG" ;;
    ("r") outLine "" "" "ABDE" "BCF" "B" "B" "AB" "" ;;
    ("s") outLine "" "" "BCDE" "A" "BCDE" "F" "ABCDE" "" ;;
    ("t") outLine "C" "C" "ABCDE" "C" "C" "C" "DE" "" ;;
    ("u") outLine "" "" "AF" "AF" "AF" "AF" "BCDEG" "" ;;
    ("v") outLine "" "" "AG" "BF" "BF" "CE" "D" "" ;;
    ("w") outLine "" "" "AG" "AG" "ADG" "ADG" "BCEF" "" ;;
    ("x") outLine "" "" "AF" "BE" "CD" "BE" "AF" "" ;;
    ("y") outLine "" "" "BF" "BF" "BF" "CDE" "E" "BCD" ;;
    ("z") outLine "" "" "ABCDEF" "E" "D" "C" "BCDEFG" "" ;;
    ("A") outLine "D" "CE" "BF" "AG" "ABCDEFG" "AG" "AG" "" ;;
    ("B") outLine "ABCDE" "AF" "AF" "ABCDE" "AF" "AF" "ABCDE" "" ;;
    ("C") outLine "CDE" "BF" "A" "A" "A" "BF" "CDE" "" ;;
    ("D") outLine "ABCD" "AE" "AF" "AF" "AF" "AE" "ABCD" "" ;;
    ("E") outLine "ABCDEF" "A" "A" "ABCDE" "A" "A" "ABCDEF" "" ;;
    ("F") outLine "ABCDEF" "A" "A" "ABCDE" "A" "A" "A" "" ;;
    ("G") outLine "CDE" "BF" "A" "A" "AEFG" "BFG" "CDEG" "" ;;
    ("H") outLine "AG" "AG" "AG" "ABCDEFG" "AG" "AG" "AG" "" ;;
    ("I") outLine "ABCDE" "C" "C" "C" "C" "C" "ABCDE" "" ;;
    ("J") outLine "BCDEF" "D" "D" "D" "D" "BD" "C" "" ;;
    ("K") outLine "AF" "AE" "AD" "ABC" "AD" "AE" "AF" "" ;;
    ("L") outLine "A" "A" "A" "A" "A" "A" "ABCDEF" "" ;;
    ("M") outLine "ABFG" "ACEG" "ADG" "AG" "AG" "AG" "AG" "" ;;
    ("N") outLine "AG" "ABG" "ACG" "ADG" "AEG" "AFG" "AG" "" ;;
    ("O") outLine "CDE" "BF" "AG" "AG" "AG" "BF" "CDE" "" ;;
    ("P") outLine "ABCDE" "AF" "AF" "ABCDE" "A" "A" "A" "" ;;
    ("Q") outLine "CDE" "BF" "AG" "AG" "ACG" "BDF" "CDE" "FG" ;;
    ("R") outLine "ABCD" "AE" "AE" "ABCD" "AE" "AF" "AF" "" ;;
    ("S") outLine "CDE" "BF" "C" "D" "E" "BF" "CDE" "" ;;
    ("T") outLine "ABCDEFG" "D" "D" "D" "D" "D" "D" "" ;;
    ("U") outLine "AG" "AG" "AG" "AG" "AG" "BF" "CDE" "" ;;
    ("V") outLine "AG" "AG" "BF" "BF" "CE" "CE" "D" "" ;;
    ("W") outLine "AG" "AG" "AG" "AG" "ADG" "ACEG" "BF" "" ;;
    ("X") outLine "AG" "AG" "BF" "CDE" "BF" "AG" "AG" "" ;;
    ("Y") outLine "AG" "AG" "BF" "CE" "D" "D" "D" "" ;;
    ("Z") outLine "ABCDEFG" "F" "E" "D" "C" "B" "ABCDEFG" "" ;;
    (".") outLine "" "" "" "" "" "" "D" "" ;;
    (",") outLine "" "" "" "" "" "E" "E" "D" ;;
    (":") outLine "" "" "" "" "D" "" "D" "" ;;
    ("!") outLine "D" "D" "D" "D" "D" "" "D" "" ;;
    ("/") outLine "G" "F" "E" "D" "C" "B" "A" "" ;;
    ("\\") outLine "A" "B" "C" "D" "E" "F" "G" "" ;;
    ("|") outLine "D" "D" "D" "D" "D" "D" "D" "D" ;;
    ("+") outLine "" "D" "D" "BCDEF" "D" "D" "" "" ;;
    ("-") outLine "" "" "" "BCDEF" "" "" "" "" ;;
    ("*") outLine "" "BDF" "CDE" "D" "CDE" "BDF" "" "" ;;
    ("=") outLine "" "" "BCDEF" "" "BCDEF" "" "" "" ;;
    (*) outLine "ABCDEFGH" "AH" "AH" "AH" "AH" "AH" "AH" "ABCDEFGH" ;;
    esac
}

function outArg
{
  typeset l=${#1} c r
  for ((c=0; c<l; c++))
  do
    outChar "${1:$c:1}"
  done
  echo
  for ((r=0; r<8; r++))
  do
    printf "%-*.*s\n" "${COLUMNS:-80}" "${COLUMNS:-80}" "${raw[r]}"
    raw[r]=""
  done
}

for i
do
  outArg "$i"
  echo
done
