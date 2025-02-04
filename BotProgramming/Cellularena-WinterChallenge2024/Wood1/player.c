/**
 * TODO :
 * Add to the bodies new lists with line-of-sight from a SPORER or a vacant slot to each of the four
 * !MY_HARVESTED sources or enemy organisms. Spore must land two actual tiles away from the target. The
 * same organism could have a varios potential locations for the SPORER or where to land. Minimize
 * (first) the wdistance {from organ to new SPORER + from new ROOT to target}; maxmize (second) the
 * length of the shot.
 * 
 * Multi-agent AI : https://en.wikipedia.org/wiki/Multi-agent_system
 **/

#include <stdlib.h>
#include <stdio.h>
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

#define TYPE    \
X(A)            \
X(B)            \
X(C)            \
X(D)            \
X(EMPTY)        \
X(WALL)         \
X(ROOT)         \
X(BASIC)        \
X(TENTACLE)     \
X(HARVESTER)    \
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

#define OWNERSHIP   \
Y(MY)               \
Y(OPP)              \
Y(FREE)

#define Y(a) a,
enum ownership {
    OWNERSHIP
};
#undef Y

#define Y(a) #a,
char *owner_str[] = {
    OWNERSHIP
};
#undef Y

size_t determine_enum(char *options[], char *selection)
{
    for (char **o = options; ; o++)
        if (!strcmp(*o, selection))
            return o - options;
}

#define NO_INDEX -1

// linked list & hash map functions  --------------------------------------------------------------------

typedef struct node_s {
    int key;
    void *content;
    struct node_s *next;
} node_t;

typedef struct {
    int size;
    node_t *node;
    void (*del_content)(void *);
} list_t;

typedef struct associative_array_s {
    int capacity;
    node_t **array;
    void (*del_content)(void *);
    int (*hash)(struct associative_array_s *, int);
} hash_map_t, hash_set_t;

node_t *new_node(int key, void *content, node_t *next)
{
    node_t *new = malloc(sizeof(node_t));

    new->key = key;
    new->content = content;
    new->next = next;

    return new;
}

void delete_nodes(node_t **pn, void (*del_content)(void *))
{
    node_t *next;

    while (*pn) {
            next = (*pn)->next;
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
        list->node = new_node(0, content, list->node);
        list->size++;
    }
}

void iterate_over_list(void *data, list_t *list,
        void (*action)(void *, void *))
{
    if (list)
        for (node_t *n = list->node; n; n = n->next)
            action(data, n->content);
}

void sort_list(void *data, list_t *list,
        int (*compare)(void *, void *, void *))
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

void remove_from_list(list_t *list, void *content)
{
    if (!list || !content)
        return;

    node_t *next;
    node_t **pn = &list->node;

    while (*pn) {
        next = (*pn)->next;
        if ((*pn)->content == content) {
            list->size--;
            free(*pn);
            *pn = next;
        }
        else
            pn = &next;
    }
}

// if predicate == NULL, returns index of the first occurrence of data in list
int find_first_in_list(void *data, list_t *list, int (*predicate)(void *, void *))
{
    if (list)
        for (node_t *n = list->node; n; n = n->next)
            if (predicate && predicate(data, n->content) || !predicate && n->content == data)
                return n - list->node;

    return NO_INDEX;
}

