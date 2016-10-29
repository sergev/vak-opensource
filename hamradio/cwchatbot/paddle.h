/*
 * Interface to Morse paddle.
 *
 * Copyright (C) 2012 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */

/*
 * Open the connection to paddle adapter.
 */
void paddle_open (void);

/*
 * Close the connection.
 */
void paddle_close (void);

/*
 * Perform sync bitbang output/input transaction.
 * Return a state of left and right input contacts.
 */
void paddle_poll (int *left_key, int *right_key);

/*
 * Control debug output.
 */
extern int paddle_debug;
