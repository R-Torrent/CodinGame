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

int measure_radius(int, int, char (*)[*]);
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
	scanf("%f", &V);

	char (*chamber)[w + 1] = malloc(h * (w + 1));
	for (int i = 0; i < h; i++)
		// lines of ASCII-art picture
		fgets(chamber[i], w + 1, stdin);

	// Write an answer using printf(). DON'T FORGET THE TRAILING \n
	// To debug: fprintf(stderr, "Debug messages...\n");


	// measured radius (in m)
	const int R = measure_radius(h, w, chamber);

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
		printf("%s %i\n", symbol[p], R);
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

#define HYPOT(v) (hypotf((float)v.i, (float)v.j))

void capture_trajectory(point_t **, int, int, char (*)[*]);
void clear_trajectory(point_t **);
float cross_product(const vector_t *, const vector_t *);

int measure_radius(int h, int w, char (*chamber)[w + 1])
{
	point_t *trajectory = NULL;
	capture_trajectory(&trajectory, h, w, chamber);

	float r = 0.0F;
	int n = 0;
	// takes the overall average for all point triplets
	// using the formula R = abc / (4 * area_circumscribed_triangle_ABC)
	for (point_t *x = trajectory; x; x = x->next)
		for (point_t *y = x->next; y; y = y->next) {
			vector_t a = { y->i - x->i, y->j - x->j };
			float la = HYPOT(a);
			for (point_t *z = y->next; z; z = z->next) {
				vector_t b = { z->i - y->i, z->j - y->j };
				vector_t c = { x->i - z->i, x->j - z->j };
				float lb = HYPOT(b);
				float lc = HYPOT(c);
				r += la * lb * lc / fabsf(cross_product(&a, &c)) / 2;
				n++;
			}
		}

	clear_trajectory(&trajectory);
	return (int)round(r / n / 10) * 10;
}

#define tr(i, j) (chamber[(i)][(j)] == ' ')

void capture_trajectory(point_t **ptrajectory, int h, int w, char (*chamber)[w + 1])
{
	point_t *new, *last;

	for (int i = 0; i < h; i++) {
		int j = 0, j1, j2;
		while (j < w && !tr(i, j))
			j++;
		for (j1 = j; j < w && tr(i, j); j++)
			;
		for (j2 = j - 1; j2 >= j1; j2--) { // add to the head of the list
			new = malloc(sizeof(point_t));
			*new = (point_t){i, j2};
			new->next = *ptrajectory;
			*ptrajectory = new;
		}
		while (j < w && !tr(i, j))
			j++;
		while (j < w && tr(i, j)) { // add to the tail of the list
			new = malloc(sizeof(point_t));
			*new = (point_t){i, j++};
			if ((last = *ptrajectory)) {
				while (last->next)
					last = last->next;
				last->next = new;
			}
			else
				*ptrajectory = new;
		}
	}
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

inline float cross_product(const vector_t *a, const vector_t *b)
{
	return a->i * b->j - a->j * b->i;
}

enum particle determine_particle(float G)
{

#define X(a, b, c, d) (abs(b) / c ## F),
	float g_p[] = {
		PARTICLE_TABLE
	};
#undef X

	enum particle candidate = UNKNOWN;
	float relative_error = 0.5F;
	float e;
	for (enum particle p = 0; p < UNKNOWN; p++)
		if (!g_p[p]) {
			if (!G)
				return p;
		}
		else if ((e = fabsf(g_p[p] - G) / g_p[p]) < relative_error) {
			candidate = p;
			relative_error = e;
		}

	return candidate;
}
