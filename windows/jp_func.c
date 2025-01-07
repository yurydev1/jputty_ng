#include "jp_func.h"
#include <math.h>
#include "storage.h"
#include "win-gui-seat.h"

//############################################################################################################
const char* get_filename(const char* path) {
    const char* filename = strrchr(path, '/');
    if (!filename) {
        filename = strrchr(path, '\\'); // Handle Windows paths
    }
    return filename ? filename + 1 : path;
}

//############################################################################################################
unsigned int _jp_filtered_sessions[JP_MAX_SESSIONS];
int _jp_last_listbox_id = JP_LISTBOX_ID_UNDEFINED;

void jp_reset_filtered_sessions() {
    _jp_last_listbox_id = JP_LISTBOX_ID_UNDEFINED;
}

void jp_add_filtered_session(int session_id) {
    _jp_filtered_sessions[++_jp_last_listbox_id] = session_id;
}

int jp_get_filtered_session(int listbox_id) {
    if (listbox_id > _jp_last_listbox_id)
        return JP_SESSIONID_UNDEFINED;
    return _jp_filtered_sessions[listbox_id];
}

int jp_get_session_listbox_id(session_id) {
    for (int i = 0; i <= _jp_last_listbox_id; i++)
        if (_jp_filtered_sessions[i] == session_id)
            return i;
    return JP_LISTBOX_ID_UNDEFINED;
}

int jp_get_session_id_via_name(const struct sesslist* sessions_list, const char* session_name) {
    for (int session_id = 0; session_id < sessions_list->nsessions; session_id++) {
        if (strcmp(sessions_list->sessions[session_id], session_name) == 0)
            return session_id;
    }
    return JP_SESSIONID_UNDEFINED;
}

//############################################################################################################
static jp_selection_info* _jp_selection_info=NULL;

void _jp_init_selection_info() {
    if (_jp_selection_info == NULL) {
        _jp_selection_info = malloc(sizeof(jp_selection_info));
        _jp_selection_info->session_filter = NULL;
        _jp_selection_info->session_id = JP_SESSIONID_UNDEFINED;
        _jp_selection_info->session_name = NULL;
        _jp_selection_info->listbox_id = JP_LISTBOX_ID_UNDEFINED;
        _jp_selection_info->listbox_top_id = JP_LISTBOX_ID_UNDEFINED;
        _jp_selection_info->mutex = NULL;
        _jp_selection_info->instance_id = JP_INSTANCE_UNDEFINED;
        _jp_selection_info->tree_hidden = false;
        _jp_selection_info->loading_settings = false;
    }
}

jp_selection_info* jp_get_selection_info() {
    _jp_init_selection_info();
    return _jp_selection_info;
}

bool jp_is_tree_hidden() {
    return jp_get_selection_info()->tree_hidden;
}

void jp_set_session_filter(const char* filter) {
    jp_selection_info* selection_info = jp_get_selection_info();
    if (selection_info->session_filter != NULL) {
        burnstr(selection_info->session_filter);
        selection_info->session_filter = NULL;
    }
    if (filter != NULL)
        selection_info->session_filter = dupstr(filter);
}

jp_selection_info* jp_set_selection_session_name(const char* session_name) {
    jp_selection_info* selection_info = jp_get_selection_info();
    if (selection_info->session_name != NULL) {
        burnstr(selection_info->session_name);
        selection_info->session_name = NULL;
    }
    if (session_name != NULL)
        selection_info->session_name = dupstr(session_name);
    return selection_info;
}

void jp_set_selection_info(const int session_id, const char* session_name, const int listbox_id, const int listbox_top_id) {
    jp_selection_info* selection_info = jp_set_selection_session_name(session_name);
    selection_info->session_id = session_id;
    selection_info->listbox_id = listbox_id;
    selection_info->listbox_top_id = listbox_top_id;
}

#define _jp_session_instance_meaningful_digits	 2
#define _jp_session_instance_prefix_mutex_tag "::"

