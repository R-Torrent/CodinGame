#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Particle Detection with Cloud Chamber
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Detect which particle just passed through the cloud chamber,
 * if unknown, you may win the Nobel prize of physics!
 **/

// Particle Charge Mass(in MeV/c2) Symbol
#define PARTICLE_TABLE           \
X(ELECTRON, -1,    0.511, e-)    \
X(PROTON,   +1,  938.0,   p+)    \
X(NEUTRON,   0,  940.0,   n0)    \
X(ALPHA,    +2, 3727.0,   alpha) \
X(PION_PIP, +1,  140.0,   pi+)

#define X(a, b, c, d) a,
enum particle {
	PARTICLE_TABLE
	UNKNOWN
};
#undef X

#define X(a, b, c, d) #d,
char *symbol[] = {
	PARTICLE_TABLE
};
#undef X

// Lorentz factor
#define GAMMA(V) (1 / sqrtf((float)(1 - V * V)))

// speed-of-light (in m/s)
const int c = 299792458;

float measure_radius(int, int, char (*)[*]);
enum particle determine_particle(float);

int main()
{
	// width of ASCII-art picture (one meter per column)
	int w;
	scanf("%d", &w);
	// height of ASCII-art picture (one meter per line)
	int h;
	scanf("%d", &h);
	// strengh of magnetic field (tesla)
	float B;
	scanf("%f", &B);
	// speed of the particle (speed-of-light unit)
	float V;
	scanf("%f\n", &V);

	char (*chamber)[w + 1] = malloc(h * (w + 1));
	for (int i = 0; i < h; i++)
		// lines of ASCII-art picture
		scanf("%[^\n]\n", chamber[i]);

	// Write an answer using printf(). DON'T FORGET THE TRAILING \n
	// To debug: fprintf(stderr, "Debug messages...\n");

	// measured radius (in m)
	const float R = measure_radius(h, w, chamber);

	// experimental ratio G = abs(q) / m
	const float G = 1e6F * GAMMA(V) * V / (B * R * c);

	const enum particle p = determine_particle(G);

	// "<symbol> <radius>" if charged particle
	// "<symbol> inf" if neutral particle
	// "I just won the Nobel prize in physics !" if unknown particle
	switch (p) {
	case NEUTRON:
		printf("%s inf\n", symbol[p]);
		break;
	case UNKNOWN:
		printf("I just won the Nobel prize in physics !\n");
		break;
	default:
		printf("%s %i\n", symbol[p], (int)round(R / 10) * 10);
	};

	free(chamber);
	return 0;
}

typedef struct point_s {
	int i; // row
	int j; // column
	struct point_s *next;
} point_t;

typedef struct {
	int i; // vertical dimension
	int j; // horizontal dimension
} vector_t;

float radius_determination(point_t *, int *);
void clear_trajectory(point_t **);
int cross_product(const vector_t *, const vector_t *);

float measure_radius(int h, int w, char (*chamber)[w + 1])
{
	point_t *trajectory = NULL, *new;

	// capture the trajectory points; their order of insertion is irrelevant
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			if (chamber[i][j] == ' ') { // add to the head of the list
				new = malloc(sizeof(point_t));
				*new = (point_t){i, j};
				new->next = trajectory;
				trajectory = new;
			}

	// data is deemed admissible only from a subset of the available point triplets
	int admissible = -1;
	radius_determination(trajectory, &admissible);
	float result = radius_determination(trajectory, &admissible);

	clear_trajectory(&trajectory);
	return result;
}

#define HYPOT(v) hypotf((float)v.i, (float)v.j)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

float radius_determination(point_t *p, int *threshold)
{
	float s = 0.0F;
	int n = 0, max_cp = 0;

	// using the formula R = abc / (4 * area_circumscribed_triangle_ABC),
	// takes the average of only such point triplets forming triangles
	// within 60% of the largest triangles
	for (; p; p = p->next)
		for (point_t *q = p->next; q; q = q->next) {
			vector_t a = { q->i - p->i, q->j - p->j };
			float la = HYPOT(a);
			for (point_t *r = q->next; r; r = r->next) {
				vector_t b = { r->i - q->i, r->j - q->j };
				int cp = abs(a.i * b.j - a.j * b.i); // cross product a * b
				if (*threshold < 0)
					max_cp = MAX(cp, max_cp);
				else if (cp >= *threshold) {
					if (!cp)
						return INFINITY;
					vector_t c = { p->i - r->i, p->j - r->j };
					float lb = HYPOT(b);
					float lc = HYPOT(c);
					s += la * lb * lc / (cp << 1);
					n++;
				}
			}
		}
	if (*threshold < 0)
		*threshold = max_cp * 3 / 5;

	return s / n;
}

inline int cross_product(const vector_t *a, const vector_t *b)
{
	return a->i * b->j - a->j * b->i;
}

void clear_trajectory(point_t **ptrajectory)
{
	point_t *next;

	while (*ptrajectory) {
		next = (*ptrajectory)->next;
		free(*ptrajectory);
		*ptrajectory = next;
	}
}

enum particle determine_particle(float G)
{

#define X(a, b, c, d) (abs(b) / c ## F),
	float g_p[] = {
		PARTICLE_TABLE
	};
#undef X

	enum particle candidate = UNKNOWN;
	float e, min_relative_error = 0.5F;

	for (enum particle p = 0; p < UNKNOWN; p++)
		if (!g_p[p]) {
			if (!G)
				return p;
		}
		else if ((e = fabsf(g_p[p] - G) / g_p[p]) < min_relative_error) {
			candidate = p;
			min_relative_error = e;
		}

	return candidate;
}
