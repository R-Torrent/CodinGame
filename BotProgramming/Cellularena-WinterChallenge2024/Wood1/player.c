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

// comment-out the DEBUG macro when the program is ready
#define DEBUG

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

// linked list & hash map functions --------------------------------------------------------------

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

void reorder_list(void *data, struct list *list,
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

    struct node **pn = &list->node, *next;
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

void iterate_over_list(struct list *list, void (*action)(void *))
{
    struct node *next;

    if (list)
        for (struct node *n = list->node; n; n = next) {
            next = n->next;
            action(n->content);
        }
}

void clear_list(struct list *list, void (*del_content)(void *))
{
    struct node *next;

    if (list)
        for (struct node *n = list->node; n; n = next) {
            next = n->next;
            if (del_content)
                del_content(n->content);
            free(n);
            n = next;
        }
    free(list);
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
            };
    free(map);
}

// tile-related auxiliary functions --------------------------------------------------------------

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

// record of kept of all tiles
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
            else if (t = WALL)
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
            int value = t == BASIC ? 1 : t == HARVESTER || t == TENTACLE
                    || t == SPORER ? 2 : t == ROOT ? 3 : 0;

            // determine protein sources
            if (tiles[x][y].status & ISPROTEIN) {
                if (tiles[x][y].status & MY_HARVESTED)
                    sources[MY][t]++;
                if (tiles[x][y].status & OPP_HARVESTED)
                    sources[OPP][t]++;
                if (!(tiles[x][y].status & (MY_HARVESTED | OPP_HARVESTED)))
                    sources[FREE][t]++;
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
    clear_list(temp, NULL);

    return array;
}

// Floyd_Warshall-related auxiliary functions -----------------------------------------------------------

#define FORBIDDEN 1000
#define RESTRICTED 50

#define EXPAND(a, b, c) (a) * (c) + (b)
#define EXPAND2(x1, y1, x2, y2) EXPAND(EXPAND((x1), (y1), height), EXPAND((x2), (y2), height), NT)
#define DISTANCES(x1, y1, x2, y2) distances[EXPAND2(x1, y1, x2, y2)]
#define PREVIOUS(x1, y1, x2, y2) previous[EXPAND2(x1, y1, x2, y2)]

int adjacent_vertices(struct vertex *v1, struct vertex *v2)
{
    for (struct vertex **pa = v1->a; pa - v1->a < 4; pa++)
        if (*pa && *pa == v2)
            return 1;

    return 0;
}

// [https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm]
void Floyd_Warshall(struct entity tiles[width][height], struct array_v *vertices,
        int *distances, struct entity **previous)
{
    const int NV = vertices->size;
    int (*distances_v)[NV] = malloc(NV * NV * sizeof(int));
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
                    distances_v[i][j] = FORBIDDEN;
                else {
                    distances_v[i][j] = 1;
                    if (ei->status & PROTECTED)
                        distances_v[i][j] += RESTRICTED;
                    if (ej->status & PROTECTED)
                        distances_v[i][j] += RESTRICTED;
                }
                previous_v[i][j] = vi;
            }
            // for each vertex
            else {
                distances_v[i][j] = i == j ? 0 : FORBIDDEN;
                previous_v[i][j] = i == j ? vi : NULL;
            }
        }
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
        distances[i] = FORBIDDEN;
        previous[i] = NULL;
    }
    for (int i = 0; i < NV; i++) {
        int x1 = vertices->array[i]->e->x;
        int y1 = vertices->array[i]->e->y;
        for (int j = 0; j < NV; j++) {
            int x2 = vertices->array[j]->e->x;
            int y2 = vertices->array[j]->e->y;
            if (distances_v[i][j] < DISTANCES(x1, y1, x2, y2)) {
                DISTANCES(x1, y1, x2, y2) = distances_v[i][j];
                PREVIOUS(x1, y1, x2, y2) = previous_v[i][j]->e;
            }
        }
    }

    free(distances_v);
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

// body-related auxiliary functions -----------------------------------------------------------

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

    // command specifics
    struct entity *target;
    struct entity *from_organ;
    struct entity *at_location;
    enum type new_organ_type;
    enum dir new_organ_dir;
    struct list *path;
};