jp_selection_info* jp_update_selection_info_w_instance_info() {
    jp_selection_info* selection_info = jp_get_selection_info();
    selection_info->instance_id = 0;
    if (selection_info->session_name != NULL) {
        char* session_instance_mutex_name = NULL;
        char buffer[_jp_session_instance_meaningful_digits + 1];
        while (1) {
            DWORD lastErr;
            itoa(selection_info->instance_id, buffer, 10);
            burnstr(session_instance_mutex_name);
            session_instance_mutex_name = dupcat(selection_info->session_name, _jp_session_instance_prefix_mutex_tag, buffer, NULL);
            selection_info->mutex = CreateMutex(NULL, FALSE, session_instance_mutex_name);
            lastErr = GetLastError();
            if (lastErr == ERROR_ALREADY_EXISTS) {
                if (selection_info->mutex != NULL)
                    CloseHandle(selection_info->mutex);
                selection_info->instance_id++;
                continue;
            }
            break;
        }
        burnstr(session_instance_mutex_name);
    }
    return selection_info;
}

void jp_release_sessionex_instance() {
    jp_selection_info* selection_info = jp_get_selection_info();
    if (selection_info->mutex != NULL) {
        CloseHandle(selection_info->mutex);
        selection_info->mutex = NULL;
    }
}

void jp_load_instance_parameters(Conf* conf) {
    jp_selection_info* selection_info = jp_update_selection_info_w_instance_info();
    void* sesskey = open_settings_r(selection_info->session_name);
    if (sesskey) {
        //char* buffer = NULL;
        //buffer = read_setting_s(sesskey, "WinTitle");
        //conf_set_str(conf, CONF_wintitle, buffer);
        conf_set_int(conf, CONF_width, read_setting_i_si(sesskey, selection_info->instance_id, "TermWidth", 80));
        conf_set_int(conf, CONF_height, read_setting_i_si(sesskey, selection_info->instance_id, "TermHeight", 24));
        conf_set_int(conf, CONF_windowpos_top, read_setting_i_si(sesskey, selection_info->instance_id, "SaveWindowTop", CW_USEDEFAULT));
        conf_set_int(conf, CONF_windowpos_left, read_setting_i_si(sesskey, selection_info->instance_id, "SaveWindowLeft", CW_USEDEFAULT));
        FontSpec* font = NULL; char* buffer = NULL;
        font = read_setting_fontspec_si(sesskey, selection_info->instance_id, "Font");
        conf_set_fontspec(conf, CONF_font, font);
        close_settings_r(sesskey);
        sfree(buffer); buffer = NULL;
        fontspec_free(font); font = NULL;
    }
}

void reset_window(WinGuiSeat* wgs, int reinit);
void deinit_fonts(WinGuiSeat* wgs);
void init_fonts(WinGuiSeat* wgs, int, int);

void jp_change_font_size(WinGuiSeat* wgs, HWND hwnd, char incr) {
    jp_selection_info* selection_info = jp_get_selection_info();
    settings_r* settings = open_settings_r(selection_info->session_name);
    if (settings != NULL) {
        FontSpec* font = read_setting_fontspec_si(settings, selection_info->instance_id, "Font");
        font->height += incr;
        write_setting_fontspec_si(settings, selection_info->instance_id, "Font", conf_get_fontspec(wgs->conf, CONF_font));
        conf_set_fontspec(wgs->conf, CONF_font, font);

        unsigned int pre_term_width = conf_get_int(wgs->conf, CONF_width);
        unsigned int pre_term_height = conf_get_int(wgs->conf, CONF_height);
        unsigned int pre_width = wgs->extra_width + wgs->font_width * pre_term_width;
        unsigned int pre_height = wgs->extra_height + wgs->font_height * pre_term_height;
        deinit_fonts(wgs);
        init_fonts(wgs, 0, 0);

        unsigned int post_width = wgs->extra_width + wgs->font_width * pre_term_width;
        unsigned int post_height = wgs->extra_height + wgs->font_height * pre_term_height;
        unsigned int post_term_width = (unsigned int)round((double)(pre_term_width * pre_width) / post_width);
        unsigned int post_term_height = (unsigned int)round((double)(pre_term_height * pre_height) / post_height);
        conf_set_int(wgs->conf, CONF_width, post_term_width);
        conf_set_int(wgs->conf, CONF_height, post_term_height);
        write_setting_i_si(settings, selection_info->instance_id, "TermWidth", post_term_width);
        write_setting_i_si(settings, selection_info->instance_id, "TermHeight", post_term_height);
        close_settings_r(settings);
        term_size(wgs->term, post_term_height, post_term_width, conf_get_int(wgs->conf, CONF_savelines));
        InvalidateRect(hwnd, NULL, TRUE);
        reset_window(wgs, 2);
    }
}

