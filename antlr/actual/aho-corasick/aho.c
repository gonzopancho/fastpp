#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include "ds_queue.h"

/**< Maximum number of states, at least as large as the sum of all keypattern's length. */
#define MAX (505 * 505)

/**< Alphabet size, 26 for lower case English letters. */
#define ALPHA_SIZE 27
#define FAIL -1

int g[MAX][ALPHA_SIZE];
int f[MAX];
struct ds_queue output[MAX];
int new_state;

void enter(char *pattern, int index)
{
	int length = strlen(pattern);
	int j, state = 0;
	for(j = 0; j < length; j ++) {
		int c = pattern[j] - 'a';
		if(g[state][c] == FAIL) {
			break;
		}
		state = g[state][c];
	}

	/**< Characters j to (length - 1) need new states */
	for(; j < length; j ++) {
		int c = pattern[j] - 'a';
		new_state ++;
		g[state][c] = new_state;
		state = new_state;
	}

	/**< Add this pattern as the output for the last state */
	assert(state >= 0 && state < MAX);
	ds_queue_add(&output[state], index);
}

int main(int argc, char *argv[])
{
	int n, i;
	char c;
	char *text, *pattern;
	int *count;

	scanf("%d", &n);
	scanf("%ms", &text);
	new_state = 0;

	/**< Initialize the goto and failure functions */
	memset(g, FAIL, MAX * ALPHA_SIZE * sizeof(int));
	memset(f, FAIL, MAX * sizeof(int));

	/**< Initialize the pattern-occurence count */
	count = malloc(n * sizeof(int));
	memset(count, 0, n * sizeof(int));

	/**< Initialize output queues for each state */
	for(i = 0; i < MAX; i++) {
		ds_queue_init(&output[i]);
	}

	/**< Read patterns and build the Trie */
	for(i = 0; i < n; i++) {
		scanf("%ms", &pattern);
		count[i] = 0;
		enter(pattern, i);
	}

	/**< Invalid transitions from the root state need to loop back */
	for(c = 'a'; c <= 'z'; c++) {
		int a = c - 'a';
		if(g[0][a] == FAIL) {
			g[0][a] = 0;
		}
	}

	/**< Build failure function: add root node's children to the queue */
	struct ds_queue Q;
	ds_queue_init(&Q);
	for(c = 'a'; c <= 'z'; c++) {
		int a = c - 'a';
		int s = g[0][a];
		if(s != 0) {
			ds_queue_add(&Q, s);
			f[s] = 0;
		}
	}

	while(!ds_queue_is_empty(&Q)) {
		int r = ds_queue_remove(&Q);

		/**< Look at all the valid state transitions from r */
		for(c = 'a'; c <= 'z'; c++) {
			int a = c - 'a';
			int s = g[r][a];

			if(s != FAIL) {
				ds_queue_add(&Q, s);
				int state = f[r];
				while(g[state][a] == FAIL) {
					state = f[state];
				}

				f[s] = g[state][a];

				/**< Add all patterns from output[f[s]] to output[s] */
				struct ds_qnode *t = output[f[s]].head;
				while(t != NULL) {
					ds_queue_add(&output[s], t->data);
					t = t->next;
				}
			}
		}
	}

	/**< Count occurrences of patterns inside text */
	int state = 0;
	int length = strlen(text);
	for(i = 0; i < length; i++) {
		int a = text[i] - 'a';
		while(g[state][a] == FAIL) {
			state = f[state];
		}
		state = g[state][a];

		/**< Increase the count for all patterns matched at this state */
		struct ds_qnode *t = output[state].head;
		while(t != NULL) {
			assert(t->data < 2005);
			count[t->data] ++;
			t = t->next;
		}
	}

	for(i = 0; i < n; i++) {
		printf("%d: ", count[i]);
		int expected;
		scanf("%d\n", &expected);
		if(count[i] == expected) {
			printf("Passed\n");
		} else {
			printf("Failed\n");
			exit(-1);
		}
	}

	free(pattern);
	free(text);
	free(count);

	return 0;
}
