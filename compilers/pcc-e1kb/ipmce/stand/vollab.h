/* DVS volume label structure */

/* Fixed part */

struct vl_fixed {
	int vl_magic       ;    /* magic word =VL_MAGIC */
	int vl_fmttime  :32;    /* formatting time */
	int vl_systemid :16;    /* system where format was done */
	int vl_revision : 8;    /* volume label revision =0 */
	int vl_version  : 8;    /* volume label version =1 */
	int vl_name        ;    /* volume name */
	int vl_size     :24;    /* volume size =153502 */
	int             :16;    /* unused */
	int vl_vlsize   : 8;    /* volume label size in blocks =9 */
	int vl_blksize  : 8;    /* log2 (block size) =7 */
	int vl_model    : 8;    /* storage media model */
	int vl_vlcopy   :24;    /* block address of label copy */
	int vl_restrack :24;    /* 1st reserved track # */
	int vl_nreserved:16;    /* number of reserved tracks =114 */
	int vl_ptntab   :16;    /* partition table address =118 */
	int             :32;    /* unused */
	int vl_maxptn   :16;    /* maximum number of partitions =256 */
};

/* Variable part */

/* Partition table entry */

struct  pt_entry {
	int pt_name1       ;    /* partition name 8 chrs */
	int pt_offset   :24;    /* partition offset */
	int pt_version  : 8;    /* partition version */
	int pt_name2    :32;    /* 4 chars of name more */
	int pt_size     :24;    /* blocks in partition */
	int             :40;    /* unused */
	int reserved       ;    /* unused */
};

/* Формат таблицы дорожек замены */

struct btt_entry {
	int btt_badhead :16;    /* номер головки плохой дорожки */
	int btt_badcyl  :16;    /* номер цилиндра плохой дорожки */
	int btt_newhead :16;    /* номер головки дорожки замены */
	int btt_newcyl  :16;    /* номер цилиндра дорожки замены */
};

/* Volume label parameters */

# define VL_MAGIC       (*((int *)"БKB KCИД"))  /* "ДИCK BKБ" */
# define VL_VERSION     1
# define VL_REVISION    1