struct opp_body {
    struct entity *root;

    // shortest distance to my organs
    // stores the simple index in the distance_to_organism and closest_organ matrices of body
    struct hash_map *closest_to_mine;
};

#define NO_INDEX -1

void populate_organisms(int n[2], struct hash_map *organisms, struct hash_map *opp_organisms,
        struct hash_map *organs)
{
    organisms = create_hash_map(n[MY]);
    opp_organisms = create_hash_map(n[MY]);

    for (int i = 0; i < n[MY]; i++) {
        struct body *new = malloc(sizeof(struct body));

        new->body_parts = create_list();
        new->distance_to_organism = malloc(NT * NT * sizeof(int));
        new->closest_organ = malloc(NT * NT * sizeof(struct entity *));
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                new->distance_to_organism[EXPAND(x, y, height)] = FORBIDDEN;
                new->closest_organ[EXPAND(x, y, height)] = NULL;
            }
        new->accessible_organs = create_list();
        new->vacant_slots = create_list();
        new->target = new->from_organ = new->at_location = NULL;
        new->new_organ_type = BASIC;
        new->new_organ_dir = N;
        new->path = create_list();

        put_map(organisms, i, new);
    }
    for (int i = 0; i < n[OPP]; i++) {
        struct opp_body *new = malloc(sizeof(struct opp_body));

        new->closest_to_mine = create_hash_map(n[MY]);
        for (int j = 0; j < n[MY]; j++) {
            int *index = malloc(sizeof(int));

            *index = NO_INDEX;
            put_map(new->closest_to_mine, j, index);
        }

        put_map(opp_organisms, i, new);
    }

    // establish the root organs
    int key_organism[2] = {0, 0};
    for (int i = 0; i < organs->size; i++) {
        struct entity *entity = get_map(organs, i);

        if (entity && entity->t == ROOT) {
            if (entity->o == MY)
                ((struct body *)get_map(organisms, key_organism[MY]++))->root = entity;
            else
                ((struct opp_body *)get_map(opp_organisms, key_organism[OPP]++))->root = entity;
        }
    }

    // fill-in the body parts
    for (int i = 0; i < organs->size; i++) {
        struct entity *entity = get_map(organs, i);

        if (entity && entity->o == MY)
            for (int j = 0; ; j++) {
                struct body *body = get_map(organisms, j);

                if (body->root->organ_id == entity->organ_root_id) {
                    add_front_list(body->body_parts, entity);
                    break;
                }
            }
    }
}

void del_inner_body(struct body *body)
{
    clear_list(body->body_parts, NULL);
    free(body->distance_to_organism);
    free(body->closest_organ);
    clear_list(body->accessible_organs, NULL);
    clear_list(body->vacant_slots, NULL);
    clear_list(body->path, NULL);
    free(body);
}

void del_opp_inner_body(struct opp_body *opp_body)
{
    clear_map(opp_body->closest_to_mine, free);
    free(opp_body);
}

#define D_TO_ORGANISM(x, y) organism->distance_to_organism[EXPAND(x, y, height)]
#define CLOSEST_ORGAN(x, y) organism->closest_organ[EXPAND(x, y, height)]

// tabulate my organism's disposition to engage
void inspect_surroundings(struct entity tiles[width][height], int *distances,
        struct entity *(*closest_sources)[4], struct hash_map *organisms,
        struct hash_map *opp_organisms)
{
    // calculate closest point to my_organism
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            for (int i = 0; i < organisms->size; i++) {
                struct body *organism = get_map(organisms, i);

                for (struct node *n = organism->body_parts->node; n; n = n->next) {
                    struct entity *e1 = n->content;
                    int d = DISTANCES(x, y, e1->x, e1->y);

                    if (d < D_TO_ORGANISM(x, y)) {
                        D_TO_ORGANISM(x, y) = d;
                        CLOSEST_ORGAN(x, y) = e1;
                    }
                }
            }
        }
}

