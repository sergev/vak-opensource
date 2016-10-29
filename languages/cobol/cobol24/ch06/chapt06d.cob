000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt06d.
000031* Inspect Example
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Data-Item Pic X(20) Value "Teach Yourself COBOL".
000068  Procedure Division.
000327 Chapt06d-Start.
000328     Inspect Data-Item
000329        Replacing All Space By "*" After "f"
000330                  All "OBOL" By "obol" After "C"
000331                  All "e" By "E" Before "Y"
000342     Display Data-Item
000352     Stop Run.