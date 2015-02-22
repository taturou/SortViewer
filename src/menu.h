#pragma once
#include <sort.h>

typedef struct menu Menu;

typedef enum algorithm_kind {
    MSA_Babble = 0,
    MSA_Merge,
    MSA_Quick,
    MSA_Bucket
    // You have to modify 'MAX_ALGORITHM_KIND' value.
} AlgorithmKind;
#define MAX_ALGORITHM_KIND  ((int)MSA_Bucket + 1)

typedef void (*MenuSelectCallback)(AlgorithmKind kind);

Menu *menu_create(void);
void menu_destroy(Menu *menu);
bool menu_show(Menu *menu, MenuSelectCallback callback);
bool menu_hide(Menu *menu);