/**
 * TODO :
 *
 * Continue with the hash_sets and convert the vacant_slots in 'struct body' to one. Redo that logic so
 * all vacant slots next to the organism are added, not just the (currently) wdistance == 1.
 *
 * Add to the bodies new lists with line-of-sight from a SPORER or a vacant slot to each of the four
 * !MY_HARVESTED sources or enemy organisms. Spore must land two actual tiles away from the target. The
 * same organism could have a varios potential locations for the SPORER or where to land. Minimize
 * (first) the wdistance {from organ to new SPORER + from new ROOT to target}; maxmize (second) the
 * length of the shot.
 * 
 * Multi-agent AI : https://en.wikipedia.org/wiki/Multi-agent_system
 **/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Grow and multiply your organisms to end up larger than your opponent.
 **/

/*
 * Cellularena - Winter Challenge 2024
 * Contest
 */

/*
 * Wood 1 League
 */

// comment-out the following macros when the program is ready
#define DEBUG_WORLD_BUILDING
#define DEBUG_AI

// linked list, hash map, and hash set functions  -------------------------------------------------------

#define NO_INDEX -1

typedef struct node_s {
	void *key;
	void *content;
	struct node_s *next;
} node_t;

typedef struct {
	int size;
	node_t *node;
	void (*del_content)(void *);
} list_t;

typedef struct associative_array_s {
	size_t capacity;
	size_t sizeof_key;
	node_t **array;
	void (*del_key)(void *);
	void (*del_content)(void *);
	size_t (*hash)(struct associative_array_s *, void *);
	int (*equals)(void *, void *, size_t);
} hash_map_t, hash_set_t;

node_t *new_node(void *key, void *content, node_t *next)
{
	node_t *new = malloc(sizeof(node_t));

	new->key = key;
	new->content = content;
	new->next = next;

	return new;
}

void delete_nodes(node_t **pn, void (*del_key)(void *), void (*del_content)(void *))
{
	node_t *next;

	while (*pn) {
		next = (*pn)->next;
		if (del_key)
			del_key((*pn)->key);
		if (del_content)
			del_content((*pn)->content);
		free(*pn);
		*pn = next;
	}
}

list_t *create_list(void (*del_content)(void *))
{
	list_t *list = malloc(sizeof(list_t));

	list->size = 0;
	list->node = NULL;
	list->del_content = del_content;

	return list;
}

void add_front_list(list_t *list, void *content)
{
	if (list) {
		list->node = new_node(NULL, content, list->node);
		list->size++;
	}
}

void iterate_over_list(void *data, list_t *list, void (*action)(void *, void *))
{
	if (list)
		for (node_t *n = list->node; n; n = n->next)
			action(data, n->content);
}

void sort_list(void *data, list_t *list, int (*compare)(void *, void *, void *))
{
	if (list)
		for (node_t *n0 = list->node; n0 != NULL; n0 = n0->next)
			for (node_t *n1 = n0->next; n1 != NULL; n1 = n1->next)
				if (compare(data, n0->content, n1->content) > 0) {
					void *temp = n0->content;
					n0->content = n1->content;
					n1->content = temp;
				}
}

// if predicate == NULL, returns index of the first occurrence of data in list
ptrdiff_t find_first_in_list(void *data, list_t *list, int (*predicate)(void *, void *))
{
	if (list) {
		ptrdiff_t f = 0;

		for (node_t *n = list->node; n; n = n->next, f++)
			if (predicate && predicate(data, n->content) || !predicate && data == n->content)
				return f;
	}

	return NO_INDEX;
}

void clear_list(list_t *list)
{
	if (list) {
		delete_nodes(&list->node, NULL, list->del_content);
		list->size = 0;
	}
}

void delete_list(list_t **plist)
{
	if (*plist) {
		clear_list(*plist);
		free(*plist);
		*plist = NULL;
	}
}

size_t default_hash(hash_map_t *map, void *key)
{
	return *(size_t *)key % map->capacity;
}

int default_equals(void *a, void *b, size_t size)
{
	return !memcmp(a, b, size);
}

hash_map_t *create_hash_map(size_t capacity, size_t sizeof_key, void (*del_key)(void *),
        void (*del_content)(void *), size_t (*hash)(hash_map_t *, void *),
        int (*equals)(void *, void *, size_t))
{
	hash_map_t *map = malloc(sizeof(hash_map_t));

	map->capacity = capacity;
	map->sizeof_key = sizeof_key;
	map->array = malloc(capacity * sizeof(node_t *));
	for (size_t i = 0; i < capacity; i++)
		map->array[i] = NULL;
	map->del_key = del_key;
	map->del_content = del_content;
	map->hash = !hash ? default_hash : hash;
	map->equals = !equals ? default_equals : equals;

	return map;
}

node_t *get_node_map(hash_map_t *map, void *key)
{
	if (map && key)
		for (node_t *node = map->array[map->hash(map, key)]; node; node = node->next)
			if (map->equals(node->key, key, map->sizeof_key))
				return node;

	return NULL;
}

void *get_map(hash_map_t *map, void *key)
{
	node_t *node = get_node_map(map, key);

	if (node)
		return node->content;

	return NULL;
}

// replaces old value if the key mapped some previous content
void put_map(hash_map_t *map, void *key, void *content)
{
	if (map && key) {
		node_t *old = get_node_map(map, key);

		if (old) {
			if (map->del_key)
				map->del_key(old->key);
			if (map->del_content)
				map->del_content(old->content);
			old->key = key;
			old->content = content;
		}
		else {
			size_t index = map->hash(map, key);

			map->array[index] = new_node(key, content, map->array[index]);
		}
	}
}