TreeNode* _jp_root_node = NULL;
HWND _jp_tree_view = NULL;

TreeNode* _jp_store_tree_items(HWND treeview, HTREEITEM hItem);
void _jp_hide_tree_view_items_ex_session(HTREEITEM hItem);
void _jp_restore_hidden_tree_items(TreeNode* node, HTREEITEM hParent);

TreeNode* jp_store_tree_items(HWND hTreeView) {
    return _jp_store_tree_items(hTreeView, NULL);
}

TreeNode* _jp_store_tree_items(HWND hTreeView, HTREEITEM hItem) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node)
        return NULL;
    node->children = NULL;
    node->childCount = 0;
    node->data = NULL;
    node->isExpanded = FALSE;

    if (!hItem) {
        node->text[0] = 0;
        hItem = TreeView_GetRoot(hTreeView);
        _jp_root_node = node;
        _jp_tree_view = hTreeView;
    }
    else {
        TVITEM tvi = { 0 };
        tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvi.hItem = hItem;
        tvi.pszText = node->text;
        tvi.cchTextMax = sizeof(node->text);
        tvi.stateMask = TVIS_EXPANDED;
        TreeView_GetItem(hTreeView, &tvi);
        node->data = (void*)tvi.lParam; // Store node-specific data
        node->isExpanded = (tvi.state & TVIS_EXPANDED) != 0; // Store expanded state

        hItem = TreeView_GetChild(hTreeView, hItem);
    }
    while (hItem) {
        node->childCount++;
        node->children = (TreeNode**)realloc(node->children, node->childCount * sizeof(TreeNode*));
        node->children[node->childCount - 1] = _jp_store_tree_items(hTreeView, hItem);
        hItem = TreeView_GetNextSibling(hTreeView, hItem);
    }

    return node;
}

void jp_hide_tree_view_items_ex_session() {
    _jp_hide_tree_view_items_ex_session(NULL);
    _jp_selection_info->tree_hidden = true;
}

void _jp_hide_tree_view_items_ex_session(HTREEITEM hItem) {
    if (!_jp_tree_view)
        return;
    if (!hItem)
        hItem = TreeView_GetRoot(_jp_tree_view);

    HTREEITEM hCurrentItem = hItem;
    while (hCurrentItem) {
        TVITEM tvi = { 0 };
        tvi.mask = TVIF_TEXT;
        tvi.hItem = hCurrentItem;
        char text[100];
        tvi.pszText = text;
        tvi.cchTextMax = sizeof(text);
        TreeView_GetItem(_jp_tree_view, &tvi);
        HTREEITEM hChild = TreeView_GetChild(_jp_tree_view, hCurrentItem);
        if (strcmp(text, "Session") != 0) {
            if (hChild) {
                _jp_hide_tree_view_items_ex_session(hChild); // Recursively hide children
            }
            HTREEITEM hNextItem = TreeView_GetNextSibling(_jp_tree_view, hCurrentItem);
            TreeView_DeleteItem(_jp_tree_view, hCurrentItem);
            hCurrentItem = hNextItem;
        }
        else {
            // Hide children of "Session"
            while (hChild) {
                HTREEITEM hNextChild = TreeView_GetNextSibling(_jp_tree_view, hChild);
                TreeView_DeleteItem(_jp_tree_view, hChild);
                hChild = hNextChild;
            }
            hCurrentItem = TreeView_GetNextSibling(_jp_tree_view, hCurrentItem);
        }
    }
}

