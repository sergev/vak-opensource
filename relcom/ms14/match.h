/*
 * Универсальный пакет выбора файлов по шаблону.
 * Copyright (C) 1990-95 Serge Vakulenko
 */
int matchinit (char *pattern);
int matchglob (register char *string, register char *pattern);
char *matchnext (void);
int ispattern (char *pattern);