int size_map(hash_map_t *map)
{
	if (map) {
		int size = 0;

		for (node_t **pn = map->array; pn - map->array < map->capacity; pn++)
			for (node_t *n = *pn; n; n = n->next)
				size++;
	}

	return -1;
}

void clear_map(hash_map_t *map)
{
	if (map)
		for (node_t **pn = map->array; pn - map->array < map->capacity; pn++)
			delete_nodes(pn, map->del_key, map->del_content);
}

void delete_map(hash_map_t **pmap)
{
	if (*pmap) {
		clear_map(*pmap);
		free(*pmap);
		*pmap = NULL;
	}
}

hash_set_t *create_hash_set(int capacity, size_t sizeof_element, void (*del_element)(void *),
		size_t (*hash)(hash_set_t *, void *), int (*equals)(void *, void *, size_t))
{
	return create_hash_map(capacity, sizeof_element, del_element, NULL, hash, equals);
}

int contains_set(hash_set_t *set, void *element)
{
	return get_node_map(set, element);
}

int add_set(hash_set_t *set, void *element)
{
	if (set && !contains_set(set, element)) {
		size_t index = set->hash(set, element);

		set->array[index] = new_node(element, NULL, set->array[index]);

		return 1;
	}

	return 0;
}

int size_set(hash_set_t *set)
{
	return size_map(set);
}

void clear_set(hash_set_t *set)
{
	clear_map(set);
}

void delete_set(hash_set_t **pset)
{
	delete_map(pset);
}

// tile-related auxiliary functions  --------------------------------------------------------------------

#define TYPE \
X(A)         \
X(B)         \
X(C)         \
X(D)         \
X(EMPTY)     \
X(WALL)      \
X(ROOT)      \
X(BASIC)     \
X(TENTACLE)  \
X(HARVESTER) \
X(SPORER)

#define X(a) a,
enum type {
	TYPE
};
#undef X

#define X(a) #a,
char *type_str[] = {
	TYPE
};
#undef X

#define DIR \
Y(N)        \
Y(E)        \
Y(S)        \
Y(W)        \
Y(X)

#define Y(a) a,
enum dir {
	DIR
};
#undef Y

#define Y(a) #a,
char *dir_str[] = {
	DIR
};
#undef Y

#define OWNERSHIP \
Y(MY)             \
Y(OPP)            \
Y(FREE)

#define Y(a) a,
enum ownership {
	OWNERSHIP
};
#undef Y

size_t determine_enum(char *options[], char *selection)
{
	for (char **o = options; ; o++)
		if (!strcmp(*o, selection))
			return o - options;
	// no error control!
}

// status flags for the tiles
// if A, B, C, D, or EMPTY then 0, 1 otherwise
#define OCCUPIED      0001
// faced by a harvester of mine
#define MY_HARVESTED  0002
// faced by an opponent's harvester
#define OPP_HARVESTED 0004
// protected by my tentacles
#define PROTECTED     0010
// menaced by opponent tentacles
#define MENACED       0020
// is type BASIC, HARVESTER, TENTACLE, SPORER, or ROOT
#define ISORGAN       0040
// is a protein source
#define ISPROTEIN     0100
// ownership; is mine
#define MINE          0200
// ownership; is opponent's
#define OPPT          0400

typedef struct vertex {
	// orientation of the vertex
	enum dir d;
	// tile where this vertex lies
	struct entity *e;
	// adjacent vertices
	struct vertex *a[4];
} vertex_t;

// record kept of all tiles
typedef struct entity {
	// grid coordinates
	int x;
	int y;
	// A, B, C, D, EMPTY, WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER
	enum type t;
	// id of this entity if it's an organ, 0 otherwise
	int organ_id;
	// N, E, S, W or X if not an organ
	enum dir d;
	int organ_parent_id;
	int organ_root_id;
	unsigned status;
	int value;
	// N, E, S, W for lateral ending vertices in OCCUPIED tiles only;
	// X for a central connecting vertex on an UNOCCUPIED tile
	vertex_t v[5];
} entity_t;

// columns in the game grid
int width;
// rows in the game grid
int height;
// NT = width * height
int NT;

#define EXPAND(a, b, c) ((a) * (c) + (b))
#define EXP1(a, b) EXPAND(a, b, height)
#define EXP2(x1, y1, x2, y2) EXPAND(EXP1(x1, y1), EXP1(x2, y2), NT)
#define INDEX1(a) EXP1((a)->x, (a)->y)
#define INDEX2(a, b) EXP2((a)->x, (a)->y, (b)->x, (b)->y)

size_t entity_hash(hash_set_t *set, void *entity)
{
	return INDEX1((entity_t *)entity) % set->capacity;
}

void init_grid(entity_t tiles[width][height])
{
	for (entity_t *e = *tiles; e - *tiles < NT; e++) {
		e->x = x;
		e->y = y;
		for (enum dir d = N; d <= X; d++) {
			e->v[d].d = d;
			e->v[d].e = e;
			e->v[d].a = { };
		}
	}
}

void reset_grid(entity_t tiles[width][height])
{
	for (entity_t *e = *tiles; e - *tiles < NT; e++) {
		e->t = EMPTY;
		e->organ_id = 0;
		e->d = X;
		e->organ_parent_id = 0;
		e->organ_root_id = 0;
		e->status = 0;
		e->value = 0;
	}
}

#define STATUS(A, B, C, D)                                                                  \
	if (A && tiles[B][C].d == D) {                                                        \
		if (tiles[B][C].t == HARVESTER)                                                     \
			tiles[x][y].status |= tiles[B][C].status & MINE ? MY_HARVESTED : OPP_HARVESTED; \
		else if (tiles[B][C].t == TENTACLE)                                                 \
			tiles[x][y].status |= tiles[B][C].status & MINE ? PROTECTED : MENACED;          \
	}

