<?php
$DECODE_ARRAY = array(
	'@','А','Б','Ц','Д','Е','Ф','Г','Х','И','Ж','К','Л','М','Н','О',
	'П','Q','Р','С','Т','У','В','W','Ь','Ы','З','[','\\',']','^','_',
	'`','а','б','ц','д','е','ф','г','х','и','ж','к','л','м','н','о',
	'п','q','р','с','т','у','в','w','ь','ы','з','{','|','}','~','?');

$ENCODE_ARRAY = array(
        '?','a','b','c','d','e','f','g','h','i','?','k','l','m','n','o',
        'p','?','r','s','t','u','j','v','x','y','z','?','?','?','?','?',
        '?','A','B','C','D','E','F','G','H','I','?','K','L','M','N','O',
        'P','?','R','S','T','U','J','V','X','Y','Z','?','?','?','?','?');

function volapuk_decode($to_decode)
{
	global $DECODE_ARRAY, $ENCODE_ARRAY;

	$len = strlen($to_decode);
	$result = "";

	for ($i=0; $i<$len; ++$i) {
		$sym = substr($to_decode, $i, 1);
		$ord = ord($sym);
		if ($ord <= 64 || $ord >= 127) {
			$result .= $sym;
			continue;
		}
		if (($i + 1) < $len && substr($to_decode, 1+$i, 1) == '`') {
			switch ($sym) {
			case 'Y': $result .= 'Ё'; break;
			case 'y': $result .= 'ё'; break;
			case 'I': $result .= 'Й'; break;
			case 'i': $result .= 'й'; break;
			case 'C': $result .= 'Ч'; break;
			case 'c': $result .= 'ч'; break;
			case 'S': $result .= 'Ш'; break;
			case 's': $result .= 'ш'; break;
			case 'H': $result .= 'Щ'; break;
			case 'h': $result .= 'щ'; break;
			case 'X': $result .= 'Ъ'; break;
			case 'x': $result .= 'ъ'; break;
			case 'E': $result .= 'Э'; break;
			case 'e': $result .= 'э'; break;
			case 'U': $result .= 'Ю'; break;
			case 'u': $result .= 'ю'; break;
			case 'A': $result .= 'Я'; break;
			case 'a': $result .= 'я'; break;
			default:  $result .= $sym;
			}
			++$i;
			continue;
		}
		$result .= $DECODE_ARRAY [$ord - 64];
	}
	return $result;
}

function volapuk_encode($to_encode)
{
	global $DECODE_ARRAY, $ENCODE_ARRAY;

	$len = strlen($to_encode);
	$result = "";

	for ($i=0; $i<$len; ++$i) {
		$sym = substr($to_encode, $i, 1);
		$ord = ord($sym);
		if ($ord < 128) {
			$result .= $sym;
			continue;
		}
		switch ($sym) {
                case 'Ё': $result .= "Y`"; break;
                case 'ё': $result .= "y`"; break;
                case 'Й': $result .= "I`"; break;
                case 'й': $result .= "i`"; break;
                case 'Ч': $result .= "C`"; break;
                case 'ч': $result .= "c`"; break;
                case 'Ш': $result .= "S`"; break;
                case 'ш': $result .= "s`"; break;
                case 'Щ': $result .= "H`"; break;
                case 'щ': $result .= "h`"; break;
                case 'Ъ': $result .= "X`"; break;
                case 'ъ': $result .= "x`"; break;
                case 'Э': $result .= "E`"; break;
                case 'э': $result .= "e`"; break;
                case 'Ю': $result .= "U`"; break;
                case 'ю': $result .= "u`"; break;
                case 'Я': $result .= "A`"; break;
                case 'я': $result .= "a`"; break;
                default:
                        if ($ord < 192) {
				$result .= $sym;
			} else {
				$result .= $ENCODE_ARRAY [$ord - 192];
			}
		}
	}
	return $result;
}
