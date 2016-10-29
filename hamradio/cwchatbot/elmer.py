#!/usr/bin/env python

import pexpect, sys
keyer = pexpect.spawn ('./test-keyer')
keyer.expect ('ready.')
keyer.logfile = sys.stdout

print '----- Type LELA:'
keyer.expect ('LELA')

print '\n----- Excellent.'