void determine_status(entity_t tiles[width][height])
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			STATUS(y < height - 1, x, y + 1, N);
			STATUS(x > 0, x - 1, y, E);
			STATUS(y > 0, x, y - 1, S);
			STATUS(x < width - 1, x + 1, y, W);

			enum type t = tiles[x][y].t;

			if (t == A || t == B || t == C || t == D)
				tiles[x][y].status |= ISPROTEIN;
			else if (t == BASIC || t == HARVESTER || t == TENTACLE || t == SPORER || t == ROOT)
				tiles[x][y].status |= ISORGAN | OCCUPIED;
			else if (t == WALL)
				tiles[x][y].status |= OCCUPIED;
		}
}

void assess_tiles(entity_t tiles[width][height], hash_map_t *organs,
		int harvested_sources[2][4], list_t *available_sources[4])
{
	for (entity_t *e = *tiles; e - *tiles < NT; e++) {
		enum type t = entity->t;

		// single-tile value = # of resorces required to build
		int value = t == BASIC ? 1 :
			t == HARVESTER || t == TENTACLE || t == SPORER ? 2 :
			t == ROOT ? 3 : 0;

		// determine protein sources
		if (e->status & ISPROTEIN) {
			if (e->status & MY_HARVESTED)
				harvested_sources[MY][t]++;
			else
				add_front_list(available_sources[t], e);
			if (e->status & OPP_HARVESTED)
				harvested_sources[OPP][t]++;
		}
		// determine organ value
		// value = its own + that of all its descendants
		else if (value) {
			entity_t *e1 = e;

			while (e1->organ_parent_id) {
				e1->value += value;
				e1 = get_map(organs, &e1->organ_parent_id);
			}
			e1->value += value;
		}
	}
}

struct array_v {
	size_t size;
	vertex_t *array[];
};

#define LINK_ADJACENT(A, B, C, D)                           \
	e1 = &tiles[A][B];                                      \
	v = &e->v[e->status & OCCUPIED ? C : X];                \
	if (!(e1->status & OCCUPIED))                           \
		v->a[C] = e1->v[X];                                 \
	else if (e1->t != WALL &&                               \
			(!(e->status & OCCUPIED) || e1->status & MINE)) \
		v->a[C] = e1->v[D];

// note that these are not all the vertices of the grid: only those that might be
// part of a new, clear and unbroken path set from one of my organs to some target
struct array_v *generate_vertices(entity_t tiles[width][height])
{
	list_t *temp = create_list(NULL);

	for (int x = 1; x < width - 1; x++)
		for (int y = 1; y < height - 1; y++) {
			entity_t *e = &tiles[x][y], *e1;
			vertex_t *v;

			if (e->t == WALL)
				continue;
			LINK_ADJACENT(x, y - 1, N, S);
			LINK_ADJACENT(x + 1, y, E, W);
			LINK_ADJACENT(x, y + 1, S, N);
			LINK_ADJACENT(x - 1, y, W, E);
			for (enum dir d = N; d <= X; d++)
				if (e->v->a[d]) {
					add_front_list(temp, e->v);
					break;
				}
		}

	struct array_v *array = malloc(sizeof(struct array_v) + temp->size * sizeof(vertex_t *));

	array->size = temp->size;
	vertex_t **p = array->array;
	for (node_t *node = temp->node; node; node = node->next)
		*p++ = node->content;
	delete_list(&temp);

	return array;
}

// distance metrics
#define WEIGHTED 0
#define TAXICAB  1

#define RESTRICTIONS \
Y(NORMAL)            \
Y(FORBIDDEN)         \
Y(RESTRICTED_A)      \
Y(RESTRICTED_B)      \
Y(RESTRICTED_C)      \
Y(RESTRICTED_D)      \
Y(REWARDED_A)        \
Y(REWARDED_B)        \
Y(REWARDED_C)        \
Y(REWARDED_D)

#define Y(a) a,
enum restrictions {
	RESTRICTIONS
};
#undef Y

#define Y(a) #a,
char *restrictions_str[] = {
	RESTRICTIONS
};
#undef Y

// weights the restriction levels impose on the pathing algorithm
int w[sizeof(restrictions_str) / sizeof(restrictions_str[0])];

// base weights for the pathing
#define BNORMAL     10
#define BFORBIDDEN  10000
// deviations
#define BRESTRICTED +15
#define BREWARDED   -2

void weigh_restriction_levels(int turn, int harvested_sources[2][4])
{
	// preciousness of the resource
	int prec[2][4];

	for (enum type t = A; t <= D; t++) {
		prec[MY][t] = (harvested_sources[MY][t] <= 1 ? 2 : 1) * (turn >= 97 ? 0 : 1);
		prec[OPP][t] = (harvested_sources[OPP][t] <= 1 ? 2 : 1) * (turn == 100 ? 0 : 1);
	}

	w[NORMAL]       = BNORMAL;
	w[FORBIDDEN]    = BFORBIDDEN;
	w[RESTRICTED_A] = BRESTRICTED * prec[MY][A];
	w[RESTRICTED_B] = BRESTRICTED * prec[MY][B];
	w[RESTRICTED_C] = BRESTRICTED * prec[MY][C];
	w[RESTRICTED_D] = BRESTRICTED * prec[MY][D];
	w[REWARDED_A]   = BREWARDED * prec[OPP][A];
	w[REWARDED_B]   = BREWARDED * prec[OPP][B];
	w[REWARDED_C]   = BREWARDED * prec[OPP][C];
	w[REWARDED_D]   = BREWARDED * prec[OPP][D];
}

// Floyd_Warshall algorithm functions  ------------------------------------------------------------------

