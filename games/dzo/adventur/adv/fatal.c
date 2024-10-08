/* фатальная ошибка */

fatal(n)
{
    write(1, "FATAL ERROR: ", 13);
    write(1, conv( n,3,10,' ' ), 3);
    write(1, "\n", 1);
    exit(1);
}
