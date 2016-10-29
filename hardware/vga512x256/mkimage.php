<?
$sizex = 512;
$sizey = 256;
$step = $sizey / 4;
$bigfont = imageloadfont ("anonymous.gdf");
$font = 2;

$im = @imagecreate ($sizex, $sizey) or die("imagecreate failed");
$black = imagecolorallocate ($im, 0, 0, 0);
$white = imagecolorallocate ($im, 255, 255, 255);

if (0) {
	// draw diagonals
	imageline ($im, 0, 0, $sizex - 1, $sizey - 1, $white);
	imageline ($im, 0, 0, 0, 7, $white);
	imageline ($im, 0, 0, 7, 0, $white);
	imageline ($im, $sizex - 1, $sizey - 1, $sizex - 8, $sizey - 1, $white);
	imageline ($im, $sizex - 1, $sizey - 1, $sizex - 1, $sizey - 8, $white);

	imageline ($im, 0, $sizey - 1, $sizex - 1, 0, $white);
	imageline ($im, 0, $sizey - 1, 7, $sizey - 1, $white);
	imageline ($im, 0, $sizey - 1, 0, $sizey - 8, $white);
	imageline ($im, $sizex - 1, 0, $sizex - 1, 7, $white);
	imageline ($im, $sizex - 1, 0, $sizex - 8, 0, $white);
} else {
	for ($x = $step; $x < $sizex; $x += $step) {
		imageline ($im, $x, 0, $x, $sizey - 1, $white);
	}
	for ($y = $step; $y < $sizey; $y += $step) {
		imageline ($im, 0, $y, $sizex - 1, $y, $white);
	}

	// draw a circle
	$diam = $sizey;
	imagefilledarc ($im, $sizex / 2, $sizey / 2, $diam, $diam, 0, 360, $black, 0);
	imagearc ($im, $sizex / 2, $sizey / 2, $diam, $diam, 0, 360, $white);

	// draw a border
	imagerectangle ($im, 0, 0, $sizex - 1, $sizey - 1, $white);

	// draw a big text
	$string = "$sizex x $sizey";
	$px = ($sizex - imagefontwidth($bigfont) * strlen($string)) / 2;
	$py = ($sizey - imagefontheight($bigfont)) / 2;
	imagestring ($im, $bigfont, $px, $py, $string, $white);

	// draw a small text
	$string = "VGA signal generation";
	$px = ($sizex - imagefontwidth($font) * strlen($string)) / 2;
	imagestring ($im, $font, $px, $sizey / 2 - 55, $string, $white);

	$string = "1 bit per pixel";
	$px = ($sizex - imagefontwidth($font) * strlen($string)) / 2;
	imagestring ($im, $font, $px, $sizey / 2 + 45, $string, $white);
}

imagepng ($im, "$sizex" . "x" . "$sizey" . ".png");

for ($of = 0; $of < 8; ++$of) {
	$fd = fopen ("rom-init-" . chr(ord("a") + $of) . ".v", "w") or
		die("fopen failed");
	for ($r = 0; $r < 64; ++$r) {
		fwrite ($fd, ".INIT_". sprintf("%02X", $r) . "(256'h");
		for ($i = 31; $i >=0; --$i) {
			$x = $of * 2 + ($i % 8) * 64;
			$y = $r * 4 + $i / 8;
			$v = 0;
			if (imagecolorat ($im, $x, $y) != 0)
				$v |= 0x01;
			if (imagecolorat ($im, $x + 1, $y) != 0)
				$v |= 0x02;
			if (imagecolorat ($im, $x + 16, $y) != 0)
				$v |= 0x04;
			if (imagecolorat ($im, $x + 17, $y) != 0)
				$v |= 0x08;
			if (imagecolorat ($im, $x + 32, $y) != 0)
				$v |= 0x10;
			if (imagecolorat ($im, $x + 33, $y) != 0)
				$v |= 0x20;
			if (imagecolorat ($im, $x + 48, $y) != 0)
				$v |= 0x40;
			if (imagecolorat ($im, $x + 49, $y) != 0)
				$v |= 0x80;
			fwrite ($fd, sprintf ("%02X", $v));
		}
		fwrite ($fd, ")");
		if ($r != 63)
			fwrite ($fd, ",");
		fwrite ($fd, "\n");
	}
	fclose ($fd);
}

imagedestroy ($im);
?>