int adjacent_vertices(vertex_t *v1, vertex_t *v2)
{
	for (vertex_t **pa = v1->a; pa - v1->a < 4; pa++)
		if (*pa == v2)
			return 1;

	return 0;
}

// [https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm]
void Floyd_Warshall(entity_t tiles[width][height], struct array_v *vertices, int *dist, entity_t **prev,
		int metric)
{
	const size_t NV = vertices->size;
	int (*distances_v)[NV] = malloc(NV * NV * sizeof(int));
	vertex_t *(*previous_v)[NV] = malloc(NV * NV * sizeof(vertex_t *));

	// initialization of Floyd-Warshall matrices & restrict areas ahead of a tentacle
	// (matrix indices range over all vertices)
	for (int i = 0; i < NV; i++) {
		vertex_t *vi = vertices->array[i];
		entity_t *ei = vi->e;

		for (int j = 0; j < i; j++) {
			vertex_t *vj = vertices->array[j];
			entity_t *ej = vj->e;

			// for each edge
			if (adjacent_vertices(vi, vj)) {
				if (ei->status & MENACED || ej->status & MENACED)
					distances_v[i][j] = w[FORBIDDEN];
				else if (metric == WEIGHTED) {
					distances_v[i][j] = w[NORMAL];
					if (ei->status & MY_HARVESTED)
						distances_v[i][j] +=
								ei->t == A ? w[RESTRICTED_A] :
								ei->t == B ? w[RESTRICTED_B] :
								ei->t == C ? w[RESTRICTED_C] :
								w[RESTRICTED_D];
					if (ej->status & MY_HARVESTED)
						distances_v[i][j] +=
								ej->t == A ? w[RESTRICTED_A] :
								ej->t == B ? w[RESTRICTED_B] :
								ej->t == C ? w[RESTRICTED_C] :
								w[RESTRICTED_D];
					if (ei->status & OPP_HARVESTED)
						distances_v[i][j] +=
								ei->t == A ? w[REWARDED_A] :
								ei->t == B ? w[REWARDED_B] :
								ei->t == C ? w[REWARDED_C] :
								w[REWARDED_D];
					if (ej->status & OPP_HARVESTED)
						distances_v[i][j] +=
								ej->t == A ? w[REWARDED_A] :
								ej->t == B ? w[REWARDED_B] :
								ej->t == C ? w[REWARDED_C] :
								w[REWARDED_D];
				}
				else // "taxicab" metric
					distances_v[i][j] = 1;
				previous_v[i][j] = vi;
			}
			else {
				distances_v[i][j] = w[FORBIDDEN];
				previous_v[i][j] = NULL;
			}
		}
		// for each vertex
		distances_v[i][i] = 0;
		previuous_v[i][i] = vi;
	}

	// Floyd_Warshall algorithm
	// (matrix indices range over all vertices)
	for (int k = 0; k < NV; k++)
		for (int i = 0; i < NV; i++)
			for (int j = 0; j < NV; j++)
				if (distances_v[i][j] > distances_v[i][k] + distances_v[k][j]) {
					distances_v[i][j] = distances_v[i][k] + distances_v[k][j];
					previous_v[i][j] = previous_v[k][j];
				}

	// determines best result for all vertices within each tile
	// (matrix indices range over all tiles)
	for (int i = 0; i < NT * NT; i++) {
		dist[i] = w[FORBIDDEN];
		prev[i] = NULL;
	}
	for (int i = 0; i < NV; i++) {
		entity_t *ei = vertices->array[i]->e;

		for (int j = 0; j < NV; j++) {
			entity_t *ej = vertices->array[j]->e;
			int index = INDEX2(ei, ej);

			if (distances_v[i][j] < dist[index]) {
				dist[index] = distances_v[i][j];
				prev[index] = previous_v[i][j]->e;
			}
		}
	}

	free(distances_v);
	free(previous_v);
}

// shortest path reconstruction from the Floyd-Warshall alorithm
list_t *path_FW(struct entity tiles[width][height],
		struct entity **prev, int x1, int y1, int x2, int y2)
{
	if (!prev[EXP2(x1, y1, x2, y2)])
		return NULL;

	list_t *path = create_list(NULL);

	add_front_list(path, &tiles[x2][y2]);
	while (x1 != x2 || y1 != y2) {
		struct entity *e = prev[EXP2(x1, y1, x2, y2)];

		x2 = e->x;
		y2 = e->y;
		add_front_list(path, &tiles[x2][y2]);
	}

	return path;
}

// body-related auxiliary functions  --------------------------------------------------------------------

struct body {
	entity_t *root;
	list_t *body_parts;

	// shortest weighted and "taxicab" distance to the organism...
	int (*distance_to_organism)[];
	// ...and closest (weighted and "taxicab") organ for all tiles
	entity_t *(*closest_organ)[];

	// tiles of interest
	list_t *accessible_organs;
	list_t *free_sources[4];
	hash_set_t *vacant_slots;

	// tactical commands
	struct command *orders;
};

struct opp_body {
	struct entity *root;
	list_t *body_parts;

	// shortest distance to my organs
	// (stores the simple index in the distance_to_organism[WEIGHTED] and closest_organ[WEIGHTED]
	// matrices of body, for each such body)
	hash_map_t *closest_index;
};

struct command {
	struct entity *target;
	struct entity *from_organ;
	struct entity *at_location;
	enum type new_organ_type;
	enum dir new_organ_dir;
	list_t *path;
};

void del_inner_body(void *body)
{
	struct body *b = (struct body *)body;

	delete_list(&b->body_parts);
	free(b->distance_to_organism);
	free(b->closest_organ);
	delete_list(&b->accessible_organs);
	delete_set(&b->vacant_slots);
	for (list_t **l = b->free_sources; l - b->free_sources <= D; l++)
		delete_list(l);
	delete_list(&b->orders->path);
	free(b->orders);
	free(body);
}

