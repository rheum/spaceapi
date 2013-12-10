#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "green.h"
#include "yellow.h"
#include "red.h"

#define DIRECTORY_URL "http://spaceapi.net/directory.json"

struct hacker_space {
    char* name;
    char* url;
};

gboolean animate_startup(gpointer data);
gboolean fetch_directory();
gboolean refresh_status(gpointer data);
gboolean update_directory();
int compare_spaces(const void *p1, const void *p2);
void popup_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data);
void select_space(GtkCheckMenuItem* menu_item, gpointer data);
void free_directory();
void init_gui();
void init_icons();
void read_config();
void update_menu_items();
