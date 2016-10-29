/fRectHeight/ {
	height = $2
}
/GLYPH/ {
	glyph = $2
}
/^[#-]+$/ {
	if (height < 20)
		width [glyph] = length ($1)
	else
		width [glyph] = length ($1) / 2
}
END {
	for (glyph in width) {
		print glyph, width [glyph]
	}
}
