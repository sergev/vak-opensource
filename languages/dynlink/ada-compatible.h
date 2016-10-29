/*
 * List of predefined primitive operations (from gcc/ada/exp_disp.ads)
 * Bracketed numerical values represent dispatch table slot numbers.
 */
class Ada_Compatible {
public:
	virtual int Size () = 0;
	/* _Size (1) - implementation of the attribute 'Size for any tagged
	 * type. Constructs of the form Prefix'Size are converted into
	 * Prefix._Size. */

	virtual int Alignment () = 0;
	/* _Alignment (2) - implementation of the attribute 'Alignment for
	 * any tagged type. Constructs of the form Prefix'Alignment are
	 * converted into Prefix._Alignment. */

	virtual void _TSS_Stream_Read () = 0;
	/* TSS_Stream_Read (3) - implementation of the stream attribute Read
	 * for any tagged type. */

	virtual void _TSS_Stream_Write () = 0;
	/* TSS_Stream_Write (4) - implementation of the stream attribute Write
	 * for any tagged type. */

	virtual void *_TSS_Stream_Input () = 0;
	/* TSS_Stream_Input (5) - implementation of the stream attribute Input
	 * for any tagged type. */

	virtual void _TSS_Stream_Output () = 0;
	/* TSS_Stream_Output (6) - implementation of the stream attribute
	 * Output for any tagged type. */

	virtual int _Op_Eq () = 0;
	/* Op_Eq (7) - implementation of the equality operator for any non-
	 * limited tagged type. */

	virtual void _Assign () = 0;
	/* _Assign (8) - implementation of the assignment operator for any
	 * non-limited tagged type. */

	virtual void _TSS_Deep_Adjust () = 0;
	/* TSS_Deep_Adjust (9) - implementation of the finalization operation
	 * Adjust for any non-limited tagged type. */

	virtual void _TSS_Deep_Finalize () = 0;
	/* TSS_Deep_Finalize (10) - implementation of the finalization
	 * operation Finalize for any non-limited tagged type. */

	virtual void _Disp_Asynchronous_Select () = 0;
	/* _Disp_Asynchronous_Select (11) - used in the expansion of ATC with
	 * dispatching triggers. Null implementation for limited interfaces,
	 * full body generation for types that implement limited interfaces,
	 * not generated for the rest of the cases. See Expand_N_Asynchronous_
	 * Select in Exp_Ch9 for more information. */

	virtual void _Disp_Conditional_Select () = 0;
	/* _Disp_Conditional_Select (12) - used in the expansion of conditional
	 * selects with dispatching triggers. Null implementation for limited
	 * interfaces, full body generation for types that implement limited
	 * interfaces, not generated for the rest of the cases. See Expand_N_
	 * Conditional_Entry_Call in Exp_Ch9 for more information. */

	virtual void _Disp_Get_Prim_Op_Kind () = 0;
	/* _Disp_Get_Prim_Op_Kind (13) - helper routine used in the expansion
	 * of ATC with dispatching triggers. Null implementation for limited
	 * interfaces, full body generation for types that implement limited
	 * interfaces, not generated for the rest of the cases. */

	virtual void _Disp_Get_Task_Id () = 0;
	/* _Disp_Get_Task_Id (14) - helper routine used in the expansion of
	 * Abort, attributes 'Callable and 'Terminated for task interface
	 * class-wide types. Full body generation for task types, null
	 * implementation for limited interfaces, not generated for the rest
	 * of the cases. See Expand_N_Attribute_Reference in Exp_Attr and
	 * Expand_N_Abort_Statement in Exp_Ch9 for more information. */

	virtual void _Disp_Timed_Select () = 0;
	/* _Disp_Timed_Select (15) - used in the expansion of timed selects
	 * with dispatching triggers. Null implementation for limited
	 * interfaces, full body generation for types that implement limited
	 * interfaces, not generated for the rest of the cases. See Expand_N_
	 * Timed_Entry_Call for more information. */

	inline void* Tag () { return *(void**) this; }
};

/*
 * Default implementation of predefined primitive operations.
 */
#define ADA_COMPATIBLE_IMPLEMENTATION \
	virtual int Size () { return sizeof (*this) * 8; } \
	virtual int Alignment () { return sizeof (void*); } \
	virtual void _TSS_Stream_Read () {} \
	virtual void _TSS_Stream_Write () {} \
	virtual void *_TSS_Stream_Input () { return 0; } \
	virtual void _TSS_Stream_Output () {} \
	virtual int _Op_Eq () { return 0; } \
	virtual void _Assign () {} \
	virtual void _TSS_Deep_Adjust () {} \
	virtual void _TSS_Deep_Finalize () {} \
	virtual void _Disp_Asynchronous_Select () {} \
	virtual void _Disp_Conditional_Select () {} \
	virtual void _Disp_Get_Prim_Op_Kind () {} \
	virtual void _Disp_Get_Task_Id () {} \
	virtual void _Disp_Timed_Select () {}