void clear_list(list_t *list)
{
    if (list) {
        delete_nodes(&list->node, list->del_content);
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

hash_map_t *create_hash_map(int capacity, void (*del_content)(void *),
        int (*hash)(hash_map_t *, int))
{
    hash_map_t *map = malloc(sizeof(hash_map_t));

    map->capacity = capacity;
    map->array = malloc(capacity * sizeof(node_t *));
    for (int i = 0; i < capacity; i++)
        map->array[i] = NULL;
    map->del_content = del_content;
    map->hash = hash;

    return map;
}

int default_hash(hash_map_t *map, int key)
{
    return key % map->capacity;
}

node_t *get_node_map(hash_map_t *map, int key)
{
    if (map || key < 0)
        for (node_t *node = map->array[map->hash(map, key)];
                node; node = node->next)
            if (node->key == key)
                return node;

    return NULL;
}

void *get_map(hash_map_t *map, int key)
{
    node_t *node = get_node_map(map, key);

    if (node)
        return node->content;

    return NULL;
}

// replaces old value if the key mapped some previous content
void put_map(hash_map_t *map, int key, void *content)
{
    if (map) {
        node_t *old = get_node_map(map, key);

        if (old) {
            if (map->del_content)
                map->del_content(old->content);
            old->content = content;
        }
        else {
            int index = map->hash(map, key);

            map->array[index] = new_node(key, content, map->array[index]);
        }
    }
}

int size_map(hash_map_t *map)
{
    int size = -1;

    if (map) {
        size = 0;
        for (node_t **pn1 = map->array; pn1 - map->array < map->capacity; pn1++)
            for (node_t *pn2 = *pn1; pn2; pn2 = pn2->next)
                size++;
    }

    return size;
}

void clear_map(hash_map_t *map)
{
    if (map)
        for (node_t **pn = map->array; pn - map->array < map->capacity; pn++)
            delete_nodes(pn, map->del_content);
}

void delete_map(hash_map_t **pmap)
{
    if (*pmap) {
        clear_map(*pmap);
        free(*pmap);
        *pmap = NULL;
    }
}

hash_set_t *create_hash_set(int capacity, void (*del_content)(void *),
        int (*hash)(hash_set_t *, int))
{
    return create_hash_map(capacity, del_content, hash);
}

int add_set(hash_set_t *set, void *content)
{
    return 0;
}

// tile-related auxiliary functions  --------------------------------------------------------------------

// status flags for the tiles
// if A, B, C, D, or EMPTY then 0, 1 otherwise
#define OCCUPIED      0001
// faced by a harvester of mine
#define MY_HARVESTED  0002
// faced by an opponent´s harvester
#define OPP_HARVESTED 0004
// protected by my tentacles
#define PROTECTED     0010
// menaced by opponent tentacles
#define MENACED       0020
// is type BASIC, HARVESTER, TENTACLE, SPORER, or ROOT
#define ISORGAN       0040
// is a protein source
#define ISPROTEIN     0100

struct vertex {
    // orientation of the vertex
    enum dir d;
    // tile where this vertex lies
    struct entity *e;
    // adjacent vertices
    struct vertex *a[4];
};

// record kept of all tiles
struct entity {
    int x;
    // grid coordinate
    int y;
    // A, B, C, D, EMPTY, WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER
    enum type t;
    // MY if your organ, OPP if enemy organ, FREE if neither
    enum ownership o;
    // id of this entity if it's an organ, 0 otherwise
    int organ_id;
    // N, E, S, W or X if not an organ
    enum dir d;
    int organ_parent_id;
    int organ_root_id;
    int status;
    int value;
    // N, E, S, W for lateral ending vertices in OCCUPIED tiles only;
    // X for a central connecting vertex on an UNOCCUPIED tile
    struct vertex v[5];
};

// columns in the game grid
int width;
// rows in the game grid
int height;
// NT = width * height
int NT;

#define EXPAND(a, b, c) ((a) * (c) + (b))
#define EXP1(a, b) EXPAND(a, b, height)
#define EXP2(x1, y1, x2, y2) EXPAND(EXP1(x1, y1), EXP1(x2, y2), NT)
#define INDEX(a) EXP1((a)->x, (a)->y)

void init_grid(struct entity tiles[width][height])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            struct entity *e = &tiles[x][y];

            e->x = x;
            e->y = y;
            e->v[N].d = N;
            e->v[E].d = E;
            e->v[S].d = S;
            e->v[W].d = W;
            e->v[X].d = X;
            e->v[N].e = e;
            e->v[E].e = e;
            e->v[S].e = e;
            e->v[W].e = e;
            e->v[X].e = e;
        }
}

void reset_grid(struct entity tiles[width][height])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            struct entity *e = &tiles[x][y];

            e->t = EMPTY;
            e->o = FREE;
            e->organ_id = 0;
            e->d = X;
            e->organ_parent_id = 0;
            e->organ_root_id = 0;
            e->status = 0;
            e->value = 0;
        }
}

