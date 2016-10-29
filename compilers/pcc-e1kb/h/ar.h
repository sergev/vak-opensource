
# define ARMAG          0177545
# define ARHDRSZ        56

struct ar_hdr {
	char    ar_name [14];
	long	ar_date;
	int     ar_uid;
	int     ar_gid;
	int	ar_mode;
	long	ar_size;
};