void del_opp_inner_body(void *opp_body)
{
	struct opp_body *opp_b = (struct opp_body *)opp_body;

	delete_list(&opp_b->body_parts);
	delete_map(&opp_b->closest_index);
	free(opp_body);
}

void populate_organisms(int n[2], hash_map_t **porganisms, hash_map_t **popp_organisms,
		hash_map_t *organs)
{
	*porganisms = create_hash_map(n[MY], sizeof(int), free, del_inner_body, NULL, NULL);
	*popp_organisms = create_hash_map(n[OPP], sizeof(int), free, del_opp_inner_body, NULL, NULL);

	// my_organisms
	for (int i = 0; i < n[MY]; i++) {
		struct body *new = malloc(sizeof(struct body));
		int *container_i = malloc(sizeof(int));
		int (*distances)[NT * sizeof(int)] = new->distance_to_organism
				= malloc(2 * NT * sizeof(int));
		struct entity *(*closest)[NT * sizeof(struct entity *)] = new->closest_organ
				= malloc(2 * NT * sizeof(struct entity *));

		new->body_parts = create_list(NULL);
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				distances[WEIGHTED][EXP1(x, y)] = w[FORBIDDEN];
				closest[WEIGHTED][EXP1(x, y)] = NULL;
				distances[TAXICAB][EXP1(x, y)] = w[FORBIDDEN];
				closest[TAXICAB][EXP1(x, y)] = NULL;
			}
		new->accessible_organs = create_list(NULL);
		new->vacant_slots = create_hash_set(NT, sizeof(struct entity), NULL, entity_hash, NULL);
		for (list_t **l = new->free_sources; l - new->free_sources <= D; l++)
			*l = create_list(NULL);

		*container_i = i;

		put_map(*porganisms, container_i, new);
	}

	// opp_organisms
	for (int i = 0; i < n[OPP]; i++) {
		struct opp_body *new = malloc(sizeof(struct opp_body));
		int *container_i = malloc(sizeof(int));

		new->body_parts = create_list(NULL);
		new->closest_index = create_hash_map(n[MY], sizeof(int), free, free, NULL, NULL);
		for (int j = 0; j < n[MY]; j++) {
			int *container_j = malloc(sizeof(int));
			int *index = malloc(sizeof(int));

			*container_j = j;
			*index = NO_INDEX;

			put_map(new->closest_index, container_j, index);
		}

		*container_i = i;

		put_map(*popp_organisms, container_i, new);
	}

	// establish the root organs
	int key_organism[2] = {0, 0};
	for (int i = 0; i < organs->capacity; i++) {
		struct entity *entity = get_map(organs, &i);

		if (entity && entity->t == ROOT) {
			if (entity->o == MY) {
				((struct body *)get_map(*porganisms, key_organism + MY))->root = entity;
				key_organism[MY]++;
			}
			else {
				((struct opp_body *)get_map(*popp_organisms, key_organism + OPP))->root = entity;
				key_organism[OPP]++;
			}
		}
	}

	// fill-in the body parts
	for (int i = 0; i < organs->capacity; i++) {
		struct entity *entity = get_map(organs, &i);

		if (entity) {
			if (entity->o == MY)
				for (int j = 0; ; j++) {
					struct body *body = get_map(*porganisms, &j);

					if (body->root->organ_id == entity->organ_root_id) {
						add_front_list(body->body_parts, entity);
						break;
					}
				}
			else
				for (int j = 0; ; j++) {
					struct opp_body *opp_body = get_map(*popp_organisms, &j);

					if (opp_body->root->organ_id == entity->organ_root_id) {
						add_front_list(opp_body->body_parts, entity);
						break;
					}
				}
		}
	}
}

int compare_highest_value(void *data, struct entity *e0, struct entity *e1)
{
	(void)data;

	return e1->value - e0->value;
}

int compare_closest_from_myroot(struct entity *my_root, struct entity *e0, struct entity *e1)
{
	// "taxicab" geometry
	return abs(e0->x - my_root->x) + abs(e0->y - my_root->y)
			- (abs(e1->x - my_root->x) + abs(e1->y - my_root->y));
}

int compare_closest(struct body *body, struct entity *e0, struct entity *e1)
{
	int (*distance_to_organism)[NT * NT * sizeof(int)] = body->distance_to_organism;

	return distance_to_organism[WEIGHTED][INDEX1(e0)] - distance_to_organism[WEIGHTED][INDEX1(e1)];
}

struct container1 {
	int (*distances)[];
	struct body *body;
	int x, y;
	int index;
};

void closest_points(struct container1 *data, struct entity *e)
{
	int (*distances)[NT * NT * sizeof(int)] = data->distances;
	int (*min_distances)[NT * sizeof(int)] = data->body->distance_to_organism;
	struct entity *(*closest)[NT * sizeof(struct entity *)] = data->body->closest_organ;
	int d;

	d = distances[WEIGHTED][INDEX2(e, data)];
	if (d < min_distances[WEIGHTED][data->index]) {
		min_distances[WEIGHTED][data->index] = d;
		closest[WEIGHTED][data->index] = e;
	}
	d = distances[TAXICAB][INDEX2(e, data)];
	if (d < min_distances[TAXICAB][data->index]) {
		min_distances[TAXICAB][data->index] = d;
		closest[TAXICAB][data->index] = e;
	}
}

void find_min_distance(int *data[2], struct entity *e)
{
	int *closest_index = data[0];
	int *distances = data[1];
	int index = INDEX1(e);
	int min_d = *closest_index == NO_INDEX ? w[FORBIDDEN] : distances[*closest_index];
	int d = distances[index];

	if (d < min_d)
		*closest_index = index;
}