void determine_status(struct entity tiles[width][height])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            int t = tiles[x][y].t;

            if (y < height - 1 && tiles[x][y + 1].d == N) {
                if (tiles[x][y + 1].t == HARVESTER)
                    tiles[x][y].status |= tiles[x][y + 1].o == MY ? MY_HARVESTED : OPP_HARVESTED;
                else if (tiles[x][y + 1].t == TENTACLE)
                    tiles[x][y].status |= tiles[x][y + 1].o == MY ? PROTECTED : MENACED;
            }
            if (x > 0 && tiles[x - 1][y].d == E) {
                if (tiles[x - 1][y].t == HARVESTER)
                    tiles[x][y].status |= tiles[x - 1][y].o == MY ? MY_HARVESTED : OPP_HARVESTED;
                else if (tiles[x - 1][y].t == TENTACLE)
                    tiles[x][y].status |= tiles[x - 1][y].o == MY ? PROTECTED : MENACED;
            }
            if (y > 0 && tiles[x][y - 1].d == S) {
                if (tiles[x][y - 1].t == HARVESTER)
                    tiles[x][y].status |= tiles[x][y - 1].o == MY ? MY_HARVESTED : OPP_HARVESTED;
                else if (tiles[x][y - 1].t == TENTACLE)
                    tiles[x][y].status |= tiles[x][y - 1].o == MY ? PROTECTED : MENACED;
            }
            if (x < width - 1 && tiles[x + 1][y].d == W) {
                if (tiles[x + 1][y].t == HARVESTER)
                    tiles[x][y].status |= tiles[x + 1][y].o == MY ? MY_HARVESTED : OPP_HARVESTED;
                else if (tiles[x + 1][y].t == TENTACLE)
                    tiles[x][y].status |= tiles[x + 1][y].o == MY ? PROTECTED : MENACED;
            }
            if (t == A || t == B || t == C || t == D)
                tiles[x][y].status |= ISPROTEIN;
            else if (t == BASIC || t == HARVESTER || t == TENTACLE || t == SPORER || t == ROOT)
                tiles[x][y].status |= ISORGAN | OCCUPIED;
            else if (t == WALL)
                tiles[x][y].status |= OCCUPIED;
        }
}

void assess_tiles(struct entity tiles[width][height],
        hash_map_t *organs, int sources[3][4], list_t *free_sources[4])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            int t = tiles[x][y].t;
            // single-tile value = # of resorces required to build
            int value = t == BASIC ? 1 :
                    t == HARVESTER || t == TENTACLE || t == SPORER ? 2 :
                    t == ROOT ? 3 : 0;

            // determine protein sources
            // (FREE here stands for "available to me")
            if (tiles[x][y].status & ISPROTEIN) {
                if (tiles[x][y].status & MY_HARVESTED)
                    sources[MY][t]++;
                else {
                    sources[FREE][t]++;
                    add_front_list(free_sources[t], &tiles[x][y]);
                }
                if (tiles[x][y].status & OPP_HARVESTED)
                    sources[OPP][t]++;
            }
            // determine organ value
            // value = its own + that of all its descendants
            else if (value) {
                struct entity *entity = get_map(organs, tiles[x][y].organ_id);

                while (entity->organ_parent_id) {
                    entity->value += value;
                    entity = get_map(organs, entity->organ_parent_id);
                };
                entity->value += value;
            }
        }
}

struct array_v {
    int size;
    struct vertex *array[];
};

// note that these are not all the vertices of the grid: only those that might be
// part of a new, clear and unbroken path set from one of my organs to some target
struct array_v *generate_vertices(struct entity tiles[width][height])
{
    list_t *temp = create_list(NULL);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            struct entity *e = &tiles[x][y];
            struct vertex *v;

