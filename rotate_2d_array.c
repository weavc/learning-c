#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int size = 25;

typedef struct {
    short ** arr;
    int size;    
} arr_2d;

static arr_2d * new_set(int s) {
    short ** arr = (short **) malloc(s * sizeof(short *));
    for (int i = 0; i < s; i++) {
        arr[i] = (short *) malloc(s * sizeof(short));
        for (int j = 0; j < s; j++) {
            arr[i][j] = '0';
        }
    }
    arr_2d * r = malloc(sizeof(arr_2d *));
    r->arr = arr;
    r->size = s;

    return r;
}

static void dispose_2d_arr(short ** arr, int size) {
    for (int i = 0; i < size; i++) {
        free(arr[i]);
    }
    free(arr);
}

static void dispose_set(arr_2d *set) {
    dispose_2d_arr(set->arr, set->size);
    free(set);
}

static void render_set(arr_2d *set) {
    printf("===========\n");
    for (int i = 0; i < set->size; i++) {
        printf("%d:\t", i);
        for (int j = 0; j < set->size; j++) {
            printf("%d ", set->arr[i][j]);
        }
        printf("\n");
    }
    printf("===========\n");
}

static short * get_between(arr_2d *set, int c1[2], int c2[2], int size) {
    short * curr = (short *) malloc(size * sizeof(short));
    int x = 1;
    int from = c1[0];
    int to = c2[0];
    if (from == to) {
        x = 0;
        from = c1[1];
        to = c2[1];
    }

    for (int p = 0; p < size; p++) {
        if (x == 0) {
            curr[p] = set->arr[from][c1[0]];
        } else {
            curr[p] = set->arr[c1[1]][from];
        }

        if (to < from) {
            from--;
        } else {
            from++;
        }
    }

    return curr;
}

static void set_between(arr_2d *set, int c1[2], int c2[2], int size, short * vals) {
    int x = 1;
    int from = c1[0];
    int to = c2[0];
    if (from == to) {
        x = 0;
        from = c1[1];
        to = c2[1];
    }

    for (int p = 0; p < size; p++) {
        if (x == 0) {
            set->arr[from][c1[0]] = vals[p];
        } else {
            set->arr[c1[1]][from] = vals[p];
        }

        if (to < from) {
            from--;
        } else {
            from++;
        }
    }
}

static void rot_array(arr_2d *set, int size, int depth) {
    int m = size-1;
    int zero = depth;

    // {x, y}
    int c[][2] = {{zero,zero}, {m+depth, zero}, {m+depth, m+depth}, {zero, m+depth}};
    short ** rset = (short **) malloc(4 * sizeof(short *));

    for (int i = 0; i < 4; i++) {
        rset[i] = get_between(set, c[i], c[(i+1)%4], size);
    }

    for (int i = 0; i < 4; i++) {
        set_between(set, c[(i+1)%4], c[(i+2)%4], size, rset[i]);
    }

    dispose_2d_arr(rset, 4);

    if (m-1 > 1) {
        rot_array(set, m-1, depth+1);
    }
}


static inline int get_start(int max) {
    int p = 1;
    while (pow(10, p) < max) {
        p++;
    }
    return pow(10, p);
}

static void set_seq_array(arr_2d *set) {
    int start = get_start(pow(set->size, 2));

    for (int i = 0; i < set->size; i++) {
        for (int j = 0; j < set->size; j++) {
            set->arr[i][j] = start + j + (i*set->size);
        }
    }
}

static void set_number_array(arr_2d *set, short val) {
    int start = get_start(pow(set->size, 2));

    for (int i = 0; i < set->size; i++) {
        for (int j = 0; j < set->size; j++) {
            set->arr[i][j] = val;
        }
    }
}

void rot_set(arr_2d *set) {
    rot_array(set, set->size, 0);
}

int main() {
    arr_2d *set = new_set(size);
    set_seq_array(set);

    for (int rots = 0; rots < 4; rots++) {
        render_set(set);
        rot_set(set);
    }
    render_set(set);
    dispose_set(set);
}