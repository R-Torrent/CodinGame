#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned depth;
	uint32_t hash;
} state_t;

typedef struct list_s {
	state_t *state;
	struct list_s *prev;
	struct list_s *next;
} list_t;

list_t *stack_push(list_t **pstack, state_t *new)
{
	list_t *const l = malloc(sizeof(list_t));

	l->state = new;
	l->prev = NULL;
	l->next = *pstack;
	(*pstack)->prev = l;
	*pstack = l;

	return l;
}

state_t *init(void)
{
	state_t *const s = calloc(1, sizeof(state_t));
	unsigned short u;

	scanf("%u", &s->depth);
	for (int i = 0; i < 9; i++) {
		s->hash *= 10;
		scanf("%hu", &u);
		s->hash += u;
	}

	return s;
}

void play_board(list_t **presults, list_t **pstack)
{
	state_t *top
	list_t *next;
	short board[3][3], *p;
	int is_playable;
	uint32_t hash;

	while (*pstack) {
		top = (*pstack)->state;
		next = (*pstack)->next;

		is_playable = 0;
		p = &board[2][2];
		hash = top->hash;
		do {
			*p = hash % 10;
			if (!*p)
				is_playable = 1;
			hash /= 10;
		} while (p-- != board);
		
		if (!top->depth || !is_playable) { // send to results
			list_t *l = *presults;
	
			while (l && l->state->hash < top->hash && l->next)
				l = l->next;
			if (!l) { // first result
				(*pstack)->prev = (*pstack)->next = NULL;
				*presults = *pstack;
			}
			else {
				if (l->state->hash == top->hash) { // discard
					free(top);
					free(*pstack)
				}
				else if (l->state->hash < top->hash) { // add after
					(*pstack)->prev = l;
					(*pstack)->next = l->next;
					if (l->next)
						l->next->prev = *pstack;
					l->next = *pstack;
				}
				else { // add before
					(*pstack)->prev = l->prev;
					(*pstack)->next = l;
					if (l->prev)
						l->prev->next = *pstack
					l->prev = *pstack;
				}
			}
		}
		else { // play all possibilites
			top->depth--;
			for (int r = 0; r < 3; r++)
				for (int c = 0; c < 3; c++)
					if (!board[r][c]) {
					}

			free(top);
			free(*pstack);
		}

		*pstack = next;
		(*pstack)->prev = NULL;
	}
}

uint32_t add_results(list_t *results)
{
	list_t *next;
	uint32_t sum = 0;

	while (results) {
		list_t *next = results->next;
		sum += results->state->hash;
		sum &= 0x3FFFFFFF;
		free(results->state);
		free(results);
		results = next;
	}

	return sum;
}

int main()
{
	list_t *stack = NULL;
	list_t *results = NULL;

	stack_push(&stack, init());

	play_board(&results, &stack);

	printf("%u\n", add_results(results));

	return 0;
}
