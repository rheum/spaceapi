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


void init_icons();

void init_gui();

gboolean update_directory();

gboolean fetch_directory();

static int compare_spaces(const void *p1, const void *p2);

void free_directory();

void update_menu_items();

void read_config();

static void select_space(GtkCheckMenuItem* menu_item, gpointer data);

static void popup_menu(GtkStatusIcon *status_icon, guint button,
        guint activate_time, gpointer user_data);