// tabulate my organism's disposition to engage directly
void inspect_surroundings(struct entity tiles[width][height], int (*distances)[], hash_map_t *organisms,
		hash_map_t *opp_organisms)
{
	int (*distance_to_organism)[NT * sizeof(int)];

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			int index = EXP1(x, y);

			for (int i = 0; i < organisms->capacity; i++) {
				struct body *body = get_map(organisms, &i);

				// calculate the closest points to my organisms
				iterate_over_list(&(struct container1){distances, body, x, y, index},
						body->body_parts, (void (*)(void *, void *))closest_points);

				// fill-in lists of interesting tiles
				distance_to_organism = body->distance_to_organism;
				if (distance_to_organism[WEIGHTED][index] < w[FORBIDDEN]) {
					if (tiles[x][y].o == OPP)
						add_front_list(body->accessible_organs, &tiles[x][y]);
					else if (tiles[x][y].status & ISPROTEIN && !(tiles[x][y].status & MY_HARVESTED))
						add_front_list(body->free_sources[tiles[x][y].t], &tiles[x][y]);
				}
				if (!(tiles[x][y].status & OCCUPIED) && distance_to_organism[TAXICAB][index] == 1)
					add_set(body->vacant_slots, &tiles[x][y]);
			}
		}

	for (int i = 0; i < organisms->capacity; i++) {
		struct body *body = get_map(organisms, &i);

		// figure the index of the closest distance to all opponents
		for (int j = 0; j < opp_organisms->capacity; j++) {
			struct opp_body *opp_body = get_map(opp_organisms, &j);
			int *closest_index = get_map(opp_body->closest_index, &i);

			distance_to_organism = body->distance_to_organism;
			iterate_over_list((int *[]){closest_index, distance_to_organism[WEIGHTED]},
					opp_body->body_parts, (void (*)(void *, void *))find_min_distance);
		}

		sort_list(NULL, body->accessible_organs,
				(int (*)(void *, void *, void *))compare_highest_value);
		for (list_t **l = body->free_sources; l - body->free_sources <= D; l++)
			sort_list(body, *l, (int (*)(void *, void *, void *))compare_closest);
	}
}

struct body *whose_body(hash_map_t *organisms, struct entity *e)
{
	for (int i = 0; i < organisms->capacity; i++) {
		struct body *body = get_map(organisms, &i);

		if (find_first_in_list(e, body->body_parts, NULL) != NO_INDEX)
			return body;
	}

	return NULL;
}

struct shooting_lane {
	// shooting SPORER, NULL if none in place yet
	struct entity *sporer;
	// tile aimed at
	struct entity *place;
	// direction of shot
	enum dir d;
	// shot length
	int l;
	// wdistance of paths involved from the plcaing position to the target, and with the new SPORER
	int wdistance;
};

struct container2 {
	void *tiles;
	int (*distances)[];
	hash_map_t *organisms;
};

void check_place(struct container2 *data, int x, int y)
{
	struct entity (*tiles)[height] = (struct entity (*)[height])data->tiles;
	int x1, y1;

	if (x >=0 && x < width && y >= 0 && y < height && !(tiles[x][y].status & OCCUPIED)) {
		// check for existing N sporers
		for (y1 = y - 1; y1 < height && !(tiles[x][y1].status & OCCUPIED); y1++)
			;
		if (y1 < height && tiles[x][y1].t == SPORER && tiles[x][y1].o == MY && tiles[x][y1].d == N)
			;
	}
}

void aim_on_source(struct container2 *data, struct entity *target)
{
	// eight potential landing tiles for a spore
	check_place(data, target->x, target->y - 2);
	check_place(data, target->x - 1, target->y - 1);
	check_place(data, target->x + 1, target->y - 1);
	check_place(data, target->x - 2, target->y);
	check_place(data, target->x + 2, target->y);
	check_place(data, target->x - 1, target->y + 1);
	check_place(data, target->x + 1, target->y + 1);
	check_place(data, target->x, target->y + 2);

 /*
	for (int i = 0; data->organisms->capacity; i++) {
		struct body *body = get_map(data->organisms, i);


	}
*/

}

void aim_sporers(struct entity tiles[width][height], list_t *free_sources[4], int (*distances)[],
		hash_map_t *organisms, hash_map_t *opp_organisms)
{
	// aim on FREE (== !MY_HARVESTED) sources
	for (enum type t = A; t <= D; t++)
		iterate_over_list(&(struct container2){tiles, distances, organisms},
				free_sources[t], (void (*)(void *, void *))aim_on_source);
}

// functions for the tactical overmind  -----------------------------------------------------------------

void order_organisms(hash_map_t *organisms)
{
	for (int i = 0; i < organisms->capacity; i++) {
		struct command *orders = malloc(sizeof(struct command));

		((struct body *)get_map(organisms, &i))->orders = orders;
		orders->target = orders->from_organ = orders->at_location = NULL;
		orders->new_organ_type = BASIC;
		orders->new_organ_dir = N;
		orders->path = create_list(NULL);
	}
}

enum dir face_to(struct entity *subject, struct entity *object)
{
	if (object == subject)
		return X;
	if (object->x == subject->x)
		return object->y < subject->y ? N : S;
	if (object->y == subject->y)
		return object->x < subject->x ? W : E;
	return X;
}

// debugging assists  -----------------------------------------------------------------------------------

