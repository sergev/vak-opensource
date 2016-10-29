typedef struct _neuron_t neuron_t;
typedef struct _synapse_t synapse_t;

struct _synapse_t {
	int		factor;
	neuron_t	*neuron;
};

struct _neuron_t {
	int		state;
	int		new_state;
	int		nsynapses;
	synapse_t	*synapse;
};

typedef struct _robot_t {
	int		x, y;
	int		nneurons;
	neuron_t	neuron [1];
} robot_t;

robot_t *robot_init (int nneurons, int nsynapses, int maxfactor);
robot_t *robot_load (FILE *fd);
robot_t *robot_copy (robot_t *r);
void robot_save (robot_t *r, FILE *fd, int rating);
void robot_free (robot_t *r);

void robot_setup (robot_t *r, int x, int y);
void robot_think (robot_t *r, int level);