void jp_restore_hidden_tree_items() {
    _jp_restore_hidden_tree_items(_jp_root_node, TVI_ROOT);
    _jp_selection_info->tree_hidden = false;
}

void _jp_restore_hidden_tree_items(TreeNode* node, HTREEITEM hParent) {
    if (!_jp_tree_view)
        return;
    if (!_jp_root_node)
        return;
    if (strlen(node->text) > 0) {
        TVINSERTSTRUCT tvis = { 0 };
        tvis.hParent = hParent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
        tvis.item.pszText = node->text;
        tvis.item.lParam = (LPARAM)node->data;

        HTREEITEM hItem = TreeView_InsertItem(_jp_tree_view, &tvis);
        if (!hItem)
            return;
        if (strcmp(node->text, "Session") == 0) {
            TreeView_SelectItem(_jp_tree_view, hItem);
        }

        for (int i = 0; i < node->childCount; i++)
            _jp_restore_hidden_tree_items(node->children[i], hItem);

        if (node->isExpanded)
            TreeView_Expand(_jp_tree_view, hItem, TVE_EXPAND);
        else
            TreeView_Expand(_jp_tree_view, hItem, TVE_COLLAPSE);
    }
    else {
        TreeView_DeleteAllItems(_jp_tree_view);
        for (int i = 0; i < node->childCount; i++)
            _jp_restore_hidden_tree_items(node->children[i], hParent);
    }

    //free(node->children);
    //free(node);
}

char* jp_generate_session_name_on_copy(const char* input, const char* pattern) {
    char* buffer = strdup(input);
    if (!buffer) return NULL;

    char* lastHash = NULL;
    char* p = strstr(buffer, pattern);
    while (p) {
        lastHash = p;
        p = strstr(p + 1, pattern);
    }

    if (lastHash) {
        char* numberStart = lastHash + 2;
        char* endPtr;
        long number = strtol(numberStart, &endPtr, 10);
        if (endPtr != numberStart && *endPtr == '\0') {
            number++;
            size_t newLength = (lastHash - buffer) + snprintf(NULL, 0, " #%ld", number) + 1;
            char* result = (char*)malloc(newLength);
            if (!result) {
                free(buffer);
                return NULL;
            }
            snprintf(result, newLength, "%.*s #%ld", (int)(lastHash - buffer), buffer, number);
            free(buffer);
            return result;
        }
    }
    size_t newLength = strlen(buffer) + 4; // 4 for " #1" and null terminator
    char* result = (char*)malloc(newLength);
    if (!result) {
        free(buffer);
        return NULL;
    }
    snprintf(result, newLength, "%s #1", buffer);
    free(buffer);
    return result;
}

bool jp_save_config_if_not_loading(Conf* conf) {
    const jp_selection_info* selection_info = jp_get_selection_info();
    if (selection_info->session_name != NULL && selection_info->loading_settings == false) {
        save_settings(jp_get_selection_info()->session_name, conf);
        return true;
    }
    return false;
}

void jp_test_func() {
    int _ii;
    char* input = "example #3 text #5";
    char* output;
    output = jp_generate_session_name_on_copy(input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.1", input, output);
        free(output);
    }
    input = "example #3 text #a";
    output = jp_generate_session_name_on_copy( input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.2", input, output);
        free(output);
    }
    input = "example #3 text";
    output = jp_generate_session_name_on_copy(input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.3", input, output);
        free(output);
    }
    input = "example #3";
    output = jp_generate_session_name_on_copy(input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.3", input, output);
        free(output);
    }
    input = "example #3 ";
    output = jp_generate_session_name_on_copy(input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.3", input, output);
        free(output);
    }
    input = "example text";
    output = jp_generate_session_name_on_copy(input, " #");
    if (output) {
        JP_DEBUG_PRINT_VAR_STR2("0012.4", input, output);
        free(output);
    }
    output = NULL;
    _ii = 123;
}
