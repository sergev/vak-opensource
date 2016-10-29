#include "global.h"
#include "robot.h"

robot_t *robot_init (int nneurons, int nsynapses, int maxfactor)
{
	robot_t *r;
	neuron_t *n;
	synapse_t *s;

	assert (nneurons > 0);
	assert (nsynapses > 0);
	r = calloc (1, sizeof(robot_t) + sizeof(neuron_t) * nneurons);
	assert (r != 0);

	r->nneurons = nneurons;
	maxfactor *= 1000;
	for (n=r->neuron+4; n<r->neuron+nneurons; ++n) {
		n->synapse = calloc (nsynapses, sizeof(synapse_t));
		assert (n->synapse != 0);
		n->nsynapses = nsynapses;

		for (s=n->synapse; s<n->synapse+nsynapses; ++s) {
			s->factor = random() % (2*maxfactor) - maxfactor;
			s->neuron = r->neuron + random() % nneurons;
		}
	}
	return r;
}

void robot_free (robot_t *r)
{
	neuron_t *n;

	for (n=r->neuron; n<r->neuron+r->nneurons; ++n)
		free (n->synapse);
	free (r);
}

robot_t *robot_copy (robot_t *r)
{
	robot_t *q;
	synapse_t *s;
	int n;

	q = calloc (1, sizeof(robot_t) + sizeof(neuron_t) * r->nneurons);
	assert (q != 0);

	q->nneurons = r->nneurons;
	for (n=4; n<r->nneurons; ++n) {
		q->neuron[n].synapse = calloc (r->neuron[n].nsynapses,
			sizeof(synapse_t));
		assert (q->neuron[n].synapse != 0);
		q->neuron[n].nsynapses = r->neuron[n].nsynapses;
		memcpy (q->neuron[n].synapse, r->neuron[n].synapse,
			sizeof(synapse_t) * r->neuron[n].nsynapses);
		for (s = q->neuron[n].synapse; s < q->neuron[n].synapse +
		    q->neuron[n].nsynapses; ++s) {
			s->neuron += q->neuron - r->neuron;
		}
	}
	return q;
}

robot_t *robot_load (FILE *fd)
{
	int nneurons, i;
	robot_t *r;
	neuron_t *n;
	synapse_t *s;
	char line [1024], *line_good;

	line_good = fgets (line, sizeof (line), fd);
	assert (line_good);

	sscanf (line, "neurons %d", &nneurons);
	assert (nneurons > 0);
	r = calloc (1, sizeof(robot_t) + sizeof(neuron_t) * nneurons);
	assert (r != 0);

	r->nneurons = nneurons;
	for (n=r->neuron+4; n<r->neuron+nneurons; ++n) {
		fscanf (fd, "\n[%d] ", &n->nsynapses);
		assert (n->nsynapses > 0);

		n->synapse = calloc (n->nsynapses, sizeof(synapse_t));
		assert (n->synapse != 0);

		for (s=n->synapse; s<n->synapse+n->nsynapses; ++s) {
			fscanf (fd, "%d*%d ", &i, &s->factor);
			assert (i >= 0);
			assert (i < nneurons);
			s->neuron = r->neuron + i;
		}
	}
	return r;
}

void robot_save (robot_t *r, FILE *fd, int rating)
{
	neuron_t *n;
	synapse_t *s;

	fprintf (fd, "neurons %d rating %d\n", r->nneurons, rating);
	for (n=r->neuron+4; n<r->neuron+r->nneurons; ++n) {
		fprintf (fd, "[%d] ", n->nsynapses);
		for (s=n->synapse; s<n->synapse+n->nsynapses; ++s) {
			fprintf (fd, "%d*%d ", s->neuron - r->neuron,
				s->factor);
		}
		fprintf (fd, "\n");
	}
}

void robot_setup (robot_t *r, int x, int y)
{
	neuron_t *n;

	r->x = x;
	r->y = y;
	for (n=r->neuron; n<r->neuron+r->nneurons; ++n)
		n->state = 0;
}

void robot_think (robot_t *r, int level)
{
	neuron_t *n;
	synapse_t *s;
	long sum;

	level *= 1000;
	for (n=r->neuron+4; n<r->neuron+r->nneurons; ++n) {
		sum = 0;
		for (s=n->synapse; s<n->synapse+n->nsynapses; ++s) {
			sum += s->neuron->state * s->factor;
		}
		if (sum >= level)
			n->new_state = 1;
		else if (sum <= -level)
			n->new_state = -1;
		else
			n->new_state = 0;
	}
	for (n=r->neuron+4; n<r->neuron+r->nneurons; ++n) {
		n->state = n->new_state;
	}
}