            if (e->t == WALL)
                continue;
            if (e->status & OCCUPIED) {
                if (y > 0 && !(tiles[x][y - 1].status & OCCUPIED)) {
                    v = &e->v[N];
                    v->a[0] = &tiles[x][y - 1].v[X];
                    v->a[1] = v->a[2] = v->a[3] = NULL;
                    add_front_list(temp, v);
                }
                if (x < width - 1 && !(tiles[x + 1][y].status & OCCUPIED)) {
                    v = &e->v[E];
                    v->a[0] = &tiles[x + 1][y].v[X];
                    v->a[1] = v->a[2] = v->a[3] = NULL;
                    add_front_list(temp, v);
                }
                if (y < height - 1 && !(tiles[x][y + 1].status & OCCUPIED)) {
                    v = &e->v[S];
                    v->a[0] = &tiles[x][y + 1].v[X];
                    v->a[1] = v->a[2] = v->a[3] = NULL;
                    add_front_list(temp, v);
                }
                if (x > 0 && !(tiles[x - 1][y].status & OCCUPIED)) {
                    v = &e->v[W];
                    v->a[0] = &tiles[x - 1][y].v[X];
                    v->a[1] = v->a[2] = v->a[3] = NULL;
                    add_front_list(temp, v);
                }
            }
            else {
                v = &e->v[X];
                v->a[0] = y == 0 ? NULL :
                        tiles[x][y - 1].status & OCCUPIED ? &tiles[x][y - 1].v[S] :
                        &tiles[x][y - 1].v[X];
                v->a[1] = x == width - 1 ? NULL :
                        tiles[x + 1][y].status & OCCUPIED ? &tiles[x + 1][y].v[W] :
                        &tiles[x + 1][y].v[X];
                v->a[2] = y == height - 1 ? NULL :
                        tiles[x][y + 1].status & OCCUPIED ? &tiles[x][y + 1].v[N] :
                        &tiles[x][y + 1].v[X];
                v->a[3] = x == 0 ? NULL :
                        tiles[x - 1][y].status & OCCUPIED ? &tiles[x - 1][y].v[E] :
                        &tiles[x - 1][y].v[X];
                add_front_list(temp, v);
            }
        }

    struct array_v *array = malloc(sizeof(struct array_v)
            + temp->size * sizeof(struct vertex *));

    array->size = temp->size;
    struct vertex **p = array->array;
    for (node_t *node = temp->node; node; node = node->next)
        *p++ = node->content;
    delete_list(&temp);

    return array;
}

#define RESTRICTIONS    \
Y(NORMAL)               \
Y(FORBIDDEN)            \
Y(RESTRICTED_A)         \
Y(RESTRICTED_B)         \
Y(RESTRICTED_C)         \
Y(RESTRICTED_D)         \
Y(REWARDED_A)           \
Y(REWARDED_B)           \
Y(REWARDED_C)           \
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

