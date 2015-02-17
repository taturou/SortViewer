#pragma once
#include <sort.h>

/* for sort_set_algorithm() */
extern SortAlgorithm sort_algorithm_babble;

/* for sort_init() */
extern int sort_algorithm_babble_init_param_descorder;
extern int sort_algorithm_babble_init_param_random;
#define SORT_ALGORITHM_BABBLE_INIT_PARAM_DESCORDER    ((void*)&sort_algorithm_babble_init_param_descorder)
#define SORT_ALGORITHM_BABBLE_INIT_PARAM_RANDOM       ((void*)&sort_algorithm_babble_init_param_random)