/*
int compare_highest_value(void *data, struct entity *e0, struct entity *e1)
{
    (void)data;
    return e1->value - e0->value;
}

int compare_closest_from_myroot(void *data, struct entity *e0, struct entity *e1)
{
    struct entity *my_root = (struct entity *)data;

    return abs(e0->x - my_root->x) + abs(e0->y - my_root->y) -
            (abs(e1->x - my_root->x) + abs(e1->y - my_root->y));
}


    // fill-in lists of interesting tiles
    for (int i = 0; i < organs->size; i++) {
        struct entity *e = get_map(organs, i);

        if (!e)
            continue;
        for (int j = 0; j < organisms->size; j++) {
            struct body *organism = get_map(organisms, j);
            int d = organism->distance_to_organism[i];

            if (e->o == OPP && d < FORBIDDEN)
                add_front_list(organism->accessible_organs, e);
            else if (!(e->status & OCCUPIED) && d == 1)
                add_front_list(organism->vacant_slots, e);

            reorder_list(NULL, organism->accessible_organs,
                    (int (*)(void *, void *, void *))compare_highest_value);
            reorder_list(organism->root, organism->vacant_slots,
                    (int (*)(void *, void *, void *))compare_closest_from_myroot);
        }
    }
}
*/

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

// debugging assists  ------------------------------------------------------------------------------

void print_entities(struct entity tiles[width][height])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            struct entity *e = &tiles[x][y];

            if (e->t == WALL || e->t == EMPTY)
                continue;
            fprintf(stderr, "(%d, %d) %s %s owner:%s id:%d parent_id:%d root_id:%d status:%d value:%d\n",
                    x, y, type_str[e->t], dir_str[e->d], owner_str[e->o], e->organ_id, e->organ_parent_id,
                    e->organ_root_id, e->status, e->value);
        }
}

void print_organisms(struct hash_map *organisms)
{
    for (int i = 0; i < organisms->size; i++) {
        struct body *o = get_map(organisms, i);

        fprintf(stderr, "organism%d: root_id:%d body_parts:%d acc._tiles:%d vac._tiles:%d\n",
                i, o->root->organ_id, o->body_parts->size, o->accessible_organs->size,
                o->vacant_slots->size);
    }
}

void print_coords(struct entity *e)
{
    fprintf(stderr, "(%d, %d) ", e->x, e->y);
}

void print_path(struct list *path)
{
    iterate_over_list(path, (void (*)(void *))print_coords);
    putc('\n', stderr);
}

// main program  ------------------------------------------------------------------------------

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
    int *distances = malloc(NT * NT * sizeof(int));
    struct entity **previous = malloc(NT * NT * sizeof(struct entity *));

    init_grid(tiles);

    // game loop
    for (int loop = 0; ; loop++) {
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

#ifdef DEBUG
        // quality control -- print-out of the remarkable entities
        print_entities(tiles);
#endif

        // establish self-governing organisms
        populate_organisms(n_organisms, organisms, opp_organisms, organs);

        // establish vertex-entity relationships of possible nodes in new routes
        struct array_v *vertices = generate_vertices(tiles);

        // shortest path calculator of potential new routes
        Floyd_Warshall(tiles, vertices, distances, previous);

        struct entity *(*closest_sources)[4] = malloc(n_organisms[MY] * 4 * sizeof(struct entity *));
        // my organisms appraise their surroundings
        inspect_surroundings(tiles, distances, closest_sources, organisms, opp_organisms);

#ifdef DEBUG
        // quality control -- print-out of my organisms
        print_organisms(organisms);
#endif

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
#ifdef DEBUG
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
#ifdef DEBUG
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

        // output commands
        for (int i = 0; i < required_actions_count; i++) {
            struct body *organism = get_map(organisms, i);

            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");

            if (organism->target)
                printf("GROW %d %d %d %s %s",
                        organism->from_organ->organ_id,
                        organism->at_location->x,
                        organism->at_location->y,
                        type_str[organism->new_organ_type],
                        dir_str[organism->new_organ_dir]);
            else
                printf("WAIT");
            if (!loop)
                printf(" glhf");
            putchar('\n');
        }

        // clear arrays, lists, and maps
        clear_map(organs, NULL);
        clear_map(organisms, (void (*)(void *))del_inner_body);
        clear_map(opp_organisms, (void (*)(void *))del_opp_inner_body);
        free(vertices);
        free(closest_sources);
    }

    return 0;
}