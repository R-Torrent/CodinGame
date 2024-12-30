#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Grow and multiply your organisms to end up larger than your opponent.
 **/

 /*
 * Winter Challenge 2024
 * Contest
 */

/*
 * Wood 1 League
 */

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

size_t determine_enum(char *options[], char *selection)
{
    for (char **o = options; ; o++)
        if (!strcmp(*o, selection))
            return o - options;
}

// status flags for the tiles
// if A, B, C, D, or EMPTY then 0, 1 otherwise
#define OCCUPIED 001
// my protein source
#define MY_SOURCE 002
// opponent protein source
#define OPP_SOURCE 004
// protected by my tentacles
#define PROTECTED 010
// menaced by opponent tentacles
#define MENACED 040

struct entity {
    int x;
    // grid coordinate
    int y;
    // A, B, C, D, EMPTY, WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER
    enum type t;
    // 1 if your organ, 0 if enemy organ, -1 if neither
    int owner;
    // id of this entity if it's an organ, 0 otherwise
    int organ_id;
    // N,E,S,W or X if not an organ
    enum dir d;
    int organ_parent_id;
    int organ_root_id;
    int status;
    int value;
    int distance_to_organism;
    struct entity *closest_organ;
};

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

void clear_list(struct list *list)
{
    struct node *next;

    if (list)
        for (struct node *n = list->node; n; n = next) {
            next = n->next;
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
    if (map) {;
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

void clear_map(struct hash_map *map)
{
    if (map)
        for (struct node **pn = map->array; pn - map->array < map->size; pn++)
            while (*pn) {
                struct node *next = (*pn)->next;
                free(*pn);
                *pn = next;
            };
    free(map);
}

// auxiliary functions ----------------------------------------------------------------------

// columns in the game grid
int width;
// rows in the game grid
int height;
// NT = width * height
int NT;

void init_grid(struct entity tiles[width][height])
{
    const struct entity empty = {
        .t = EMPTY,
        .owner = -1,
        .organ_id = 0,
        .d = X,
        .organ_parent_id = 0,
        .organ_root_id = 0,
        .status = 0,
        .value = 0
    };

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            tiles[x][y] = empty;
            tiles[x][y].x = x;
            tiles[x][y].y = y;
        }
}

void determine_status(struct entity tiles[width][height])
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            int t = tiles[x][y].t;
            int source = (t == A || t == B || t == C || t == D);

            if (y < height - 1 && tiles[x][y + 1].d == N) {
                if (tiles[x][y + 1].t == HARVESTER)
                    tiles[x][y].status |= tiles[x][y + 1].owner ? MY_SOURCE : OPP_SOURCE;
                else if (tiles[x][y + 1].t == TENTACLE)
                    tiles[x][y].status |= tiles[x][y + 1].owner ? PROTECTED : MENACED;
            }
            if (x > 0 && tiles[x - 1][y].d == E) {
                if (tiles[x - 1][y].t == HARVESTER)
                    tiles[x][y].status |= tiles[x - 1][y].owner ? MY_SOURCE : OPP_SOURCE;
                else if (tiles[x - 1][y].t == TENTACLE)
                    tiles[x][y].status |= tiles[x - 1][y].owner ? PROTECTED : MENACED;
            }
            if (y > 0 && tiles[x][y - 1].d == S) {
                if (tiles[x][y - 1].t == HARVESTER)
                    tiles[x][y].status |= tiles[x][y - 1].owner ? MY_SOURCE : OPP_SOURCE;
                else if (tiles[x][y - 1].t == TENTACLE)
                    tiles[x][y].status |= tiles[x][y - 1].owner ? PROTECTED : MENACED;
            }
            if (x < width - 1 && tiles[x + 1][y].d == W) {
                if (tiles[x + 1][y].t == HARVESTER)
                    tiles[x][y].status |= tiles[x + 1][y].owner ? MY_SOURCE : OPP_SOURCE;
                else if (tiles[x + 1][y].t == TENTACLE)
                    tiles[x][y].status |= tiles[x + 1][y].owner ? PROTECTED : MENACED;
            }
            if (!source && t != EMPTY)
                tiles[x][y].status |= OCCUPIED;
        }
}

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

#define FORBIDDEN 1000
#define RESTRICTED 50

#define EXPAND(a, b, c) (a) * (c) + (b)
#define EXPAND2(x1, y1, x2, y2) EXPAND(EXPAND((x1), (y1), height), EXPAND((x2), (y2), height), NT)
#define DISTANCES(x1, y1, x2, y2) distances[EXPAND2(x1, y1, x2, y2)]
#define PREVIOUS(x1, y1, x2, y2) previous[EXPAND2(x1, y1, x2, y2)]

