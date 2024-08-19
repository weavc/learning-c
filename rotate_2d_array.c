#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DIRECTION_X 1
#define DIRECTION_Y 2


// static variables are scoped to this file, even if defined inside a function
static int size = 5;

typedef struct {
    short ** arr;
    int size;    
} arr_2d;

static arr_2d * new_set(int s) {
    // dynamic assignment of short[]
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

// const is used in this case to show the value cannot be changed
static void render_set(const arr_2d *set) {
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

typedef struct {
    int step;
    int counter;
    int from;
    int to;
} walk_counter;

typedef void (*walk_func)(const arr_2d *set, short direction, int index, walk_counter * x_counter, walk_counter * y_counter, const void * args);

static void walk_between(const arr_2d *set, int corner1[2], int corner2[2], int size, walk_func func, const void * args) {
    short direction = 0;
    walk_counter x_counter;
    x_counter.step = 1;
    x_counter.counter = corner1[0];
    x_counter.from = corner1[0];
    x_counter.to = corner2[0];
    walk_counter y_counter;
    y_counter.step = 1;
    y_counter.counter = corner1[1];
    y_counter.from = corner1[1];
    y_counter.to = corner2[1];

    if (corner1[0] != corner2[0]) {
        direction = direction | DIRECTION_X;
    }

    if (corner2[0] < corner1[0]) {
        x_counter.step = -1;
    }

    if (corner1[1] != corner2[1]) {
        direction = direction | DIRECTION_Y;
    }

    if (corner2[1] < corner1[1]) {
        y_counter.step = -1;
    }

    walk_counter * xc = &x_counter;
    walk_counter * yc = &y_counter;
    
    for (int p = 0; p < size; p++) {
        yc->counter = yc->from + (yc->step*p);
        xc->counter = xc->from + (xc->step*p);
        func(set, direction, p, xc, yc, args);
    }
}

static void set_walk_func(const arr_2d *set, short direction, int index, walk_counter * x_counter, walk_counter * y_counter, const void * args) {
    const short * vals = (short *) args;

    if (direction & DIRECTION_Y) {
        set->arr[y_counter->counter][x_counter->to] = vals[index];
    } 
    
    if (direction & DIRECTION_X) {
        set->arr[y_counter->to][x_counter->counter] = vals[index];
    }
}

static void get_walk_func(const arr_2d *set, short direction, int index, walk_counter * x_counter, walk_counter * y_counter, const void * args) {
    short * curr = (short *) args;
    if (direction & DIRECTION_Y) {
        curr[index] = set->arr[y_counter->counter][x_counter->to];
    } 
    
    if (direction & DIRECTION_X) {
        curr[index] = set->arr[y_counter->to][x_counter->counter];
    }
}

static void rot_array(const arr_2d *set, int size, int depth) {
    int m = size-1;
    int zero = depth;

    // {x, y}
    int c[][2] = {{zero,zero}, {m+depth, zero}, {m+depth, m+depth}, {zero, m+depth}};
    short ** rset = (short **) malloc(4 * sizeof(short *));

    for (int i = 0; i < 4; i++) {
        rset[i] = (short *) malloc(size * sizeof(short));
        walk_between(set, c[i], c[(i+1)%4], size, get_walk_func, rset[i]);
    }

    for (int i = 0; i < 4; i++) {
        walk_between(set, c[(i+1)%4], c[(i+2)%4], size, set_walk_func, rset[i]);
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

static void set_seq_array(const arr_2d *set) {
    int start = get_start(pow(set->size, 2));

    for (int i = 0; i < set->size; i++) {
        for (int j = 0; j < set->size; j++) {
            set->arr[i][j] = start + j + (i*set->size);
        }
    }
}

static void set_number_array(const arr_2d *set, short val) {
    int start = get_start(pow(set->size, 2));

    for (int i = 0; i < set->size; i++) {
        for (int j = 0; j < set->size; j++) {
            set->arr[i][j] = val;
        }
    }
}

void rot_set(const arr_2d *set) {
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