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
 * Wood 3 League
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
    int distance_to_root;
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

#define MAX 1000

int distance(struct entity *pe0, struct entity *pe1)
{
    return abs(pe1->x - pe0->x) + abs(pe1->y - pe0->y);
}

void add_to_list(struct list **plist, struct entity *pe)
{
    struct list *new = malloc(sizeof(struct list));

    new->e = pe;
    new->next = *plist;
    *plist = new;
}

void reorder_list(struct list *list, int (*compare)(struct entity *, struct entity *))
{
    for (struct list *pe0 = list; pe0 != NULL; pe0 = pe0->next)
        for (struct list *pe1 = pe0; pe1->next != NULL; pe1 = pe1->next)
            if (compare(pe1->e, pe1->next->e) > 0) {
                struct entity *temp = pe1->e;
                pe1->e = pe1->next->e;
                pe1->next->e = temp;
            }
}

int compare_distance_to_root(struct entity *pe0, struct entity *pe1)
{
    return pe0->distance_to_root - pe1->distance_to_root;
}

void remove_from_list(struct list **plist, struct entity *pe)
{
    struct list *next;

    if (!*plist || !pe)
        return;
    if ((*plist)->e == pe) {
        next = (*plist)->next;
        free(*plist);
        *plist = next;
        remove_from_list(plist, pe);
    }
    else
        remove_from_list(&(*plist)->next, pe);
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

int main()
{
    // columns in the game grid
    int width;
    // rows in the game grid
    int height;
    scanf("%d%d", &width, &height);

    struct entity (*tiles)[height] = calloc(width * height, sizeof(struct entity));
    int my_proteins[4];
    int opp_proteins[4];

    struct entity empty = {
        .t = EMPTY,
        .owner = -1,
        .organ_id = 0,
        .d = X,
        .organ_parent_id = 0,
        .organ_root_id = 0,
    };
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            tiles[x][y] = empty;
            tiles[x][y].x = x;
            tiles[x][y].y = y;
        }

    // A protein points of interest
    struct entity *harvestA = NULL;
    struct entity *targetA = NULL;
    struct entity *sourceA = NULL;

    struct entity *my_root = NULL;
    struct entity *opp_root;

    // game loop
    for (int loop = 0; ; loop++) {
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
                if (tiles[x][y].owner == 1)
                    my_root = &tiles[x][y];
                else
                    opp_root = &tiles[x][y];
            }
            char organ_dir[2];
            scanf("%s", organ_dir);
            tiles[x][y].d = (enum dir)determine_enum(dir_str, organ_dir);
            scanf("%d%d",&tiles[x][y].organ_parent_id, &tiles[x][y].organ_root_id);
            //fprintf(stderr, "(%d, %d): %d %s %s\n", x, y,
            //    tiles[x][y].organ_id, type_str[tiles[x][y].t], dir_str[tiles[x][y].d]);
        }
        // your protein stock
        scanf("%d%d%d%d", my_proteins + A, my_proteins + B, my_proteins + C, my_proteins + D);
        // opponent's protein stock
        scanf("%d%d%d%d", opp_proteins + A, opp_proteins + B, opp_proteins + C, opp_proteins + D);
        // your number of organisms, output an action for each one in any order
        int required_actions_count;
        scanf("%d", &required_actions_count);

        // list of proteinA souces within reach
        struct list *proteinA_candidates = NULL;
        int number_candidates = 0;

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++) {
                if (!loop)
                    tiles[x][y].distance_to_root = distance(&tiles[x][y], my_root);
                tiles[x][y].distance_to_organism = MAX;
                tiles[x][y].closest_organ = NULL;
                for (int x1 = 0; x1 < width; x1++)
                    for (int y1 = 0; y1 < height; y1++) {
                        if (tiles[x1][y1].owner != 1)
                            continue;
                        int d = distance(&tiles[x][y], &tiles[x1][y1]);
                        if (d < tiles[x][y].distance_to_organism) {
                            tiles[x][y].distance_to_organism = d;
                            tiles[x][y].closest_organ = &tiles[x1][y1];
                        }
                        if (!d) // closest organ == itself!
                            goto escape1;
                    }
escape1:        if (tiles[x][y].t == A && tiles[x][y].distance_to_organism <= my_proteins[A]) {
                    add_to_list(&proteinA_candidates, &tiles[x][y]);
                    number_candidates++;
                }
            }

        // candidates ordered according to distance from root
        reorder_list(proteinA_candidates, compare_distance_to_root);
        if (harvestA) {
            if (harvestA->t == A)
                fprintf(stderr, "Protein A objective (%d, %d)\n", harvestA->x, harvestA->y);
            else {
                fprintf(stderr, "Protein feeding objective (%d, %d) destroyed!\n", harvestA->x, harvestA->y);
                harvestA = NULL;
            }
        }
        if (targetA)
                fprintf(stderr, "Previous growth target (%d, %d)\n", targetA->x, targetA->y);
        if (sourceA) {
            if (sourceA->t == A)
                fprintf(stderr, "Steady source of A proteins (%d, %d)\n", sourceA->x, sourceA->y);
            else {
                fprintf(stderr, "Protein A source (%d, %d) destroyed!\n", sourceA->x, sourceA->y);
                sourceA = NULL;
            }
        }

        // strategy selection
        // primary objective: to establish protein A source
        if (!harvestA && my_proteins[C] && my_proteins[D] && number_candidates) {
            targetA = NULL;
            fprintf(stderr, "Aiming for harvest source\n");
            harvestA = proteinA_candidates->e;
            fprintf(stderr, "New objective (%d, %d)\n", harvestA->x, harvestA->y);
        }
        // secondary objective: to gobble the other protein sources
        else if (!harvestA && (!targetA || targetA->t != A)) {
            // remove harvest source from candidate list
            if (sourceA) {
                remove_from_list(&proteinA_candidates, sourceA);
                number_candidates--;
            }
            fprintf(stderr, "Determining new target\n");
            // Select new targetA
            if (number_candidates) {
                fprintf(stderr, "Strategy: Go for the proteins\n");
                // pick the first candidate after the half-way mark
                int target_index = number_candidates / 2 + number_candidates % 2 - 1;
                struct list *listA = proteinA_candidates;
                while (target_index--)
                    listA = listA->next;
                targetA = listA->e;
                fprintf(stderr, "New protein-rich target (%d, %d)\n", targetA->x, targetA->y);
            }
            else {
                fprintf(stderr, "Strategy: Simply grow where possible\n");
                // choose any empty slot next to an organ
                for (int x = 0; x < width; x++)
                    for (int y = 0; y < height; y++)
                        if (tiles[x][y].t == EMPTY && tiles[x][y].distance_to_organism == 1) {
                            targetA = &tiles[x][y];
                            goto escape2;
                        }
escape2:        fprintf(stderr, "Empty slot to grow into (%d, %d)\n", targetA->x, targetA->y);
            }
        }

        // clear candidate list
        clear_list(&proteinA_candidates);

        // error fallback strategy
        if (!harvestA && !targetA) {
            fprintf(stderr, "Something went wrong! Default behavior: Grow towards the enemy root\n");
            targetA = opp_root;
        }

        for (int i = 0; i < required_actions_count; i++) {

            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");

            struct entity *my_organ, *objective = NULL;
            enum type new_organ = BASIC;
            enum dir harvester_dir = N;

            if (targetA) { // grow towards other protein sources
                my_organ = targetA->closest_organ;
                objective = targetA;
            }
            else if (distance(harvestA->closest_organ, harvestA) > 2) { // grow towards primary protein source
                my_organ = harvestA->closest_organ;
                objective = harvestA;
            }
            else {
                if (tiles[harvestA->x][harvestA->y + 1].t == EMPTY &&
                        ((my_organ = &tiles[harvestA->x + 1][harvestA->y + 1])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x][harvestA->y + 2])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x - 1][harvestA->y + 1])->owner == 1)) {
                    harvester_dir = N;
                    objective = &(struct entity){.x = harvestA->x, .y = harvestA->y + 1};
                }
                else if (tiles[harvestA->x - 1][harvestA->y].t == EMPTY &&
                        ((my_organ = &tiles[harvestA->x - 1][harvestA->y + 1])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x - 2][harvestA->y])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x - 1][harvestA->y - 1])->owner == 1)) {
                    harvester_dir = E;
                    objective = &(struct entity){.x = harvestA->x - 1, .y = harvestA->y};
                }
                else if (tiles[harvestA->x][harvestA->y - 1].t == EMPTY &&
                        ((my_organ = &tiles[harvestA->x - 1][harvestA->y - 1])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x][harvestA->y - 2])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x + 1][harvestA->y - 1])->owner == 1)) {
                    harvester_dir = S;
                    objective = &(struct entity){.x = harvestA->x, .y = harvestA->y - 1};
                }
                else if (tiles[harvestA->x + 1][harvestA->y].t == EMPTY &&
                        ((my_organ = &tiles[harvestA->x + 1][harvestA->y - 1])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x + 2][harvestA->y])->owner == 1 ||
                        (my_organ = &tiles[harvestA->x + 1][harvestA->y + 1])->owner == 1)) {
                    harvester_dir = W;
                    objective = &(struct entity){.x = harvestA->x + 1, .y = harvestA->y};
                }
                if (objective) {
                    new_organ = HARVESTER;
                    sourceA = harvestA;
                    harvestA = NULL;
                }
                else { // complicated cases, such as starting next to the protein source
                    my_organ = harvestA->closest_organ;
                    objective = opp_root;
                }
            }

            // output command
            printf("GROW %d %d %d %s %s",
                    my_organ->organ_id,
                    objective->x,
                    objective->y,
                    type_str[new_organ],
                    dir_str[harvester_dir]);
            if (!loop)
                printf(" glhf");
            putchar('\n');
        }
    }

    return 0;
}