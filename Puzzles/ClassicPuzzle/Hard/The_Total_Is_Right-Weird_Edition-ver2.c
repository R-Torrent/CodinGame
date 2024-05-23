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
	struct Node *left, *right;
	int i;
} node_t;

bool check_n_insert(node_t **, int);
void clear_tree_set(node_t *);

typedef struct List {
	struct List *next;
	int i;
} list_t;

void add_list(list_t **, int);
void clear_list(list_t *);

bool check_op(node_t **, list_t **, int);

int N, a, top;

int main()
{
	scanf("%d%d", &N, &a);
	top = N * a * a * a;

	node_t *tree_set = NULL;
	list_t *solutions[LIMIT] = { NULL };

	check_n_insert(&tree_set, a);
	add_list(&solutions[0], a);

	int i = 1;
	if (a == N)
		goto end;
	for (++i; i <= LIMIT; i++)
		for (int j = 1; j <= i / 2; j++) {
			int k = i - j;
			for (list_t *x = solutions[j - 1]; x; x = x->next)
				for (list_t *y = solutions[k - 1]; y; y = y->next) {
					list_t **target = solutions + i - 1;
					if (check_op(&tree_set, target, x->i + y->i))
						goto end;
					int sub = x->i - y->i;
					if (sub < 0)
						sub = -sub;
					if (check_op(&tree_set, target, sub))
						goto end;
					if (check_op(&tree_set, target, x->i * y->i))
						goto end;
					if (y->i && !(x->i % y->i) &&
						check_op(&tree_set, target, x->i / y->i))
							goto end;
					if (x->i && !(y->i % x->i) &&
						check_op(&tree_set, target, y->i / x->i))
							goto end;
				}
		}
end:
	clear_tree_set(tree_set);
	for (int i = 0; i < LIMIT; i++)
		clear_list(solutions[i]);

	if (i > LIMIT) {
		printf("Solution not found\n");
		return 1;
	}
	printf("%d\n", i);
	return 0;
}

bool check_op(node_t **pnode, list_t **plist, int i)
{
	if (i < top && check_n_insert(pnode, i))
		add_list(plist, i);
	return i == N;
}

bool check_n_insert(node_t **pnode, int i)
{
	node_t *node = *pnode;

	if (!node) {
		node = malloc(sizeof(node_t));
		node->left = node->right = NULL;
		node->i = i;
		*pnode = node;
		return true;
	}
	else if (i < node->i)
		return check_n_insert(&node->left, i);
	else if (i > node->i)
		return check_n_insert(&node->right, i);
	else
		return false;
}

void clear_tree_set(node_t *node)
{
	if (node) {
		clear_tree_set(node->left);
		clear_tree_set(node->right);
		free(node);
	}
}

void add_list(list_t **plink, int i)
{
	list_t *link;

	link = malloc(sizeof(list_t));
	link->next = *plink;
	link->i = i;
	*plink = link;
}

void clear_list(list_t *link)
{
	if (link) {
		clear_list(link->next);
		free(link);
	}
}