void weigh_restriction_levels(int turn, int sources[3][4])
{
    // preciousness of the resource
    int prec[2][4];

    for (enum type t = A; t <= D; t++) {
        prec[MY][t] = (sources[MY][t] <= 1 ? 2 : 1) * (turn >= 97 ? 0 : 1);
        prec[OPP][t] = (sources[OPP][t] <= 1 ? 2 : 1) * (turn == 100 ? 0 : 1);
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

#define DISTANCES(x1, y1, x2, y2) wdistances[EXP2(x1, y1, x2, y2)]
#define PREVIOUS(x1, y1, x2, y2) previous[EXP2(x1, y1, x2, y2)]

int adjacent_vertices(struct vertex *v1, struct vertex *v2)
{
    for (struct vertex **pa = v1->a; pa - v1->a < 4; pa++)
        if (*pa && *pa == v2)
            return 1;

    return 0;
}

// [https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm]
void Floyd_Warshall(struct entity tiles[width][height], struct array_v *vertices,
        int *wdistances, struct entity **previous)
{
    const int NV = vertices->size;
    int (*wdistances_v)[NV] = malloc(NV * NV * sizeof(int));
    struct vertex *(*previous_v)[NV] = malloc(NV * NV * sizeof(struct vertex *));

    // initialization of Floyd-Warshall matrices & restrict areas ahead of a tentacle
    // (matrix indices range over all vertices)
    for (int i = 0; i < NV; i++) {
        struct vertex *vi = vertices->array[i];
        struct entity *ei = vi->e;

        for (int j = 0; j < NV; j++) {
            struct vertex *vj = vertices->array[j];
            struct entity *ej = vj->e;

            // for each edge
            if (adjacent_vertices(vi, vj)) {
                if (ei->status & MENACED || ej->status & MENACED)
                    wdistances_v[i][j] = w[FORBIDDEN];
                else {
                    wdistances_v[i][j] = w[NORMAL];
                    if (ei->status & MY_HARVESTED)
                        wdistances_v[i][j] +=
                                ei->t == A ? w[RESTRICTED_A] :
                                ei->t == B ? w[RESTRICTED_B] :
                                ei->t == C ? w[RESTRICTED_C] :
                                w[RESTRICTED_D];
                    if (ej->status & MY_HARVESTED)
                        wdistances_v[i][j] +=
                                ej->t == A ? w[RESTRICTED_A] :
                                ej->t == B ? w[RESTRICTED_B] :
                                ej->t == C ? w[RESTRICTED_C] :
                                w[RESTRICTED_D];
                    if (ei->status & OPP_HARVESTED)
                        wdistances_v[i][j] +=
                                ei->t == A ? w[REWARDED_A] :
                                ei->t == B ? w[REWARDED_B] :
                                ei->t == C ? w[REWARDED_C] :
                                w[REWARDED_D];
                    if (ej->status & OPP_HARVESTED)
                        wdistances_v[i][j] +=
                                ej->t == A ? w[REWARDED_A] :
                                ej->t == B ? w[REWARDED_B] :
                                ej->t == C ? w[REWARDED_C] :
                                w[REWARDED_D];
                }
                previous_v[i][j] = vi;
            }
            // for each vertex
            else {
                wdistances_v[i][j] = i == j ? 0 : w[FORBIDDEN];
                previous_v[i][j] = i == j ? vi : NULL;
            }
        }
    }

    // Floyd_Warshall algorithm
    // (matrix indices range over all vertices)
    for (int k = 0; k < NV; k++)
        for (int i = 0; i < NV; i++)
            for (int j = 0; j < NV; j++)
                if (wdistances_v[i][j] > wdistances_v[i][k] + wdistances_v[k][j]) {
                    wdistances_v[i][j] = wdistances_v[i][k] + wdistances_v[k][j];
                    previous_v[i][j] = previous_v[k][j];
                }

    // determines best result for all vertices within each tile
    // (matrix indices range over all tiles)
    for (int i = 0; i < NT * NT; i++) {
        wdistances[i] = w[FORBIDDEN];
        previous[i] = NULL;
    }
    for (int i = 0; i < NV; i++) {
        int x1 = vertices->array[i]->e->x;
        int y1 = vertices->array[i]->e->y;
        for (int j = 0; j < NV; j++) {
            int x2 = vertices->array[j]->e->x;
            int y2 = vertices->array[j]->e->y;
            if (wdistances_v[i][j] < DISTANCES(x1, y1, x2, y2)) {
                DISTANCES(x1, y1, x2, y2) = wdistances_v[i][j];
                PREVIOUS(x1, y1, x2, y2) = previous_v[i][j]->e;
            }
        }
    }

    free(wdistances_v);
    free(previous_v);
}

// shortest path reconstruction from the Floyd-Warshall alorithm
list_t *path_FW(struct entity tiles[width][height],
        struct entity **previous, int x1, int y1, int x2, int y2)
{
    if (!PREVIOUS(x1, y1, x2, y2))
        return NULL;

    list_t *path = create_list(NULL);

    add_front_list(path, &tiles[x2][y2]);
    while (x1 != x2 || y1 != y2) {
        struct entity *e = PREVIOUS(x1, y1, x2, y2);

        x2 = e->x;
        y2 = e->y;
        add_front_list(path, &tiles[x2][y2]);
    }

    return path;
}

// body-related auxiliary functions  --------------------------------------------------------------------

struct body {
    struct entity *root;
    list_t *body_parts;

    // shortest distance to the organism...
    int *distance_to_organism;
    // ...and closest organ for all tiles
    struct entity **closest_organ;

    // tiles of interest
    list_t *accessible_organs;
    list_t *vacant_slots;
    list_t *free_sources[4];

    // tactical commands
    struct command *orders;
};

struct opp_body {
    struct entity *root;
    list_t *body_parts;

    // shortest distance to my organs
    // (stores the simple index in the distance_to_organism and closest_organ matrices of body)
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

void del_inner_body(struct body *body)
{
    delete_list(&body->body_parts);
    free(body->distance_to_organism);
    free(body->closest_organ);
    delete_list(&body->accessible_organs);
    delete_list(&body->vacant_slots);
    for (list_t **l = body->free_sources; l - body->free_sources <= D; l++)
            delete_list(l);
    delete_list(&body->orders->path);
    free(body->orders);
    free(body);
}

void del_opp_inner_body(struct opp_body *opp_body)
{
    delete_list(&opp_body->body_parts);
    delete_map(&opp_body->closest_index);
    free(opp_body);
}

void populate_organisms(int n[2], hash_map_t **porganisms, hash_map_t **popp_organisms,
        hash_map_t *organs)
{
    *porganisms = create_hash_map(n[MY], (void (*)(void *))del_inner_body, default_hash);
    *popp_organisms = create_hash_map(n[OPP], (void (*)(void *))del_opp_inner_body, default_hash);

    // my_organisms
    for (int i = 0; i < n[MY]; i++) {
        struct body *new = malloc(sizeof(struct body));

        new->body_parts = create_list(NULL);
        new->distance_to_organism = malloc(NT * sizeof(int));
        new->closest_organ = malloc(NT * sizeof(struct entity *));
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                new->distance_to_organism[EXP1(x, y)] = w[FORBIDDEN];
                new->closest_organ[EXP1(x, y)] = NULL;
            }
        new->accessible_organs = create_list(NULL);
        new->vacant_slots = create_list(NULL);
        for (list_t **l = new->free_sources; l - new->free_sources <= D; l++)
            *l = create_list(NULL);

        put_map(*porganisms, i, new);
    }

    // opp_organisms
    for (int i = 0; i < n[OPP]; i++) {
        struct opp_body *new = malloc(sizeof(struct opp_body));

        new->body_parts = create_list(NULL);
        new->closest_index = create_hash_map(n[MY], free, default_hash);
        for (int j = 0; j < n[MY]; j++) {
            int *index = malloc(sizeof(int));

            *index = NO_INDEX;

            put_map(new->closest_index, j, index);
        }

        put_map(*popp_organisms, i, new);
    }

    // establish the root organs
    int key_organism[2] = {0, 0};
    for (int i = 0; i < organs->capacity; i++) {
        struct entity *entity = get_map(organs, i);

        if (entity && entity->t == ROOT) {
            if (entity->o == MY)
                ((struct body *)get_map(*porganisms, key_organism[MY]++))->root = entity;
            else
                ((struct opp_body *)get_map(*popp_organisms, key_organism[OPP]++))->root = entity;
        }
    }

    // fill-in the body parts
    for (int i = 0; i < organs->capacity; i++) {
        struct entity *entity = get_map(organs, i);

        if (entity) {
            if (entity->o == MY)
                for (int j = 0; ; j++) {
                    struct body *body = get_map(*porganisms, j);

                    if (body->root->organ_id == entity->organ_root_id) {
                        add_front_list(body->body_parts, entity);
                        break;
                    }
                }
            else
                for (int j = 0; ; j++) {
                    struct opp_body *opp_body = get_map(*popp_organisms, j);

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
    return body->distance_to_organism[INDEX(e0)]
            - body->distance_to_organism[INDEX(e1)];
}

struct container1 {
    int *wdistances;
    struct body *body;
    int x, y;
    int index;
};

void closest_points(struct container1 *data, struct entity *e)
{
    int d = data->DISTANCES(e->x, e->y, data->x, data->y);

    if (d < data->body->distance_to_organism[data->index]) {
        data->body->distance_to_organism[data->index] = d;
        data->body->closest_organ[data->index] = e;
    }
}

void find_min_distance(int *data[2], struct entity *e)
{
    int *closest_index = data[0];
    int *distances = data[1];
    int index = INDEX(e);
    int min_d = *closest_index == NO_INDEX ? w[FORBIDDEN] : distances[*closest_index];
    int d = distances[index];

    if (d < min_d)
        *closest_index = index;
}

// tabulate my organism's disposition to engage directly
void inspect_surroundings(struct entity tiles[width][height], int *wdistances,
        hash_map_t *organisms, hash_map_t *opp_organisms)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            int index = EXP1(x, y);

            for (int i = 0; i < organisms->capacity; i++) {
                struct body *body = get_map(organisms, i);

                // calculate the closest points to my organisms
                iterate_over_list(&(struct container1){wdistances, body, x, y, index},
                        body->body_parts, (void (*)(void *, void *))closest_points);

                // fill-in lists of interesting tiles
                if (body->distance_to_organism[index] < w[FORBIDDEN]) {
                    if (tiles[x][y].o == OPP)
                        add_front_list(body->accessible_organs, &tiles[x][y]);
                    else if (tiles[x][y].status & ISPROTEIN && !(tiles[x][y].status & MY_HARVESTED))
                        add_front_list(body->free_sources[tiles[x][y].t], &tiles[x][y]);
                }
                if (!(tiles[x][y].status & OCCUPIED) && body->distance_to_organism[index] == w[NORMAL])
                    add_front_list(body->vacant_slots, &tiles[x][y]);
            }
        }

    for (int i = 0; i < organisms->capacity; i++) {
        struct body *body = get_map(organisms, i);

        // figure the index of the closest distance to all opponents
        for (int j = 0; j < opp_organisms->capacity; j++) {
            struct opp_body *opp_body = get_map(opp_organisms, j);
            int *closest_index = get_map(opp_body->closest_index, i);

            iterate_over_list((int *[]){closest_index, body->distance_to_organism},
                    opp_body->body_parts, (void (*)(void *, void *))find_min_distance);
        }

        sort_list(NULL, body->accessible_organs,
                (int (*)(void *, void *, void *))compare_highest_value);
        sort_list(body->root, body->vacant_slots,
                (int (*)(void *, void *, void *))compare_closest_from_myroot);
        for (list_t **l = body->free_sources; l - body->free_sources <= D; l++)
            sort_list(body, *l, (int (*)(void *, void *, void *))compare_closest);
    }
}

struct body *whose_body(hash_map_t *organisms, struct entity *e)
{
    for (int i = 0; i < organisms->capacity; i++) {
        struct body *body = get_map(organisms, i);

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
    int *wdistances;
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

void aim_sporers(struct entity tiles[width][height], list_t *free_sources[4], int *wdistances,
        hash_map_t *organisms, hash_map_t *opp_organisms)
{
    // aim on FREE (== !MY_HARVESTED) sources
    for (enum type t = A; t <= D; t++)
        iterate_over_list(&(struct container2){tiles, wdistances, organisms},
                free_sources[t], (void (*)(void *, void *))aim_on_source);
}

// functions for the tactical overmind  -----------------------------------------------------------------

void order_organisms(hash_map_t *organisms)
{
    for (int i = 0; i < organisms->capacity; i++) {
        struct command *orders = malloc(sizeof(struct command));

        ((struct body *)get_map(organisms, i))->orders = orders;
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

void print_entities(struct entity tiles[width][height], list_t *free_sources[4])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            struct entity *e = &tiles[x][y];

            if (e->t == WALL || e->t == EMPTY)
                continue;
            fprintf(stderr, "(%d, %d) %s %s owner:%s id:%d parent_id:%d root_id:%d status:%d value:%d\n",
                    x, y, type_str[e->t], dir_str[e->d], owner_str[e->o], e->organ_id,
                    e->organ_parent_id, e->organ_root_id, e->status, e->value);
        }

    fprintf(stderr, "FREE sources -");
    for (enum type t = A; t <= D; t++)
        fprintf(stderr, " %s: %d", type_str[t], free_sources[t]->size);
    putc('\n', stderr);
}

void print_restrictions(void)
{
    fprintf(stderr, "Base weights - %s: %d %s: %d\n", restrictions_str[NORMAL], w[NORMAL],
            restrictions_str[FORBIDDEN], w[FORBIDDEN]);
    fprintf(stderr, "Modifications to weights (cummulative for each endpoint of each path segment)-\n  ");
    for (enum restrictions r = RESTRICTED_A; r <= RESTRICTED_D; r++)
        fprintf(stderr, "%s: %+d ", restrictions_str[r], w[r]);
    fprintf(stderr, "\n  ");
    for (enum restrictions r = REWARDED_A; r <= REWARDED_D; r++)
        fprintf(stderr, "%s: %+d ", restrictions_str[r], w[r]);
    putc('\n', stderr);
}

void print_accessible(struct body *body, struct entity *e)
{
    int index = INDEX(e);
    struct entity *co = body->closest_organ[index];

    fprintf(stderr, "  opp_organ %d (%d, %d) [val. %d] accessible from organ %d (%d, %d) at wdist %d\n",
            e->organ_id, e->x, e->y, e->value, co->organ_id, co->x, co->y,
            body->distance_to_organism[index]);
}

void print_source(struct body *body, struct entity *e)
{
    int index = INDEX(e);
    struct entity *co = body->closest_organ[index];

    fprintf(stderr, "  source %s (%d, %d) accessible from organ %d (%d, %d) at wdistance %d\n",
            type_str[e->t], e->x, e->y, co->organ_id, co->x, co->y, body->distance_to_organism[index]);
}

void print_opponent(int my, struct body *body, int index)
{
    if (index != NO_INDEX) {
        struct entity *co = body->closest_organ[index];

        fprintf(stderr, "  closest to MY_%d: organ %d (%d, %d) %d away\n", my,
                co->organ_id, co->x, co->y, body->distance_to_organism[index]);
    }
    else
        fprintf(stderr, "  no direct path from MY_%d\n", my);
}

void print_organisms(hash_map_t *organisms, hash_map_t *opp_organisms)
{
    for (int i = 0; i < organisms->capacity; i++) {
        struct body *o = get_map(organisms, i);

        fprintf(stderr, "MY_%d: root_id:%d organs:%d acc.tiles:%d vac.tiles:%d"
                " freeA:%d, freeB:%d, freeC:%d, freeD:%d\n", i, o->root->organ_id, o->body_parts->size,
                o->accessible_organs->size, o->vacant_slots->size, o->free_sources[A]->size,
                o->free_sources[B]->size, o->free_sources[C]->size, o->free_sources[D]->size);
        iterate_over_list(o, o->accessible_organs, (void (*)(void *, void *))print_accessible);
        for (list_t **l = o->free_sources; l - o->free_sources <= D; l++)
            iterate_over_list(o, *l, (void (*)(void *, void *))print_source);
    }

    for (int i = 0; i < opp_organisms->capacity; i++) {
        struct opp_body *o = get_map(opp_organisms, i);

        fprintf(stderr, "OPP_%d: root_id:%d organs:%d\n",
                i, o->root->organ_id, o->body_parts->size);
        for (int j = 0; j < organisms->capacity; j++)
            print_opponent(j, get_map(organisms, j), *(int *)get_map(o->closest_index, j));
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
    struct entity (*tiles)[height] = malloc(NT * sizeof(struct entity));
    int my_proteins[4];
    int opp_proteins[4];
    int sources[3][4];
    list_t *free_sources[4];
    for (enum type t = A; t <= D; t++)
        free_sources[t] = create_list(NULL);
    int n_organisms[2];
    int *wdistances = malloc(NT * NT * sizeof(int)); // weighted distances
    struct entity **previous = malloc(NT * NT * sizeof(struct entity *));

    init_grid(tiles);

    // game loop
    for (int turn = 1; turn <= 100; turn++) {
        reset_grid(tiles);
        hash_map_t *organisms, *opp_organisms;
        memset(sources, 0, 12 * sizeof(int));
        memset(n_organisms, 0, 2 * sizeof(int));
 
        int entity_count;
        scanf("%d", &entity_count);
        hash_map_t *organs = create_hash_map(entity_count, NULL, default_hash);
        for (int i = 0; i < entity_count; i++) {
            int x, y;
            scanf("%d%d", &x, &y);
            struct entity *entity = &tiles[x][y];
            char type[33];
            scanf("%s", type);
            entity->t = (enum type)determine_enum(type_str, type);
            int owner;
            scanf("%d", &owner);
            entity->o = owner == 1 ? MY : owner == 0 ? OPP : FREE;
            scanf("%d", &entity->organ_id);
            if (entity->t == ROOT)
                n_organisms[entity->o]++;
            if (entity->o != FREE)
                put_map(organs, entity->organ_id, entity);
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
        assess_tiles(tiles, organs, sources, free_sources);

#ifdef DEBUG_WORLD_BUILDING
        // quality control -- print-out of the remarkable entities
        print_entities(tiles, free_sources);
#endif

        // ponderate the pathing restriction levels
        weigh_restriction_levels(turn, sources);

        // establish self-governing organisms
        populate_organisms(n_organisms, &organisms, &opp_organisms, organs);

        // establish vertex-entity relationships of possible nodes in new routes
        struct array_v *vertices = generate_vertices(tiles);

#ifdef DEBUG_WORLD_BUILDING
        // quality control -- print-out the weights on restricted tiles
        print_restrictions();
#endif

        // shortest path calculator of potential new routes
        Floyd_Warshall(tiles, vertices, wdistances, previous);

        // my organisms appraise their surroundings for direct interactions
        inspect_surroundings(tiles, wdistances, organisms, opp_organisms);

        // my organisms appraise their surroundings for ranged (spore) interactions
        aim_sporers(tiles, free_sources, wdistances, organisms, opp_organisms);

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
            my_organ = target->closest_organ;
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
            my_organ = target->closest_organ;
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
            my_organ = target->closest_organ;
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
            struct command *orders = ((struct body *)get_map(organisms, i))->orders;

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
            clear_list(free_sources[t]);
        delete_map(&organs);
        delete_map(&organisms);
        delete_map(&opp_organisms);
        free(vertices);
    }

    free(tiles);
    for (enum type t = A; t <= D; t++)
        delete_list(free_sources + t);
    free(wdistances);
    free(previous);

    return 0;
}