void Floyd_Warshall(int *distances, struct entity **previous, struct entity tiles[width][height])
{
    int *p, d, i1, k1;

    // initialization & restrict areas ahead of a tentacle
    for (int i = 0; i < NT; i++)
        for (int j = 0; j < NT; j++) {
            int k = EXPAND(i, j, NT);
            distances[k] = i == j ? 0 : FORBIDDEN;
            previous[k] = i == j ? (struct entity *)tiles + i : NULL;
        }
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            if (y < height - 1) {
                if (!(tiles[x][y].status & OCCUPIED && tiles[x][y + 1].status & OCCUPIED))
                    DISTANCES(x, y, x, y + 1) =
                            (tiles[x][y + 1].t != TENTACLE || tiles[x][y + 1].d != N) ? 1 :
                            tiles[x][y + 1].owner ? RESTRICTED :
                            FORBIDDEN;
                PREVIOUS(x, y, x, y + 1) = &tiles[x][y];
            }
            if (x > 0) {
                if (!(tiles[x][y].status & OCCUPIED && tiles[x - 1][y].status & OCCUPIED))
                    DISTANCES(x, y, x - 1, y) =
                            (tiles[x - 1][y].t != TENTACLE || tiles[x - 1][y].d != E) ? 1 :
                            tiles[x - 1][y].owner ? RESTRICTED :
                            FORBIDDEN;
                PREVIOUS(x, y, x - 1, y) = &tiles[x][y];
            }
            if (y > 0) {
                if (!(tiles[x][y].status & OCCUPIED && tiles[x][y - 1].status & OCCUPIED))
                    DISTANCES(x, y, x, y - 1) =
                            (tiles[x][y - 1].t != TENTACLE || tiles[x][y - 1].d != S) ? 1 :
                            tiles[x][y - 1].owner ? RESTRICTED :
                            FORBIDDEN;
                PREVIOUS(x, y, x, y - 1) = &tiles[x][y];
            }
            if (x < width - 1) {
                if (!(tiles[x][y].status & OCCUPIED && tiles[x + 1][y].status & OCCUPIED))
                    DISTANCES(x, y, x + 1, y) =
                            (tiles[x + 1][y].t != TENTACLE || tiles[x + 1][y].d != W) ? 1 :
                            tiles[x + 1][y].owner ? RESTRICTED :
                            FORBIDDEN;
                PREVIOUS(x, y, x + 1, y) = &tiles[x][y];
            }
        }

    // algorithm
    for (int k = 0; k < NT; k++)
        for (int i = 0; i < NT; i++)
            for (int j = 0; j < NT; j++)
                if (*(p = distances + (i1 = EXPAND(i, j, NT))) >
                        (d = distances[EXPAND(i, k, NT)] + distances[(k1 = EXPAND(k, j, NT))])) {
                    *p = d;
                    previous[i1] = previous[k1];
                }
}

// shortest path reconstruction from the Floyd-Warshall alorithm
struct list *path_FW(struct entity tiles[width][height],
        struct entity **previous, int x1, int y1, int x2, int y2)
{
    struct list *path = create_list();

    if (!previous[EXPAND2(x1, y1, x2, y2)])
        return NULL;
    add_front_list(path, &tiles[x2][y2]);
    while (x1 != x2 || y1 != y2) {
        struct entity *pe = previous[EXPAND2(x1, y1, x2, y2)];
        x2 = pe->x;
        y2 = pe->y;
        add_front_list(path, &tiles[x2][y2]);
    }
    return path;
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

// main porgram  ------------------------------------------------------------------------------

int main()
{
    scanf("%d%d", &width, &height);
    NT = width * height;

    // memmory allocation
    struct entity (*tiles)[height] = malloc(NT * sizeof(struct entity));
    int my_proteins[4];
    int opp_proteins[4];
    int my_sources[4];
    int opp_sources[4];
    int *distances = malloc(NT * NT * sizeof(int));
    struct entity **previous = malloc(NT * NT * sizeof(struct entity *));

    // organs of interest
    struct entity *my_root;
    struct entity *opp_root;
    struct entity *target;

    // game loop
    for (int loop = 0; ; loop++) {
        init_grid(tiles);
        struct hash_map *organs = create_hash_map(NT + 1);
        memset(my_sources, 0, 4 * sizeof(int));
        memset(opp_sources, 0, 4 * sizeof(int));

        int entity_count;
        scanf("%d", &entity_count);
        for (int i = 0; i < entity_count; i++) {
            int x, y;
            scanf("%d%d", &x, &y);
            struct entity *entity = &tiles[x][y];
            char type[33];
            scanf("%s", type);
            entity->t = (enum type)determine_enum(type_str, type);
            scanf("%d%d", &entity->owner, &entity->organ_id);
            if (!loop && entity->t == ROOT) {
                if (entity->owner)
                    my_root = entity;
                else
                    opp_root = entity;
            }
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

        // determine status for each tile
        determine_status(tiles);

        // shortest path calculator
        Floyd_Warshall(distances, previous, tiles);

        // calcultate entity values and active protein sources
        // value = its own + that of all its descendants
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                int t = tiles[x][y].t;
                int value = t == BASIC ? 1 : t == HARVESTER || t == TENTACLE
                        || t == SPORER ? 2 : t == ROOT ? 3 : 0;

                if (tiles[x][y].status & MY_SOURCE)
                    my_sources[t]++;
                else if (tiles[x][y].status & OPP_SOURCE)
                    opp_sources[t]++;
                else if (value) {
                    struct entity *entity = get_map(organs, tiles[x][y].organ_id);
                    while (entity->organ_parent_id) {
                        entity->value += value;
                        entity = get_map(organs, entity->organ_parent_id);
                    };
                    entity->value += value;
                }
            }

        // calculate closest point to my_organism
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                tiles[x][y].distance_to_organism = FORBIDDEN;
                tiles[x][y].closest_organ = NULL;
                for (int x1 = 0; x1 < width; x1++)
                    for (int y1 = 0; y1 < height; y1++) {
                        if (tiles[x1][y1].owner != 1)
                            continue;
                        int d = DISTANCES(x, y, x1, y1);
                        if (d < tiles[x][y].distance_to_organism) {
                            tiles[x][y].distance_to_organism = d;
                            tiles[x][y].closest_organ = &tiles[x1][y1];
                        }
                        if (!d) // closest organ == itself!
                            goto escape;
                    }
escape:         ;
            }

        // quality control -- print-out of the remarkable entities
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                struct entity *e = &tiles[x][y];
                if (e->t == WALL || e->t == EMPTY)
                    continue;
                fprintf(stderr, "(%d, %d) %s %s owner:%d id:%d parent_id:%d root_id:%d status:%d value:%d dist_to_%d:%d\n",
                        x, y, type_str[e->t], dir_str[e->d], e->owner, e->organ_id, e->organ_parent_id,
                        e->organ_root_id, e->status, e->value, e->closest_organ ? e->closest_organ->organ_id : 0,
                        e->closest_organ ? e->distance_to_organism : FORBIDDEN);
            }
        if (target)
                fprintf(stderr, "Previous target (%d, %d)\n", target->x, target->y);

