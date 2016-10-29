	subroutine paint (n, x, y)
	common /gftab/ irdb (5), rdb (17)
	call painta (n, int (x1 * rdb (1)), int (y1 * rdb (1)))
	return
	end
