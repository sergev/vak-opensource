      ********************************
      * Parameter For Converse-Panel *
      * Parameter For Get-Input      *
      ********************************
       01  Chapt24a-Converse-Data.
           05  Chapt24a-Ret-Code
                                       Pic S9(4) Comp-5.
           05  Chapt24a-Lens.
               10  Chapt24a-Len-Len
                                       Pic S9(4) Comp-5 Value +20.
               10  Chapt24a-Ip-Num-Len
                                       Pic S9(4) Comp-5 Value +40.
               10  Chapt24a-Ip-Char-Len
                                       Pic S9(4) Comp-5 Value +106.
               10  Chapt24a-Op-Num-Len
                                       Pic S9(4) Comp-5 Value +6.
               10  Chapt24a-Op-Char-Len
                                       Pic S9(4) Comp-5 Value +2.
               10  Chapt24a-Field-Len
                                       Pic S9(4) Comp-5 Value +344.
               10  Chapt24a-Colr-Len
                                       Pic S9(4) Comp-5 Value +21.
               10  Chapt24a-Type-Len
                                       Pic S9(4) Comp-5 Value +21.
               10  Filler
                                       Pic S9(4) Comp-5 Value +0.
               10  Filler
                                       Pic S9(4) Comp-5 Value +0.
           05  Chapt24a-Data.
      ******** Chapt24a-Ip-Num-Data ********
               10  Chapt24a-Key
                                       Pic S9(4) Comp-5.
                   88  Chapt24a-Read-Hit
                                       Value 317.
                   88  Chapt24a-Clear-Hit
                                       Value 318.
                   88  Chapt24a-Exit-Hit
                                       Value 319.
               10  Chapt24a-Next-Fld-Id
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Next-Fld-Num
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Next-Tab-Num
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Next-Occurs
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Fld-Id
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Fld-Num
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Tab-Num
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Occurs
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Menu-Id
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Row-Col-Sw
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Cursor-Row
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Cursor-Col
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Row
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Col
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Disp-Sw
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Next-Vert
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Vert
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Next-Hor
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Last-Hor
                                       Pic S9(4) Comp-5.
      ******** Chapt24a-Ip-Char-Data ********
               10  Chapt24a-Next-Panel
                                       Pic X(8).
               10  Chapt24a-Next-Field
                                       Pic X(30).
               10  Chapt24a-Last-Field
                                       Pic X(30).
               10  Chapt24a-Menu-Option
                                       Pic X(30).
               10  Chapt24a-Switch-Sw
                                       Pic X.
               10  Chapt24a-Size-Sw
                                       Pic X.
               10  Chapt24a-Mouse-Sw
                                       Pic X.
               10  Chapt24a-Capture-Sw
                                       Pic X.
               10  Chapt24a-Wait-Sw
                                       Pic X.
               10  Chapt24a-Curs-Sw
                                       Pic X.
               10  Chapt24a-Chg-Sw
                                       Pic X.
               10  Chapt24a-Timeout
                                       Pic X.
      ******** Chapt24a-Op-Num-Data ********
               10  Chapt24a-Pan-Pos-Sw
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Pan-Row
                                       Pic S9(4) Comp-5.
               10  Chapt24a-Pan-Col
                                       Pic S9(4) Comp-5.
      ******** Chapt24a-Op-Char-Data ********
               10  Chapt24a-New-Window
                                       Pic X.
               10  Chapt24a-Display-Sw
                                       Pic X.
      ******** Chapt24a-Op-Var-Data ********
           05  Chapt24a-Fields.
               10  Chapt24a-Number
                                       Pic X(0008).
               10  Chapt24a-Last
                                       Pic X(0025).
               10  Chapt24a-First
                                       Pic X(0015).
               10  Chapt24a-Middle
                                       Pic X(0010).
               10  Chapt24a-Address-Line-1
                                       Pic X(0050).
               10  Chapt24a-Address-Line-2
                                       Pic X(0050).
               10  Chapt24a-City
                                       Pic X(0040).
               10  Chapt24a-State-Or-Country
                                       Pic X(0020).
               10  Chapt24a-Postal-Code
                                       Pic X(0015).
               10  Chapt24a-Home-Phone
                                       Pic X(0020).
               10  Chapt24a-Work-Phone
                                       Pic X(0020).
               10  Chapt24a-Other-Phone
                                       Pic X(0020).
               10  Chapt24a-Start-Date
                                       Pic 9(0008).
               10  Chapt24a-Last-Rent-Paid-Date
                                       Pic 9(0008).
               10  Chapt24a-Next-Rent-Due-Date
                                       Pic 9(0008).
               10  Chapt24a-Rent-Amount
                                       Pic 9(04)v9(02).
               10  Chapt24a-Consignment-Percent
                                       Pic 9(003).
               10  Chapt24a-Operation-Type
                                       Pic X(0018).
           05  Chapt24a-Colrs.
               10  Chapt24a-Number-C
                                       Pic X.
               10  Chapt24a-Last-C
                                       Pic X.
               10  Chapt24a-First-C
                                       Pic X.
               10  Chapt24a-Middle-C
                                       Pic X.
               10  Chapt24a-Address-Line-1-C
                                       Pic X.
               10  Chapt24a-Address-Line-2-C
                                       Pic X.
               10  Chapt24a-City-C
                                       Pic X.
               10  Chapt24a-State-Or-Country-C
                                       Pic X.
               10  Chapt24a-Postal-Code-C
                                       Pic X.
               10  Chapt24a-Home-Phone-C
                                       Pic X.
               10  Chapt24a-Work-Phone-C
                                       Pic X.
               10  Chapt24a-Other-Phone-C
                                       Pic X.
               10  Chapt24a-Start-Date-C
                                       Pic X.
               10  Chapt24a-Last-Rent-Paid-Date-C
                                       Pic X.
               10  Chapt24a-Next-Rent-Due-Date-C
                                       Pic X.
               10  Chapt24a-Rent-Amount-C
                                       Pic X.
               10  Chapt24a-Consignment-Percent-C
                                       Pic X.
               10  Chapt24a-Operation-Type-C
                                       Pic X.
               10  Chapt24a-Read-C
                                       Pic X.
               10  Chapt24a-Clear-C
                                       Pic X.
               10  Chapt24a-Exit-C
                                       Pic X.
           05  Chapt24a-Types.
               10  Chapt24a-Number-T
                                       Pic X.
               10  Chapt24a-Last-T
                                       Pic X.
               10  Chapt24a-First-T
                                       Pic X.
               10  Chapt24a-Middle-T
                                       Pic X.
               10  Chapt24a-Address-Line-1-T
                                       Pic X.
               10  Chapt24a-Address-Line-2-T
                                       Pic X.
               10  Chapt24a-City-T
                                       Pic X.
               10  Chapt24a-State-Or-Country-T
                                       Pic X.
               10  Chapt24a-Postal-Code-T
                                       Pic X.
               10  Chapt24a-Home-Phone-T
                                       Pic X.
               10  Chapt24a-Work-Phone-T
                                       Pic X.
               10  Chapt24a-Other-Phone-T
                                       Pic X.
               10  Chapt24a-Start-Date-T
                                       Pic X.
               10  Chapt24a-Last-Rent-Paid-Date-T
                                       Pic X.
               10  Chapt24a-Next-Rent-Due-Date-T
                                       Pic X.
               10  Chapt24a-Rent-Amount-T
                                       Pic X.
               10  Chapt24a-Consignment-Percent-T
                                       Pic X.
               10  Chapt24a-Operation-Type-T
                                       Pic X.
               10  Chapt24a-Read-T
                                       Pic X.
               10  Chapt24a-Clear-T
                                       Pic X.
               10  Chapt24a-Exit-T
                                       Pic X.
      ************************************************
      * Field Ids - Use For Cursor Positioning, Etc. *
      ************************************************
       01  Chapt24a-Ids.
           05  Chapt24a-Number-I
                                       Pic S9(4) Comp-5 Value +1.
           05  Chapt24a-Last-I
                                       Pic S9(4) Comp-5 Value +2.
           05  Chapt24a-First-I
                                       Pic S9(4) Comp-5 Value +3.
           05  Chapt24a-Middle-I
                                       Pic S9(4) Comp-5 Value +4.
           05  Chapt24a-Address-Line-1-I
                                       Pic S9(4) Comp-5 Value +5.
           05  Chapt24a-Address-Line-2-I
                                       Pic S9(4) Comp-5 Value +6.
           05  Chapt24a-City-I
                                       Pic S9(4) Comp-5 Value +7.
           05  Chapt24a-State-Or-Country-I
                                       Pic S9(4) Comp-5 Value +8.
           05  Chapt24a-Postal-Code-I
                                       Pic S9(4) Comp-5 Value +9.
           05  Chapt24a-Home-Phone-I
                                       Pic S9(4) Comp-5 Value +10.
           05  Chapt24a-Work-Phone-I
                                       Pic S9(4) Comp-5 Value +11.
           05  Chapt24a-Other-Phone-I
                                       Pic S9(4) Comp-5 Value +12.
           05  Chapt24a-Start-Date-I
                                       Pic S9(4) Comp-5 Value +13.
           05  Chapt24a-Last-Rent-Paid-Date-I
                                       Pic S9(4) Comp-5 Value +14.
           05  Chapt24a-Next-Rent-Due-Date-I
                                       Pic S9(4) Comp-5 Value +15.
           05  Chapt24a-Rent-Amount-I
                                       Pic S9(4) Comp-5 Value +16.
           05  Chapt24a-Consignment-Percent-I
                                       Pic S9(4) Comp-5 Value +17.
           05  Chapt24a-Operation-Type-I
                                       Pic S9(4) Comp-5 Value +18.
           05  Chapt24a-Read-I
                                       Pic S9(4) Comp-5 Value +19.
           05  Chapt24a-Clear-I
                                       Pic S9(4) Comp-5 Value +20.
           05  Chapt24a-Exit-I
                                       Pic S9(4) Comp-5 Value +21.