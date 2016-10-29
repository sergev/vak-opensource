
      PARAMETER (MAXSEG=1500)	! Max number of segments 	(Windows-95 <= 4950)
      PARAMETER (MAXMAT=1500)	! max nr of 'in-core' alloc.	(MAXMAT <= MAXSEG)

      PARAMETER (LOADMX=MAXSEG/10)	! Max number of LD cards
      PARAMETER (NSMAX=99)	! Max number of EX cards
      PARAMETER (NETMX=128)	! Max number of segs connected to NT/TL

      PARAMETER (JMAX=60)	! Max segments connected to a single segment or junction
C      PARAMETER (mxsym=20)	! Max nr of symmetry planes (=max par in GR command)