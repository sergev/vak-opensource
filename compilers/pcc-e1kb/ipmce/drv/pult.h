/*
 *      Elbrus-B console Xenix driver.
 */

/* ioctls */

# define PTADDR         0x100   /* set read/write memory byte address */
# define PTSEEK         0x200   /* update memory byte address */
# define PTSETREG       0x400   /* set register */
# define PTGETREG       0x1000  /* get direct register */
# define PTGETINDX      0x2000  /* get indexed register */
