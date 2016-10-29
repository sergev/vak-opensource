:
#ed -s rgroups_svc.c << EOF
#/main *(/,/^}\$/d
#-r mainsvc.c
#/^}\$/+1,\$d
#g/_RPCSVC_CLOSEDOWN/d
#g/static void msgout/d
#g/static void closedown/d
#g/_rpc/d
#g/msgout/s/msgout *(/fprintf(stderr, /\\
#s/");/\\\\n");/
#w
#q
#EOF
sed '
/main *(/r mainsvc.c
/main *(/,/^}$/d
/_RPCSVC_CLOSEDOWN/d
/static void msgout/d
/static void closedown/d
/_rpc/d
/msgout/s/msgout *(/fprintf(stderr, /
/msgout/s/");/\\n");/
' rgroups_svc.c > rgroups_svc.c~ && mv rgroups_svc.c~ rgroups_svc.c
