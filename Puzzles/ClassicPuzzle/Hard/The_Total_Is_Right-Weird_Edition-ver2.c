// Second attempt on this hard problem: 100% score

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * The Total Is Right - Weird Edition
 * Puzzles > Classic Puzzle > Hard
 */

#define LIMIT 12 // game's limit on the appearances of number 'a'

typedef struct Node {
	struct Node *left, *right; // left and right children in the search tree set
	struct Node *next; // next integer with equal solution
	int i;
} node_t;

bool check_op(node_t **, node_t **, int);
node_t *check_n_insert(node_t **, int);
void clear_tree_set(node_t *);

int N, a, top;

int main()
{
	scanf("%d%d", &N, &a);
	top = N * a * a * a;

	node_t *tree_set = NULL; // ordered tree for all integers encountered
	node_t *solutions[LIMIT] = { NULL }; // list of equal solutions

	int u = 1; // u: uses of 'a'
	if (check_op(&tree_set, solutions, a))
		goto end;
	for (u = 2; u <= LIMIT; u++)
		for (int v = 1; v <= u / 2; v++) {
			int w = u - v; // run all partitions v + w = u
			for (node_t *x = solutions[v - 1]; x; x = x->next)
				for (node_t *y = solutions[w - 1]; y; y = y->next) {
					node_t **target_list = solutions + u - 1;
					if (check_op(&tree_set, target_list, x->i + y->i))
						goto end;
					int sub = x->i - y->i;
					if (sub < 0)
						sub = -sub;
					if (check_op(&tree_set, target_list, sub))
						goto end;
					if (check_op(&tree_set, target_list, x->i * y->i))
						goto end;
					if (y->i && !(x->i % y->i) &&
						check_op(&tree_set, target_list, x->i / y->i))
							goto end;
					if (x->i && !(y->i % x->i) &&
						check_op(&tree_set, target_list, y->i / x->i))
							goto end;
				}
		}
end:
	clear_tree_set(tree_set);

	if (u > LIMIT) {
		printf("Solution not found\n");
		return 1;
	}
	printf("%d\n", u);
	return 0;
}

bool check_op(node_t **pnode, node_t **plist, int i)
{
	node_t *node;

	if (i < top && (node = check_n_insert(pnode, i))) {
		node->next = *plist;
		*plist = node;
	}
	return i == N;
}

node_t *check_n_insert(node_t **pnode, int i)
{
	node_t *node = *pnode;

	if (!node) {
		node = malloc(sizeof(node_t));
		node->left = node->right = NULL;
		node->i = i;
		*pnode = node;
		return node;
	}
	else if (i < node->i)
		return check_n_insert(&node->left, i);
	else if (i > node->i)
		return check_n_insert(&node->right, i);
	else
		return NULL;
}

void clear_tree_set(node_t *node)
{
	if (node) {
		clear_tree_set(node->left);
		clear_tree_set(node->right);
		free(node);
	}
}
