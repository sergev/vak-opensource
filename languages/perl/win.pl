package win;
# Window array has the following elements
# (all coordinates are w.r.t. (1,1) = top left hand corner of screen
#
# @window=( 
# llx,        # the lower-left x coordinate for the window  
# lrx,        # the lower-left y coordinate of the window
# urx,        # the upper-right x coordinate of the window
# ury,        # the upper right y coordinate of the window
# nattr,      # the normal attribute for the window
# hattr,      # the highlight attribute for the window
# title       # current title text
# footer      # current footer text
# )
# 
# Associated with the window, at any given time is the current text
# body.  we use another array.
# 
# @text=
# line1,   # first line of text to be placed beneath header
# line2,   # second line of text to be placed beneath footer
# ...,
# lineN,   # nth line of text, with n not to exceed lly - ury - 2
# )

sub clear{                       # clear the complete window area
local($llx,$lly,$urx,$ury,$attr) = @_;
local($width,$height) = ($urx-$llx,$lly-$ury);
eval <<'EOLoop';
print "\e"."["."$attr"."m";      # set background attribute, usually black
for($i = $ury;$i<=$lly;$i++){      
  print "\e"."["."$i".";"."$llx"."f";  
  print " " x $width;
}
EOLoop
1;
}

sub refresh{                     # print the current buffer in window
local(@buffer) = splice(@_,0,shift);
local($llx,$lly,$urx,$ury,$attr,$nattr) = splice(@_,0,shift);
local($i,$j,$tmp,$tmp2,$string);
local($width,$height) = ($urx-$llx-1,$lly-$ury);
$llx+=2;
eval <<'EOLoop';
print "\e"."["."$attr"."m";      
for($i = $ury+1,$j=0;$i<$lly;$i++,$j++){      
  #if($i==($lly-1)){if(defined($buffer[$j+1])){$j-=($i-1);$i=($ury+1);}}
  $tmp = $width - length($buffer[$j]);
  print "\e"."["."$i".";"."$llx"."H";
  if ($tmp < 2 )
    {$string =substr($buffer[$j],0,$width-3);
     print $string;}
  else { 
     print $buffer[$j];
     print " " x ($tmp-2); }
  }
EOLoop
}

sub title{         # highlight the top line, text is arg
local($llx,$lly,$urx,$ury,$oattr,$nattr,$text) = @_; 
local($width) = ($urx-$llx);
$fill = $width - length($text);
print "\e"."["."$ury".";"."$llx"."f";
print "\e"."["."$nattr"."m";     
print $text;                  
print " " x $fill;
}

sub footer{         # print a footer on the window with text
local($llx,$lly,$urx,$ury,$oattr,$nattr,$junk,$text) = @_; 
local($width) = ($urx-$llx);
$fill = $width - length($text);
print "\e"."["."$lly".";"."$llx"."f";
print "\e"."["."$nattr"."m";     
print $text;                  
print " " x $fill;
}

sub border{                   
local($llx,$lly,$urx,$ury,$oattr,$nattr) = @_;
local($width) = ($urx-$llx);
local($i,$j);
$ury+=1;
eval <<'EOLoop';
for($i = $ury;$i<$lly;$i++){      
  print "\e"."["."$i".";"."$llx"."H";
  print "\e"."["."$nattr"."m";      
  print " ";                
  print "\e"."["."$oattr"."m";      
  print " " x ($width-2);
  print "\e"."["."$nattr"."m";      
  print " ";                
}
EOLoop
1;
}


sub highlight_field{                # highlight a field in current buffer
local($row,$col,$field,$attr) = @_;
print "\e"."["."$row".";"."$col"."f";
print "\e"."["."$attr"."m";      
print $field;                  
}

sub getdata{                # get data from the keyboard      
local($col,$row,$prompt,$attr) = @_;
local($tmp)=length($prompt)+1;
local($tmp2) = $tmp +$col;
print "\e"."["."$row".";"."$col"."H";
print "\e"."["."$attr"."m";      
print $prompt;                  
print "\e"."["."0"."m";      
print "\e"."["."$row".";"."$tmp2"."H";
$tmp = <STDIN>;
}
1;

