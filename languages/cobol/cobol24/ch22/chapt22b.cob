000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22b.
000031*annuity Example
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Loan-Amt         Pic 9(6)v99   Value Zeros.
000077 01  Interest-Rate    Pic 9(3)v99   Value Zeros.
000078 01  Loan-Years       Pic 9(3)      Value Zeros.
000087 01  Payment-Amt      Pic 9(6)v99   Value Zeros.
000097 01  Monthly-Interest Pic 9(3)v9(9) Value Zeros.
000136 Screen Section.
000137 01  Data-Entry Blank Screen Auto.
000138     03  Line 01 Column 01 Value "Enter Principal: ".
000139     03  Line 01 Column 18 Pic Z(6).99 Using Loan-Amt.
000140     03  Line 03 Column 1  Value "Enter Interest Rate: ".
000141     03  Line 03 Column 22 Pic Z(2)9.99 Using Interest-Rate.
000142     03  Line 04 Column 1  Value "Number of Years of Loan: ".
000143     03  Line 04 Column 26 Pic ZZ9 Using Loan-Years.
000144     03  Line 06 Column 1  Value "Monthly Payment: ".
000145     03  Line 06 Column 18 Pic Z(3),Z(3).99 From Payment-Amt.
000148 Procedure Division.
000159 Chapt22b-Start.
000169     Display Data-Entry
000179     Accept Data-Entry
000189     Compute Monthly-Interest Rounded = (Interest-Rate / 12) / 100
000199     Compute Payment-Amt Rounded = Loan-Amt *
000209             Function Annuity (Monthly-Interest, Loan-Years * 12)
000219     Display Data-Entry
000299     Stop Run
000309     .