void print_entities(entity_t tiles[width][height], list_t *available_sources[4])
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			entity_t *e = &tiles[x][y];

			if (e->t == WALL || e->t == EMPTY)
				continue;
			fprintf(stderr, "(%d, %d) %s %s owner:%s id:%d parent_id:%d root_id:%d status:%o value:%d\n",
					x, y, type_str[e->t], dir_str[e->d],
					owner_str[e->status & MINE ? MY : e->status & OPPT ? OPP : FREE], e->organ_id,
					e->organ_parent_id, e->organ_root_id, e->status, e->value);
		}

	fprintf(stderr, "Available (to me) sources ->");
	for (enum type t = A; t <= D; t++)
		fprintf(stderr, " %s: %d", type_str[t], available_sources[t]->size);
	putc('\n', stderr);
}

void print_restrictions(size_t number_vertices)
{
	fprintf(stderr, "# of vertices for the Floyd-Warshall algorithm -> %zu\n", number_vertices);
	fprintf(stderr, "Base weights -> %s: %d %s: %d\n", restrictions_str[NORMAL], w[NORMAL],
			restrictions_str[FORBIDDEN], w[FORBIDDEN]);
	fprintf(stderr, "Modifications to weights "
			"(cummulative for each endpoint of each path segment) ->\n  ");
	for (enum restrictions r = RESTRICTED_A; r <= RESTRICTED_D; r++)
		fprintf(stderr, "%s: %+d ", restrictions_str[r], w[r]);
	fprintf(stderr, "\n  ");
	for (enum restrictions r = REWARDED_A; r <= REWARDED_D; r++)
		fprintf(stderr, "%s: %+d ", restrictions_str[r], w[r]);
	putc('\n', stderr);
}

#define COMMON_DECLARATIONS                                                                             \
	int wdist = ((int (*)[NT * NT * sizeof(int)])body->distance_to_organism)[WEIGHTED][index];          \
	struct entity *closest =                                                                            \
			((struct entity *(*)[NT * sizeof(struct entity *)])body->closest_organ)[WEIGHTED][index];

void print_accessible(struct body *body, struct entity *e)
{
	int index = INDEX1(e);
	COMMON_DECLARATIONS

	fprintf(stderr, "  opp_organ %d (%d, %d) [val. %d] accessible from organ %d (%d, %d) at wdist %d\n",
			e->organ_id, e->x, e->y, e->value, closest->organ_id, closest->x, closest->y, wdist);
}

void print_source(struct body *body, struct entity *e)
{
	int index = INDEX1(e);
	COMMON_DECLARATIONS

	fprintf(stderr, "  source %s (%d, %d) accessible from organ %d (%d, %d) at wdistance %d\n",
			type_str[e->t], e->x, e->y, closest->organ_id, closest->x, closest->y, wdist);
}

void print_opponent(int my, struct body *body, int index)
{
	if (index != NO_INDEX) {
		COMMON_DECLARATIONS

		fprintf(stderr, "  closest to MY_%d: organ %d (%d, %d) %d away\n", my,
				closest->organ_id, closest->x, closest->y, wdist);
	}
	else
		fprintf(stderr, "  no direct path from MY_%d\n", my);
}

void print_organisms(hash_map_t *organisms, hash_map_t *opp_organisms)
{
	for (int i = 0; i < organisms->capacity; i++) {
		struct body *o = get_map(organisms, &i);

		fprintf(stderr, "MY_%d: root_id:%d organs:%d acc.tiles:%d vac.tiles:%d"
				" freeA:%d, freeB:%d, freeC:%d, freeD:%d\n", i, o->root->organ_id, o->body_parts->size,
				o->accessible_organs->size, size_set(o->vacant_slots), o->free_sources[A]->size,
				o->free_sources[B]->size, o->free_sources[C]->size, o->free_sources[D]->size);
		iterate_over_list(o, o->accessible_organs, (void (*)(void *, void *))print_accessible);
		for (list_t **l = o->free_sources; l - o->free_sources <= D; l++)
			iterate_over_list(o, *l, (void (*)(void *, void *))print_source);
	}

	for (int i = 0; i < opp_organisms->capacity; i++) {
		struct opp_body *o = get_map(opp_organisms, &i);

		fprintf(stderr, "OPP_%d: root_id:%d organs:%d\n",
				i, o->root->organ_id, o->body_parts->size);
		for (int j = 0; j < organisms->capacity; j++)
			print_opponent(j, get_map(organisms, &j), *(int *)get_map(o->closest_index, &j));
	}
}

void print_coords(void *data, struct entity *e)
{
	(void)data;
	fprintf(stderr, "(%d, %d) ", e->x, e->y);
}

void print_path(list_t *path)
{
	iterate_over_list(NULL, path, (void (*)(void *, void *))print_coords);
	putc('\n', stderr);
}

// main program  ----------------------------------------------------------------------------------------

