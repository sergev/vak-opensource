#!/usr/bin/env guile -q -s
!#
;;; Commentary:

;;; This is the famous Hello-World-program, written for Guile.  It is a
;;; little bit enhanced in that it understands the command line options
;;; `--help' (-h) and `--version' (-v), which print a short usage
;;; decription or version information, respectively.

;;; Author: Martin Grabmueller
;;; Date: 2001-05-29

;;; Code:

(use-modules (ice-9 getopt-long))

;; This is the grammar for the command line synopsis we expect.
;;
(define command-synopsis
  '((version (single-char #\v) (value #f))
    (help    (single-char #\h) (value #f))))

;; Display version information.
;;
(define (display-version)
  (display "hello 0.0.1\n"))

;; Display the usage help message.
;;
(define (display-help)
  (display "Usage: hello [options...]\n")
  (display "  --help, -h           Show this usage information\n")
  (display "  --version, -v        Show version information\n"))

;; Interpret options, if --help or --version was given, print out the
;; requested information and exit.  Otherwise, print the famous
;; message.
;;
(define (main options)
  (let ((help-wanted (option-ref options 'help #f))
	(version-wanted (option-ref options 'version #f)))
    (if (or version-wanted help-wanted)
	(begin
	  (if version-wanted
	      (display-version))
	  (if help-wanted
	      (display-help)))
	(begin
	  (display "Hello, World!") (newline)))))

;; Call the main program with parsed command line options.
;;
(main (getopt-long (command-line) command-synopsis))

;; Local variables:
;; mode: scheme
;; End:
