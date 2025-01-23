/**
 * TODO :
 * Add to the bodies new lists with line-of-sight from a SPORER or a vacant slot to each of the four
 * !MY_HARVESTED sources or enemy organisms. Spore must land two actual tiles away from the target. The
 * same organism could have a varios potential locations for the SPORER or where to land. Minimize
 * (first) the wdistance from new ROOT to target; maxmize (second) the length of the shot.
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

// linked list & hash map functions  --------------------------------------------------------------------

struct node {
    int key;
    void *content;
    struct node *next;
};

struct list {
    int size;
    struct node *node;
};

struct hash_map {
    int size;
    struct node **array;
};

struct list *create_list(void)
{
    struct list *list = malloc(sizeof(struct list));

    list->size = 0;
    list->node = NULL;

    return list;
}

void add_front_list(struct list *list, void *content)
{
    if (list) {
        struct node *new = malloc(sizeof(struct node));

        new->content = content;
        new->next = list->node;
        list->size++;
        list->node = new;
    }
}

void sort_list(void *data, struct list *list,
        int (*compare)(void *, void *, void *))
{
    if (list)
        for (struct node *n0 = list->node; n0 != NULL; n0 = n0->next)
            for (struct node *n1 = n0->next; n1 != NULL; n1 = n1->next)
                if (compare(data, n0->content, n1->content) > 0) {
                    void *temp = n0->content;
                    n0->content = n1->content;
                    n1->content = temp;
                }
}

void remove_from_list(struct list *list, void *content)
{
    if (!list || !content)
        return;

    struct node *next;
    struct node **pn = &list->node;

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

void iterate_over_list(void *data, struct list *list,
        void (*action)(void *, void *))
{
    struct node *next;

    if (list)
        for (struct node *n = list->node; n; n = next) {
            next = n->next;
            action(data, n->content);
        }
}

void clear_list(struct list *list, void (*del_content)(void *))
{
    struct node *next;

    if (list) {
        for (struct node **pn = &list->node; *pn; *pn = next) {
            next = (*pn)->next;
            if (del_content)
                del_content((*pn)->content);
            free(*pn);
        }
        list->size = 0;
    }
}

void delete_list(struct list **plist, void (*del_content)(void *))
{
    if (*plist) {
        clear_list(*plist, del_content);
        free(*plist);
        *plist = NULL;
    }
}

struct hash_map *create_hash_map(int size)
{
    struct hash_map *map = malloc(sizeof(struct hash_map));

    map->size = size;
    map->array = malloc(size * sizeof(struct node *));
    for (int i = 0; i < size; i++)
        map->array[i] = NULL;

    return map;
}

int hash(int key, int size)
{
    return key % size;
}

void put_map(struct hash_map *map, int key, void *content)
{
    if (map) {
        struct node *new = malloc(sizeof(struct node));
        int index = hash(key, map->size);

        new->key = key;
        new->content = content;
        new->next = map->array[index];
        map->array[index] = new;
    }
}

void *get_map(struct hash_map *map, int key)
{
    if (map)
        for (struct node *node = map->array[hash(key, map->size)];
                node; node = node->next)
            if (node->key == key)
                return node->content;

    return NULL;
}

void clear_map(struct hash_map *map, void (*del_content)(void *))
{
    if (map)
        for (struct node **pn = map->array; pn - map->array < map->size; pn++)
            while (*pn) {
                struct node *next = (*pn)->next;

                if (del_content)
                    del_content((*pn)->content);
                free(*pn);
                *pn = next;
            }
}

void delete_map(struct hash_map **pmap, void (*del_content)(void *))
{
    if (*pmap) {
        clear_map(*pmap, del_content);
        free(*pmap);
        *pmap = NULL;
    }
}

// tile-related auxiliary functions  --------------------------------------------------------------------

// status flags for the tiles
// if A, B, C, D, or EMPTY then 0, 1 otherwise
#define OCCUPIED      0001
// faced by a harvester of mine
#define MY_HARVESTED  0002
// faced by an opponentīs harvester
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
        struct hash_map *organs, int sources[3][4])
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
                else
                    sources[FREE][t]++;
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
    struct list *temp = create_list();

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
    for (struct node *node = temp->node; node; node = node->next)
        *p++ = node->content;
    delete_list(&temp, NULL);

    return array;
}

#define RESTRICTIONS    \
Y(NORMAL)               \
Y(RESTRICTED_A)         \
Y(RESTRICTED_B)         \
Y(RESTRICTED_C)         \
Y(RESTRICTED_D)         \
Y(FORBIDDEN)            \
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
#define BNORMAL     1
#define BRESTRICTED 2
#define BFORBIDDEN  1000
#define BREWARDED   -1

void weigh_restriction_levels(int turn, int sources[3][4])
{
    // preciousness of the resource
    int prec[2][4];

    for (enum type t = A; t <= D; t++) {
        prec[MY][t] = (sources[MY][t] <= 1 ? 2 : 1) * (turn >= 97 ? 0 : 1);
        prec[OPP][t] = (sources[OPP][t] <= 1 ? 2 : 1) * (turn == 100 ? 0 : 1);
    }

    w[NORMAL]       = BNORMAL;
    w[RESTRICTED_A] = BNORMAL + (BRESTRICTED - BNORMAL) * prec[MY][A];
    w[RESTRICTED_B] = BNORMAL + (BRESTRICTED - BNORMAL) * prec[MY][B];
    w[RESTRICTED_C] = BNORMAL + (BRESTRICTED - BNORMAL) * prec[MY][C];
    w[RESTRICTED_D] = BNORMAL + (BRESTRICTED - BNORMAL) * prec[MY][D];
    w[FORBIDDEN]    = BFORBIDDEN;
    w[REWARDED_A]   = BNORMAL + (BREWARDED - BNORMAL) * prec[OPP][A];
    w[REWARDED_B]   = BNORMAL + (BREWARDED - BNORMAL) * prec[OPP][B];
    w[REWARDED_C]   = BNORMAL + (BREWARDED - BNORMAL) * prec[OPP][C];
    w[REWARDED_D]   = BNORMAL + (BREWARDED - BNORMAL) * prec[OPP][D];
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
struct list *path_FW(struct entity tiles[width][height],
        struct entity **previous, int x1, int y1, int x2, int y2)
{
    if (!PREVIOUS(x1, y1, x2, y2))
        return NULL;

    struct list *path = create_list();

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
    struct list *body_parts;

    // shortest distance to the organism...
    int *distance_to_organism;
    // ...and closest organ for all tiles
    struct entity **closest_organ;

    // tiles of interest
    struct list *accessible_organs;
    struct list *vacant_slots;
    struct list *free_sources[4];

    // tactical commands
    struct command *orders;
};

struct opp_body {
    struct entity *root;
    struct list *body_parts;

    // shortest distance to my organs
    // (stores the simple index in the distance_to_organism and closest_organ matrices of body)
    struct hash_map *closest_index;
};

#define NO_INDEX -1

void populate_organisms(int n[2], struct hash_map **porganisms, struct hash_map **popp_organisms,
        struct hash_map *organs)
{
    *porganisms = create_hash_map(n[MY]);
    *popp_organisms = create_hash_map(n[OPP]);

    // my_organisms
    for (int i = 0; i < n[MY]; i++) {
        struct body *new = malloc(sizeof(struct body));

        new->body_parts = create_list();
        new->distance_to_organism = malloc(NT * sizeof(int));
        new->closest_organ = malloc(NT * sizeof(struct entity *));
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                new->distance_to_organism[EXP1(x, y)] = w[FORBIDDEN];
                new->closest_organ[EXP1(x, y)] = NULL;
            }
        new->accessible_organs = create_list();
        new->vacant_slots = create_list();
        for (struct list **l = new->free_sources; l - new->free_sources < 4; l++)
            *l = create_list();

        put_map(*porganisms, i, new);
    }

    // opp_organisms
    for (int i = 0; i < n[OPP]; i++) {
        struct opp_body *new = malloc(sizeof(struct opp_body));

        new->body_parts = create_list();
        new->closest_index = create_hash_map(n[MY]);
        for (int j = 0; j < n[MY]; j++) {
            int *index = malloc(sizeof(int));

            *index = NO_INDEX;

            put_map(new->closest_index, j, index);
        }

        put_map(*popp_organisms, i, new);
    }

    // establish the root organs
    int key_organism[2] = {0, 0};
    for (int i = 0; i < organs->size; i++) {
        struct entity *entity = get_map(organs, i);

        if (entity && entity->t == ROOT) {
            if (entity->o == MY)
                ((struct body *)get_map(*porganisms, key_organism[MY]++))->root = entity;
            else
                ((struct opp_body *)get_map(*popp_organisms, key_organism[OPP]++))->root = entity;
        }
    }

    // fill-in the body parts
    for (int i = 0; i < organs->size; i++) {
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

// tabulate my organism's disposition to engage
void inspect_surroundings(struct entity tiles[width][height], int *wdistances,
        struct hash_map *organisms, struct hash_map *opp_organisms)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            int index = EXP1(x, y);

            for (int i = 0; i < organisms->size; i++) {
                struct body *body = get_map(organisms, i);

                // calculate the closest points to my organisms
                for (struct node *n = body->body_parts->node; n; n = n->next) {
                    struct entity *ebody = n->content;
                    int d = DISTANCES(ebody->x, ebody->y, x, y);

                    if (d < body->distance_to_organism[index]) {
                        body->distance_to_organism[index] = d;
                        body->closest_organ[index] = ebody;
                    }
                }
                // fill-in lists of interesting tiles
                if (body->distance_to_organism[index] < w[FORBIDDEN]) {
                    if (tiles[x][y].o == OPP)
                        add_front_list(body->accessible_organs, &tiles[x][y]);
                    else if (tiles[x][y].status & ISPROTEIN && !(tiles[x][y].status & MY_HARVESTED))
                        add_front_list(body->free_sources[tiles[x][y].t], &tiles[x][y]);
                }
                if (!(tiles[x][y].status & OCCUPIED) && body->distance_to_organism[index] == 1)
                    add_front_list(body->vacant_slots, &tiles[x][y]);
            }
        }

    for (int i = 0; i < organisms->size; i++) {
        struct body *body = get_map(organisms, i);

        // figure the index of the closest distance to all opponents
        for (int j = 0; j < opp_organisms->size; j++) {
            struct opp_body *opp_body = get_map(opp_organisms, j);
            int *closest_index = get_map(opp_body->closest_index, i);

            iterate_over_list((int *[]){closest_index, body->distance_to_organism},
                    opp_body->body_parts, (void (*)(void *, void *))find_min_distance);
        }

        sort_list(NULL, body->accessible_organs,
                (int (*)(void *, void *, void *))compare_highest_value);
        sort_list(body->root, body->vacant_slots,
                (int (*)(void *, void *, void *))compare_closest_from_myroot);
        for (struct list **l = body->free_sources; l - body->free_sources < 4; l++)
            sort_list(body, *l, (int (*)(void *, void *, void *))compare_closest);
    }
}

// functions for the tactical overmind  -----------------------------------------------------------------

struct command {
    struct entity *target;
    struct entity *from_organ;
    struct entity *at_location;
    enum type new_organ_type;
    enum dir new_organ_dir;
    struct list *path;
};

void order_organisms(struct hash_map *organisms)
{
    for (int i = 0; i < organisms->size; i++) {
        struct command *orders = malloc(sizeof(struct command));

        ((struct body *)get_map(organisms, i))->orders = orders;
        orders->target = orders->from_organ = orders->at_location = NULL;
        orders->new_organ_type = BASIC;
        orders->new_organ_dir = N;
        orders->path = create_list();
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

// clean-up operations  ---------------------------------------------------------------------------------

void del_inner_body(struct body *body)
{
    delete_list(&body->body_parts, NULL);
    free(body->distance_to_organism);
    free(body->closest_organ);
    delete_list(&body->accessible_organs, NULL);
    delete_list(&body->vacant_slots, NULL);
    for (struct list **l = body->free_sources; l - body->free_sources < 4; l++)
            delete_list(l, NULL);
    delete_list(&body->orders->path, NULL);
    free(body->orders);
    free(body);
}

void del_opp_inner_body(struct opp_body *opp_body)
{
    delete_list(&opp_body->body_parts, NULL);
    delete_map(&opp_body->closest_index, free);
    free(opp_body);
}

// debugging assists  -----------------------------------------------------------------------------------

void print_entities(struct entity tiles[width][height])
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
}

void print_restrictions(void)
{
    for (enum restrictions r = NORMAL; r < FORBIDDEN; r++)
        fprintf(stderr, "%s: %d ", restrictions_str[r], w[r]);
    putc('\n', stderr);
    for (enum restrictions r = FORBIDDEN; r < sizeof(restrictions_str) / sizeof(*restrictions_str); r++)
        fprintf(stderr, "%s: %d ", restrictions_str[r], w[r]);
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
        fprintf(stderr, "  no line-of-sight with MY_%d\n", my);
}

void print_organisms(struct hash_map *organisms, struct hash_map *opp_organisms)
{
    for (int i = 0; i < organisms->size; i++) {
        struct body *o = get_map(organisms, i);

        fprintf(stderr, "MY_%d: root_id:%d organs:%d acc.tiles:%d vac.tiles:%d"
                " freeA:%d, freeB:%d, freeC:%d, freeD:%d\n", i, o->root->organ_id, o->body_parts->size,
                o->accessible_organs->size, o->vacant_slots->size, o->free_sources[A]->size,
                o->free_sources[B]->size, o->free_sources[C]->size, o->free_sources[D]->size);
        iterate_over_list(o, o->accessible_organs, (void (*)(void *, void *))print_accessible);
        for (struct list **l = o->free_sources; l - o->free_sources < 4; l++)
            iterate_over_list(o, *l, (void (*)(void *, void *))print_source);
    }

    for (int i = 0; i < opp_organisms->size; i++) {
        struct opp_body *o = get_map(opp_organisms, i);

        fprintf(stderr, "OPP_%d: root_id:%d organs:%d\n",
                i, o->root->organ_id, o->body_parts->size);
        for (int j = 0; j < organisms->size; j++)
            print_opponent(j, get_map(organisms, j), *(int *)get_map(o->closest_index, j));
    }
}

void print_coords(void *data, struct entity *e)
{
    (void)data;
    fprintf(stderr, "(%d, %d) ", e->x, e->y);
}

void print_path(struct list *path)
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
    int n_organisms[2];
    int *wdistances = malloc(NT * NT * sizeof(int)); // weighted distances of paths starting at my organs
    struct entity **previous = malloc(NT * NT * sizeof(struct entity *));

    init_grid(tiles);

    // game loop
    for (int turn = 1; turn <= 100; turn++) {
        reset_grid(tiles);
        struct hash_map *organisms, *opp_organisms;
        memset(sources, 0, 12 * sizeof(int));
        memset(n_organisms, 0, 2 * sizeof(int));
 
        int entity_count;
        scanf("%d", &entity_count);
        struct hash_map *organs = create_hash_map(entity_count);
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
        assess_tiles(tiles, organs, sources);

#ifdef DEBUG_WORLD_BUILDING
        // quality control -- print-out of the remarkable entities
        print_entities(tiles);
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

        // my organisms appraise their surroundings
        inspect_surroundings(tiles, wdistances, organisms, opp_organisms);

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
        delete_map(&organs, NULL);
        delete_map(&organisms, (void (*)(void *))del_inner_body);
        delete_map(&opp_organisms, (void (*)(void *))del_opp_inner_body);
        free(vertices);
    }

    free(tiles);
    free(wdistances);
    free(previous);

    return 0;
}