#pragma once
#include "putty.h"
#include <commctrl.h>
#include "jp_defs.h"


#define SCROLL_DELAY_TIMER WM_USER+100

#define JP_MAX_SESSIONS 1024 * 1
#define JP_LISTBOX_ID_UNDEFINED -1
#define JP_SESSIONID_UNDEFINED -1
#define JP_INSTANCE_UNDEFINED -1

struct s_jp_selection_info;

typedef struct s_jp_selection_info  {
    char* session_filter;
    int session_id;
    char* session_name;
    int listbox_id;
    int listbox_top_id;
    short instance_id;
    HANDLE mutex;
    bool tree_hidden;
    bool loading_settings;
} jp_selection_info;

jp_selection_info* jp_get_selection_info();
void jp_load_instance_parameters(Conf* conf);
char* jp_generate_session_name_on_copy(const char* input, const char* pattern);
bool jp_save_config_if_not_loading(Conf* conf);

typedef struct TreeNode {
    char text[100];
    void* data;
    BOOL isExpanded;
    struct TreeNode** children;
    int childCount;
} TreeNode;

TreeNode* jp_store_tree_items(HWND treeview);
void jp_hide_tree_view_items_ex_session();
void jp_restore_hidden_tree_items();
bool jp_is_tree_hidden();

void jp_test_func();