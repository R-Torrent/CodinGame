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

// Particle Charge Mass(in MeV/c2), Symbol
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
	scanf("%f", &V); fgetc(stdin);

	char (*chamber)[w + 1] = malloc(h * (w + 1));
	for (int i = 0; i < h; i++)
		// lines of ASCII-art picture
		scanf("%[^\n]\n", chamber[i]);

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

int measure_radius(int h, int w, char (*chamber)[w + 1])
{
// CODE THIS FUNCTION <------------
	return 0;
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
// CODE BEHAVIOUR WHEN g_p == 0 (or close) <------------
		if ((e = fabsf(g_p[p] - G) / g_p[p]) < relative_error) {
			candidate = p;
			relative_error = e;
		}

	return candidate;
}
