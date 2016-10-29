
      PARAMETER (MAXSEG=8000)	! Max number of segments 	(Windows-95 <= 4950)
      PARAMETER (MAXMAT=8000)	! max nr of 'in-core' alloc.	(MAXMAT <= MAXSEG)

      PARAMETER (LOADMX=maxseg/10)	! Max number of LD cards
      PARAMETER (NSMAX=128)	! Max number of EX cards
      PARAMETER (NETMX=256)	! Max number of segs connected to NT/TL 

      PARAMETER (JMAX=60)	! Max segments connected to a single segment or junction