        struct list *vulnerable_organs = create_list();
        struct list *accessible_organs = create_list();
        struct list *vacant_slots = create_list();
        struct list *path = NULL;
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                struct entity *e = &tiles[x][y];
                if (!e->owner) {
                    if (e->distance_to_organism == 2)
                        add_front_list(vulnerable_organs, e);
                    if (e->distance_to_organism < FORBIDDEN)
                        add_front_list(accessible_organs, e);
                }
                else if (!(e->status & OCCUPIED) && e->distance_to_organism == 1)
                    add_front_list(vacant_slots, e);
            }

        reorder_list(NULL, vulnerable_organs, (int (*)(void *, void *, void *))compare_highest_value);
        reorder_list(NULL, accessible_organs, (int (*)(void *, void *, void *))compare_highest_value);
        reorder_list(my_root, vacant_slots, (int (*)(void *, void *, void *))compare_closest_from_myroot);

        // strategy selection
        struct entity *my_organ;
        enum type new_organ_type = BASIC;
        enum dir new_organ_dir = N;
        // primary objective: to establish a tentacle if possible
        if (vulnerable_organs->size && my_proteins[B] && my_proteins[C]) {
            // select the highest-value enemy organ
            target = vulnerable_organs->node->content;
            fprintf(stderr, "Aiming for an enemy organ. Targeting %d (%d, %d)\n",
                    target->organ_id, target->x, target->y);
            my_organ = target->closest_organ;
            new_organ_type = TENTACLE;
            path = path_FW(tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
            target = path->node->next->content;
            new_organ_dir = face_to(target, vulnerable_organs->node->content);
        }
        // secondary objective: to grow towards the highest-value enemy organ
        else if (accessible_organs->size && my_proteins[A]) {
            target = accessible_organs->node->content;
            fprintf(stderr, "Growing towards an enemy organ. Targeting %d (%d, %d)\n",
                    target->organ_id, target->x, target->y);
            my_organ = target->closest_organ;
//            path = path_FW(tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
//            target = path->node->next->e;
        }
        // fallback strategy: to grow wherever possible
        else if (vacant_slots->size && my_proteins[A]) {
            target = vacant_slots->node->content;
            my_organ = target->closest_organ;
            fprintf(stderr, "Strategy: Simply grow where possible\n");
            // choose any empty slot, favoring slots close from my_root and away from the tentacles
            fprintf(stderr, "Empty slot to grow into (%d, %d)\n", target->x, target->y);
        }
        else {
            fprintf(stderr, "Nothing to do!\n");
            target = NULL;
        }

        // clear lists and maps
        clear_list(vulnerable_organs);
        clear_list(accessible_organs);
        clear_list(vacant_slots);
        clear_list(path);
        clear_map(organs);
        for (int i = 0; i < required_actions_count; i++) {

            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");

            // output command
            if (target)
                printf("GROW %d %d %d %s %s",
                        my_organ->organ_id,
                        target->x,
                        target->y,
                        type_str[new_organ_type],
                        dir_str[new_organ_dir]);
            else
                printf("WAIT");
            if (!loop)
                printf(" glhf");
            putchar('\n');
        }
    }

    return 0;
}