/* мигалка на экране */

screen(n)
    int n;
{
    if( isatty(1) )  printf("%8d\b\b\b\b\b\b\b\b",n);
}
