#include "spaceapi.h"

GdkPixbuf *green_icon = NULL;
GdkPixbuf *yellow_icon = NULL;
GdkPixbuf *red_icon = NULL;
guint anim_id = 0;

GtkStatusIcon *status_icon = NULL;
GtkMenu *menu = NULL;
GtkMenu *sub_menu = NULL;
GtkCheckMenuItem** space_items = NULL;

struct hacker_space *directory = NULL;
int space_count = 0;
int selected_space = -1;

void read_config(){

}

void init_icons() {
    GdkPixbufLoader *loader;

    loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, green_png, 3028, NULL);
    green_icon = gdk_pixbuf_loader_get_pixbuf(loader);
    gdk_pixbuf_loader_close(loader, NULL);

    loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, yellow_png, 3120, NULL);
    yellow_icon = gdk_pixbuf_loader_get_pixbuf(loader);
    gdk_pixbuf_loader_close(loader, NULL);

    loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, red_png, 3026, NULL);
    red_icon = gdk_pixbuf_loader_get_pixbuf(loader);
    gdk_pixbuf_loader_close(loader, NULL);

    if (green_icon == NULL || yellow_icon == NULL || red_icon == NULL) {
        fputs("Failed to load icons from resources.", stderr);
        exit(1);
    }
}

void init_gui() {
    menu = (GtkMenu*) gtk_menu_new();
    sub_menu = (GtkMenu*) gtk_menu_new();
    gtk_widget_set_sensitive((GtkWidget*) sub_menu, FALSE);

    GtkMenuItem *sub_menu_item = (GtkMenuItem*) gtk_menu_item_new_with_label("Spaces");
    gtk_menu_item_set_submenu(sub_menu_item, (GtkWidget*) sub_menu);

    GtkImageMenuItem *quit_menu_item = (GtkImageMenuItem*)
                                       gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    g_signal_connect(G_OBJECT(quit_menu_item), "activate",
                     G_CALLBACK(gtk_main_quit), NULL);

    GtkImageMenuItem *settings_menu_item = (GtkImageMenuItem*)
                                           gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);

    gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) sub_menu_item);
    gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) settings_menu_item);
    gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) quit_menu_item);
}

gboolean animate_startup(gpointer data) {
    static int i = 0;
    if (i == 0) {
        gtk_status_icon_set_from_pixbuf(status_icon, green_icon);
    }
    else if (i == 1) {
        gtk_status_icon_set_from_pixbuf(status_icon, yellow_icon);
    }
    else if (i == 2) {
        gtk_status_icon_set_from_pixbuf(status_icon, red_icon);
    }
    i++;
    i %= 3;
    return TRUE;
}

gboolean update_directory(gpointer data) {
    if (fetch_directory())
        update_menu_items();
    return FALSE;
}

gboolean fetch_directory() {
    if (directory != NULL) {
        free_directory();
    }

    char *buf = NULL;
    buf = perform_http_get(DIRECTORY_URL);

    if (buf == NULL)
        return FALSE;

    char *ptr = strtok(buf, "{}\t\n");
    char *ptr2 = NULL;
    char *ptr3 = NULL;
    for (; ptr != NULL; ptr = strtok(NULL, "{}\t\n")) {
        ptr = strchr(ptr, '"');
        ptr2 = strchr(ptr, ':');
        ptr3 = strchr(ptr2 + 2, '"');

        struct hacker_space space;
        space.name = strndup(ptr + 1, ptr2 - ptr - 2);
        space.url = strndup(ptr2 + 2, ptr3 - ptr2 -2);

        space_count++;
        directory = realloc(directory,
                            space_count * sizeof(struct hacker_space));
        directory[space_count-1] = space;
    }
    free(buf);
    qsort(directory, space_count, sizeof(struct hacker_space), compare_spaces);
    return TRUE;
}

void free_directory() {
    gtk_widget_set_sensitive((GtkWidget*) sub_menu, FALSE);
    for (int i = 0; i < space_count; i++) {
        free(directory[i].name);
        free(directory[i].url);
        gtk_widget_destroy((GtkWidget*) space_items[i]);
    }
    free(directory);
    directory = NULL;
    space_count = 0;
}

int compare_spaces(const void *p1, const void *p2) {
    return strcmp(((struct hacker_space*) p1)->name, ((struct hacker_space*) p2)->name);
}


void update_menu_items() {
    space_items = realloc(space_items, space_count * sizeof(GtkCheckMenuItem));
    for (int i = 0; i < space_count; i++) {
        space_items[i] = (GtkCheckMenuItem*) gtk_check_menu_item_new_with_label(directory[i].name);
        gtk_menu_shell_append((GtkMenuShell*) sub_menu, (GtkWidget*) space_items[i]);
        g_signal_connect(G_OBJECT(space_items[i]), "toggled", G_CALLBACK(select_space), GINT_TO_POINTER(i));
    }
    gtk_widget_set_sensitive((GtkWidget*) sub_menu, TRUE);
}


void popup_menu(GtkStatusIcon *status_icon, guint button,
                       guint activate_time, gpointer user_data) {
    gtk_widget_show((GtkWidget*) menu);
    gtk_widget_show_all((GtkWidget*) menu);
    gtk_menu_popup(menu, NULL, NULL, gtk_status_icon_position_menu,
                   status_icon, button, activate_time);
}

void select_space(GtkCheckMenuItem* menu_item, gpointer user_data){
    if(gtk_check_menu_item_get_active(menu_item)){
        printf("Activate: %s\n", directory[GPOINTER_TO_INT(user_data)].name);
        for (int i = 0; i < space_count; i++){
            if ( menu_item != space_items[i] ){
                gtk_check_menu_item_set_active(space_items[i], FALSE);
            } else {
                selected_space = i;
                g_source_remove(anim_id);
            }
        }
    }
    refresh_status(NULL);
}

gboolean refresh_status(gpointer data){
    if (selected_space != -1){
        gtk_status_icon_set_from_pixbuf(status_icon, yellow_icon);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    read_config();

    gtk_init(&argc, &argv);
    init_icons();
    init_gui();


    status_icon = gtk_status_icon_new_from_pixbuf(green_icon);
    g_signal_connect(G_OBJECT(status_icon), "popup-menu", G_CALLBACK(popup_menu), NULL);

    g_idle_add(update_directory, NULL);
    anim_id = g_timeout_add(500, animate_startup, NULL);
    g_timeout_add(5*60*1000, refresh_status, NULL);

    gtk_main();
}
