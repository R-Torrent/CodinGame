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
 * Wood 2 League
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
    int distance_to_organism;
    struct entity *closest_organ;
};

struct list {
    struct entity *e;
    struct list *next;
};

size_t determine_enum(char *options[], char *selection)
{
    for (char **o = options; ; o++)
        if (!strcmp(*o, selection))
            return o - options;
}

void add_to_list(struct list **plist, struct entity *pe)
{
    struct list *new = malloc(sizeof(struct list));

    new->e = pe;
    new->next = *plist;
    *plist = new;
}

int list_size(struct list *list)
{
    int size = 0;

    while (list) {
        size++;
        list = list->next;
    }
    return size;
}

void reorder_list(void *data, struct list *list,
        int (*compare)(void *, struct entity *, struct entity *))
{
    for (struct list *pe0 = list; pe0 != NULL; pe0 = pe0->next)
        for (struct list *pe1 = pe0->next; pe1 != NULL; pe1 = pe1->next)
            if (compare(data, pe0->e, pe1->e) > 0) {
                struct entity *temp = pe0->e;
                pe0->e = pe1->e;
                pe1->e = temp;
            }
}

int compare_lowest_id(void *data, struct entity *pe0, struct entity *pe1)
{
    (void)data;
    return pe0->organ_id - pe1->organ_id;
}

int compare_closest_from_myroot(struct entity *my_root, struct entity *pe0, struct entity *pe1)
{
    return abs(pe0->x - my_root->x) + abs(pe0->y - my_root->y) -
            (abs(pe1->x - my_root->x) + abs(pe1->y - my_root->y));
}

int remove_from_list(struct list **plist, struct entity *pe)
{
    struct list *next;

    if (!*plist || !pe)
        return 0;
    if ((*plist)->e == pe) {
        next = (*plist)->next;
        free(*plist);
        *plist = next;
        return 1 + remove_from_list(plist, pe);
    }
    else
        return remove_from_list(&(*plist)->next, pe);
}

void clear_list(struct list **plist)
{
    struct list *next;

    while (*plist)
    {
        next = (*plist)->next;
        free(*plist);
        *plist = next;
    }
}

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
        .status = 0
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
struct list *path_FW(struct list **ppath, struct entity tiles[width][height], struct entity **previous,
        int x1, int y1, int x2, int y2)
{
    if (!previous[EXPAND2(x1, y1, x2, y2)])
        return NULL;
    add_to_list(ppath, &tiles[x2][y2]);
    while (x1 != x2 || y1 != y2) {
        struct entity *pe = previous[EXPAND2(x1, y1, x2, y2)];
        x2 = pe->x;
        y2 = pe->y;
        add_to_list(ppath, &tiles[x2][y2]);
    }
    return *ppath;
}

int main()
{
    scanf("%d%d", &width, &height);
    NT = width * height;

    struct entity (*tiles)[height] = malloc(NT * sizeof(struct entity));
    int my_proteins[4];
    int opp_proteins[4];
    int *distances = malloc(NT * NT * sizeof(int));
    struct entity **previous = malloc(NT * NT * sizeof(struct entity *));

    // organs of interest
    struct entity *my_root;
    struct entity *opp_root;
    struct entity *target;

    // game loop
    for (int loop = 0; ; loop++) {
        init_grid(tiles);

        int entity_count;
        scanf("%d", &entity_count);
        for (int i = 0; i < entity_count; i++) {
            int x, y;
            scanf("%d%d", &x, &y);
            char type[33];
            scanf("%s", type);
            tiles[x][y].t = (enum type)determine_enum(type_str, type);
            scanf("%d%d", &tiles[x][y].owner, &tiles[x][y].organ_id);
            if (!loop && tiles[x][y].t == ROOT) {
                if (tiles[x][y].owner)
                    my_root = &tiles[x][y];
                else
                    opp_root = &tiles[x][y];
            }
            char organ_dir[2];
            scanf("%s", organ_dir);
            tiles[x][y].d = (enum dir)determine_enum(dir_str, organ_dir);
            scanf("%d%d",&tiles[x][y].organ_parent_id, &tiles[x][y].organ_root_id);
            //fprintf(stderr, "(%d, %d): %d %s %s\n", x, y,
            //        tiles[x][y].organ_id, type_str[tiles[x][y].t], dir_str[tiles[x][y].d]);
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

        if (target)
                fprintf(stderr, "Previous target (%d, %d)\n", target->x, target->y);

        struct list *vulnerable_organs = NULL;
        struct list *accessible_organs = NULL;
        struct list *vacant_slots = NULL;
        struct list *path = NULL;
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                if (!tiles[x][y].owner) {
                    if (tiles[x][y].distance_to_organism == 2)
                        add_to_list(&vulnerable_organs, &tiles[x][y]);
                    if (tiles[x][y].distance_to_organism < FORBIDDEN)
                        add_to_list(&accessible_organs, &tiles[x][y]);
                }
                else if (!(tiles[x][y].status & OCCUPIED)
                        && tiles[x][y].distance_to_organism == 1)
                    add_to_list(&vacant_slots, &tiles[x][y]);
            }
        reorder_list(NULL, vulnerable_organs, compare_lowest_id);
        reorder_list(NULL, accessible_organs, compare_lowest_id);
        reorder_list(my_root, vacant_slots,
                (int (*)(void *, struct entity *, struct entity *))compare_closest_from_myroot);
        fprintf(stderr, "Vacant slots: %d\n", list_size(vacant_slots));

        struct entity *my_organ;
        enum type new_organ_type = BASIC;
        enum dir new_organ_dir = N;

        // strategy selection
        // primary objective: to establish a tentacle if possible
        if (vulnerable_organs && my_proteins[B] && my_proteins[C]) {
            // select the oldest organ with, possibly, very many children
            target = vulnerable_organs->e;
            fprintf(stderr, "Aiming for an enemy organ. Targeting %d (%d, %d)\n",
                    target->organ_id, target->x, target->y);
            my_organ = target->closest_organ;
            new_organ_type = TENTACLE;
            path_FW(&path, tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
            target = path->next->e;
            if (vulnerable_organs->e->x == target->x)
                new_organ_dir = vulnerable_organs->e->y < target->y ? N : S;
            else if (vulnerable_organs->e->y == target->y)
                new_organ_dir = vulnerable_organs->e->x < target->x ? W : E;            
        }
        // secondary objective: to grow towards the oldest enemy organ possible (== lowest organ_id)
        else if (accessible_organs && my_proteins[A]) {
            target = accessible_organs->e;
            fprintf(stderr, "Growing towards an enemy organ. Targeting %d (%d, %d)\n",
                    target->organ_id, target->x, target->y);
            my_organ = target->closest_organ;
            path_FW(&path, tiles, previous, my_organ->x, my_organ->y, target->x, target->y);
            target = path->next->e;
            
        }
        // fallback strategy: to grow wherever possible
        else if (vacant_slots && my_proteins[A]) {
            target = vacant_slots->e;
            my_organ = target->closest_organ;
            fprintf(stderr, "Strategy: Simply grow where possible\n");
            // choose any empty slot, favoring slots close from my_root and away from the tentacles
            fprintf(stderr, "Empty slot to grow into (%d, %d)\n", target->x, target->y);
        }
        else {
            fprintf(stderr, "Nothing to do!\n");
            target = NULL;
        }

        // clear lists
        clear_list(&vulnerable_organs);
        clear_list(&accessible_organs);
        clear_list(&vacant_slots);
        clear_list(&path);

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