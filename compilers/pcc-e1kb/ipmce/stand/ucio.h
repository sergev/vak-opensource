# define UCREG(u)       (0x38|(u))

# define UC_RDY         0x20000         /* готовность */
# define UC_ACK         0x10000         /* квитанция */
# define UC_STROBE      0x800           /* строб данных */
# define UC_PARITY      0x400           /* четность данных */
# define UC_ID1         0x200           /* идентификатор 1 */
# define UC_ID0         0x100           /* идентификатор 0 */
# define UC_BYTE        0xff            /* данные */

# define UC_ID          (UC_ID0|UC_ID1) /* идентификатор */
# define UC_DATA        (UC_ID|UC_BYTE) /* данные */
# define UC_PDATA       (UC_PARITY|UC_DATA) /* данные и четность */

# define UC_OUTACK(u,r) out (UCREG(u), r|=UC_RDY|UC_ACK)
# define UC_OUTNACK(u,r) out (UCREG(u), r=UC_RDY|r&~UC_ACK)
# define UC_OUTDATA(u,a,r) out (UCREG(u), r=UC_STROBE|r&~UC_PDATA|(a)&UC_PDATA)
# define UC_OUTCLR(u,a,r) out (UCREG(u), r&=~(a))
