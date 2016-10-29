/* фатальная ошибка */

fatal()
{
    write(2, "FATAL ERROR\n", 12);
    exit(1);
}
