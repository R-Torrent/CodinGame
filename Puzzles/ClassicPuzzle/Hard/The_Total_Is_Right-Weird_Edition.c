#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Solved using Reversed Polish Notation, to avoid the use of parentheses.
 *
 * Candidate arithmetic expressions are codified into a char array with format
 *   a a (a o a o ...) o
 * where 'a' stands for "push(a)" and 'o' any of the four arithmetic operations,
 * "push(pop() op pop())". After the expression is evaluated, a single integer
 * will be stored in the stack's head.
 *
 * Each expression is recursively explored for each of the four arithmetic
 * operators
 */

#define LIMIT 12 // game's limit on the number of operations
#define DEPTH (LIMIT + 1) // maximum depth of stack

typedef struct {
	int sp; // next free stack position
	int stack[DEPTH];
} stack_t;

bool is_valid(char *, int);
bool evaluates_to_N(stack_t *, int, int, char *, int);
void duplicate_stack(stack_t *, stack_t *);
void push(stack_t *, int);
int pop(stack_t *);

int compare(const void *, const void *);

int main()
{
	int N, a;
	scanf("%d%d", &N, &a);

	char expr[2 * LIMIT + 1];
	int sol;
	bool found = false;
	for (sol = 1; sol <= LIMIT && !found; sol++) {
		int size = 2 * sol + 1;
		expr[0] = expr[1] = 1; // 1 == 'a' in the above explanation
		expr[size - 1] = 0;    // 0 == 'o' in the above explanation
		// starting expr = a a (a a a ... a o ... o o o) o
		for (int i = 2; i < size - 1; i++)
			expr[i] = i <= sol;

		while (true) {
			stack_t st = { .sp = 0 };
			if (is_valid(expr, size) && evaluates_to_N(&st, N, a, expr, size)) {
				found = true;
				break;
			}
			// permutations with duplications for the central ( a o ... ) portion
			// find rightmost one with a zero to its right
			int i;
			for (i = size - 3; i >= 2; i--)
				if (expr[i] && !expr[i + 1])
					break;
			if (i < 2) // none found
				break;
			else {
				expr[i] = 0; // swap elements
				expr[i + 1] = 1;
				// sort in descending order
				qsort(expr + i + 1, size - i - 2, 1, compare);
			}
		}
	}
	if (!found) {
		printf("Solution not found\n");
		return 1;
	}
	printf("%d\n", sol);
	return 0;
}

int compare(const void *a, const void *b)
{
	return *(char *)b - *(char *)a;
}

bool is_valid(char *expr, int size)
{
	int a = 0;

	while (size--)
		if (*expr++)
			a++;
		else if (!--a)
			return false;
	return true;
}

bool evaluates_to_N(stack_t *st, int N, int a, char *expr, int size)
{
	if (!size--)
		return pop(st) == N;
	if (*expr++) { // push value 'a'
		push(st, a);
		return evaluates_to_N(st, N, a, expr, size);
	}
	int op2 = pop(st), op1 = pop(st);
	stack_t st_copy;
	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 + op2); // add
	if (evaluates_to_N(&st_copy, N, a, expr, size))
		return true;
	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 - op2); // subtract
	if (evaluates_to_N(&st_copy, N, a, expr, size))
		return true;
	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 * op2); // multiply
	if (evaluates_to_N(&st_copy, N, a, expr, size))
		return true;
	if (op2 && !(op1 % op2)) {
		duplicate_stack(&st_copy, st);
		push(&st_copy, op1 / op2); // divide
		if (evaluates_to_N(&st_copy, N, a, expr, size))
			return true;
	}
	return false;
}

void duplicate_stack(stack_t *dst, stack_t *src)
{
	dst->sp = src->sp;
	for (int i = 0; i < DEPTH; i++)
		dst->stack[i] = src->stack[i];
}

void push(stack_t *st, int i)
{
	if (st->sp < DEPTH)
		st->stack[st->sp++] = i;
	else
		exit(2);
}

int pop(stack_t *st)
{
	if (st->sp > 0)
		return st->stack[--st->sp];
	else
		exit(2);
}
