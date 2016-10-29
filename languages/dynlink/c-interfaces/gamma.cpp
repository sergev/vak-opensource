class Alpha {
public:
	int data_alpha;
	virtual void func_alpha () = 0;
};

class Beta {
public:
	int data_beta;
	virtual void func_beta () = 0;
};

class Gamma : public Alpha, public Beta {
public:
	int data_gamma;
	virtual void func_alpha () {
		asm ("# func_alpha");
	}
	virtual void func_beta () {
		asm ("# func_beta");
	}
	virtual void func_gamma () {
		asm ("# func_gamma");
	}
};

int func ()
{
	static Gamma x;

	x.data_alpha = 1;
	x.data_beta = 2;
	x.data_gamma = 3;
	return sizeof (x);
}
