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
 * Wood 4 League
 */

#define TYPE    \
X(WALL)         \
X(ROOT)         \
X(BASIC)        \
X(TENTACLE)     \
X(HARVESTER)    \
X(SPORER)       \
X(A)            \
X(B)            \
X(C)            \
X(D)

#define X(a) a,
enum type {
    TYPE
};
#undef X

#define X(a) #a,
char *type_s[] = {
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
char *dir_s[] = {
    DIR
};
#undef Y

struct entity {
    int x;
    // grid coordinate
    int y;
    // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
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
    struct entity *neigbors[4];
};

struct list {
    struct entity *e;
    struct list *next;
};

size_t determine(char *options[], char *selection)
{
    for (char **o = options; ; o++)
        if (!strcmp(*o, selection))
            return o - options;
}

int distance(struct entity *pe0, struct entity *pe1)
{
    return abs(pe1->x - pe0->x) + abs(pe1->y - pe0->y);
}

#define INIT 1000

int main()
{
    // columns in the game grid
    int width;
    // rows in the game grid
    int height;
    scanf("%d%d", &width, &height);

    // game loop
    for (int loop = 0; ; loop++) {
        struct entity *my_root;
        struct entity *opp_root;
        int entity_count;
        scanf("%d", &entity_count);
        struct entity ent[entity_count];
        for (struct entity *pe = ent; pe - ent < entity_count; pe++) {
            scanf("%d%d", &pe->x, &pe->y);
            char type[33];
            scanf("%s", type);
            pe->t = (enum type)determine(type_s, type);
            scanf("%d%d", &pe->owner, &pe->organ_id);
            if (pe->t == ROOT) {
                if (pe->owner == 1)
                    my_root = pe;
                else
                    opp_root = pe;
            }
            char organ_dir[2];
            scanf("%s", organ_dir);
            pe->d = (enum dir)determine(dir_s, organ_dir);
            scanf("%d%d",&pe->organ_parent_id, &pe->organ_root_id);
        }
        int my_a;
        int my_b;
        int my_c;
        // your protein stock
        int my_d;
        scanf("%d%d%d%d", &my_a, &my_b, &my_c, &my_d);
        int opp_a;
        int opp_b;
        int opp_c;
        // opponent's protein stock
        int opp_d;
        scanf("%d%d%d%d", &opp_a, &opp_b, &opp_c, &opp_d);
        // your number of organisms, output an action for each one in any order
        int required_actions_count;
        scanf("%d", &required_actions_count);

        for (struct entity *pe0 = ent; pe0 - ent < entity_count; pe0++) {
            pe0->distance_to_root = distance(pe0, my_root);
            pe0->distance_to_organism = INIT;
            pe0->closest_organ = NULL;
            for (struct entity *pe1 = ent; pe1 - ent < entity_count; pe1++) {
                if (pe1->owner != 1)
                    continue;
                int d = distance(pe0, pe1);
                if (d < pe0->distance_to_organism) {
                    pe0->distance_to_organism = d;
                    pe0->closest_organ = pe1;
                }
            }
            pe0->neigbors[N] = NULL;
            pe0->neigbors[E] = NULL;
            pe0->neigbors[S] = NULL;
            pe0->neigbors[W] = NULL;
            for (struct entity *pe1 = ent; pe1 - ent < entity_count; pe1++) {
                if (pe0->y - 1 == pe1->y) {
                    pe0->neigbors[N] = pe1;
                    break;
                }
                if (pe0->x + 1 == pe1->x) {
                    pe0->neigbors[E] = pe1;
                    break;
                }
                if (pe0->y + 1 == pe1->y) {
                    pe0->neigbors[S] = pe1;
                    break;
                }
                if (pe0->x - 1 == pe1->x) {
                    pe0->neigbors[W] = pe1;
                    break;
                }
            }
        }

        // list of proteinA souces within reach
        struct list *proteinA_candidates = NULL;
        int number_candidates = 0;
        for (struct entity *pe = ent; pe - ent < entity_count; pe++) {
            //if (pe->t != A || pe->distance_to_organism > my_a)
            //    continue;
            if (pe->t != A)
                continue;
            struct list *new = malloc(sizeof(struct list));
            new->e = pe;
            new->next = proteinA_candidates;
            proteinA_candidates = new;
            number_candidates++;
        }
        struct entity *targetA = NULL;
        if (number_candidates) {
            // candidates ordered according to distance from root
            for (struct list *pA0 = proteinA_candidates; pA0 != NULL; pA0 = pA0->next)
                for (struct list *pA1 = pA0; pA1->next != NULL; pA1 = pA1->next)
                    if (pA1->e->distance_to_root > pA1->next->e->distance_to_root) {
                        struct entity *temp = pA1->e;
                        pA1->e = pA1->next->e;
                        pA1->next->e = temp;
                    }
            // pick the first candidate after the half-way mark
            int target_index = number_candidates / 2 + number_candidates % 2 - 1;
            struct list *listA = proteinA_candidates;
            while (target_index--)
                listA = listA->next;
            targetA = listA->e;
            // clear list
            struct list *next;
            while (proteinA_candidates)
            {
                next = proteinA_candidates->next;
                free(proteinA_candidates);
                proteinA_candidates = next;
            }
        }
        else
            // choose any empty slot next to an organ
            for (struct entity *pe = ent; pe - ent < entity_count; pe++) {
                if (pe->owner != 1)
                    continue;
                if (pe->y > 0 && !pe->neigbors[N]) {
                    targetA = &(struct entity){pe->x, pe->y - 1};
                    break;
                }
                if (pe->x < width - 1 && !pe->neigbors[E]) {
                    targetA = &(struct entity){pe->x + 1, pe->y};
                    break;
                }
                if (pe->y < height - 1 && !pe->neigbors[S]) {
                    targetA = &(struct entity){pe->x, pe->y + 1};
                    break;
                }
                if (pe->x > 0 && !pe->neigbors[W]) {
                    targetA = &(struct entity){pe->x - 1, pe->y};
                    break;
                }
            }
        // last resort, if error
        if (!targetA)
            targetA = opp_root;

        for (int i = 0; i < required_actions_count; i++) {
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
            printf("GROW %d %d %d BASIC",
                    targetA->closest_organ->organ_id,
                    targetA->x,
                    targetA->y);
            if (!loop)
                printf(" glhf");
            putchar('\n');
        }
    }

    return 0;
}