int main()
{
	scanf("%d%d", &width, &height);
	NT = width * height;

	// memory allocation
	entity_t (*tiles)[height] = malloc(NT * sizeof(entity_t));
	int my_proteins[4];
	int opp_proteins[4];
	int harvested_sources[2][4];
	list_t *available_sources[4];
	for (enum type t = A; t <= D; t++)
		available_sources[t] = create_list(NULL);
	int n_organisms[2];
	// weighted and "taxicab" distances
	int (*distances)[NT * NT * sizeof(int)] = malloc(2 * NT * NT * sizeof(int));
	entity_t *(*previous)[NT * NT * sizeof(entity_t *)]
			= malloc(2 * NT * NT * sizeof(entity_t *));

	init_grid(tiles);

	// game loop
	for (int turn = 1; turn <= 100; turn++) {
		reset_grid(tiles);
		hash_map_t *organisms, *opp_organisms;
		memset(harvested_sources, 0, 8 * sizeof(int));
		memset(n_organisms, 0, 2 * sizeof(int));
 
		int entity_count;
		scanf("%d", &entity_count);
		hash_map_t *organs = create_hash_map(entity_count, sizeof(int), NULL, NULL, NULL, NULL);
		while (entity_count--) {
			int x, y;
			scanf("%d%d", &x, &y);
			entity_t *entity = &tiles[x][y];
			char type[33];
			scanf("%s", type);
			entity->t = (enum type)determine_enum(type_str, type);
			int owner;
			scanf("%d", &owner);
			entity->status |= owner == 1 ? MINE : owner == 0 ? OPPT : 0;
			scanf("%d", &entity->organ_id);
			if (entity->t == ROOT)
				n_organisms[e->status & MINE ? MY : OPP]++;
			if (entity->status & (MINE | OPPT))
				put_map(organs, &entity->organ_id, entity);
			char organ_dir[2];
			scanf("%s", organ_dir);
			entity->d = (enum dir)determine_enum(dir_str, organ_dir);
			scanf("%d%d",&entity->organ_parent_id, &entity->organ_root_id);
		}
		// your protein stock
		scanf("%d%d%d%d", my_proteins + A, my_proteins + B, my_proteins + C, my_proteins + D);
		// opponent's protein stock
		scanf("%d%d%d%d", opp_proteins + A, opp_proteins + B, opp_proteins + C, opp_proteins + D);
		// your number of organisms, output an action for each one in any order
		int required_actions_count;
		scanf("%d", &required_actions_count);
		if (required_actions_count != n_organisms[MY])
			fprintf(stderr, "ERROR: Root organs not recognized\n");

		// determine status for each tile
		determine_status(tiles);

		// calcultate entity values and active protein sources
		assess_tiles(tiles, organs, harvested_sources, available_sources);

#ifdef DEBUG_WORLD_BUILDING
		// quality control -- print-out of the remarkable entities
		print_entities(tiles, available_sources);
#endif

		// ponderate the pathing restriction levels
		weigh_restriction_levels(turn, harvested_sources);

		// establish self-governing organisms
		populate_organisms(n_organisms, &organisms, &opp_organisms, organs);

		// establish vertex-entity relationships of possible nodes in new routes
		struct array_v *vertices = generate_vertices(tiles);

#ifdef DEBUG_WORLD_BUILDING
		// quality control -- print-out the weights on restricted tiles + # of vertices for F-W
		print_restrictions(vertices->size);
#endif

		// shortest path calculator of potential new routes
		Floyd_Warshall(tiles, vertices, distances[WEIGHTED], previous[WEIGHTED], WEIGHTED);
		Floyd_Warshall(tiles, vertices, distances[TAXICAB], previous[TAXICAB], TAXICAB);

		// my organisms appraise their surroundings for direct interactions
		inspect_surroundings(tiles, distances, organisms, opp_organisms);

		// my organisms appraise their surroundings for ranged (spore) interactions
		aim_sporers(tiles, available_sources, distances, organisms, opp_organisms);

#ifdef DEBUG_WORLD_BUILDING
		// quality control -- print-out of all organisms
		print_organisms(organisms, opp_organisms);
#endif

/*
		// strategy selection

		struct entity *my_organ;
		enum type new_organ_type = BASIC;
		enum dir new_organ_dir = N;
		// primary objective: to establish a tentacle if possible on the highest-value target
		if (vulnerable_organs->size) {
			// select the highest-value enemy organ
			target = vulnerable_organs->node->content;
			fprintf(stderr, "Aiming for an enemy organ. Targeting %d (%d, %d)\n",
					target->organ_id, target->x, target->y);
			target = path->node->next->content;
			my_organ = target->wclosest_organ;
			new_organ_type = TENTACLE;
			new_organ_dir = face_to(target, vulnerable_organs->node->content);
			path = path_FW(tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
#ifdef DEBUG_WORLD_BUILDING
			print_path(path);
#endif
		}
		// secondary objective: to grow towards the highest-value enemy organ
		else if (accessible_organs->size) {
			target = accessible_organs->node->content;
			fprintf(stderr, "Growing towards an enemy organ. Targeting %d (%d, %d)\n",
					target->organ_id, target->x, target->y);
			my_organ = target->wclosest_organ;
			path = path_FW(tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
#ifdef DEBUG_WORLD_BUILDING
			print_path(path);
#endif
			target = path->node->next->content;
		}
		// fallback strategy: to grow wherever possible
		// choose any empty slot, favoring slots close to my_root
		else if (vacant_slots->size) {
			target = vacant_slots->node->content;
			my_organ = target->wclosest_organ;
			fprintf(stderr, "Strategy: Simply grow where possible. Empty slot to grow into (%d, %d)\n",
					target->x, target->y);
		}
		else {
			fprintf(stderr, "Nothing to do!\n");
			target = NULL;
		}
*/

		// tactical_commands
		order_organisms(organisms);

		// output commands
		for (int i = 0; i < required_actions_count; i++) {
			struct command *orders = ((struct body *)get_map(organisms, &i))->orders;

			// Write an action using printf(). DON'T FORGET THE TRAILING \n
			// To debug: fprintf(stderr, "Debug messages...\n");

			if (orders->target)
				printf("GROW %d %d %d %s %s",
						orders->from_organ->organ_id,
						orders->at_location->x,
						orders->at_location->y,
						type_str[orders->new_organ_type],
						dir_str[orders->new_organ_dir]);
			else
				printf("WAIT");
			if (turn == 1)
				printf(" glhf");
			else if (turn == 100)
				printf(" gg");
			putchar('\n');
		}

		// clear arrays, lists, and maps
		for (enum type t = A; t <= D; t++)
			clear_list(available_sources[t]);
		delete_map(&organs);
		delete_map(&organisms);
		delete_map(&opp_organisms);
		free(vertices);
	}

	free(tiles);
	for (enum type t = A; t <= D; t++)
		delete_list(available_sources + t);
	free(distances);
	free(previous);

	return 0;
}
