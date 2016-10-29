struct ranlib {
	short   ran_len;        /* 1 байт - длина имени в байтах */
	long    ran_off;        /* 4 байта - смещение в файле */
	char    *ran_name;      /* указатель на имя */
};
