#include "spaceapi.h"

GdkPixbuf *green_icon;
GdkPixbuf *yellow_icon;
GdkPixbuf *red_icon;

CURL *curl;
CURLcode res;

void init_icons(){
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

    if (green_icon == NULL || yellow_icon == NULL || red_icon == NULL){
        fputs("Failed to load icons from resources.", stderr);
        exit(1);
    }
}

gboolean do_something(gpointer data){
    static int i = 0;
    if (i == 0){
        gtk_status_icon_set_from_pixbuf(data, green_icon);
    }
    else if (i == 1){
        gtk_status_icon_set_from_pixbuf(data, yellow_icon);
    }
    else if (i == 2){
        gtk_status_icon_set_from_pixbuf(data, red_icon);
    }
    i++;
    i %= 3;
    return TRUE;
}

size_t allocate_buffer(void *ptr, size_t size, size_t nmemb, void *data){
    size_t count = (size_t)(size*nmemb);
    char** buf = ((char **) data);
    if (*buf == NULL){
        *buf = strndup(ptr, count); 
    }else {
        *buf = realloc(*buf, (size_t) (strlen(*buf) + count));
        strncat(*buf, ptr, count);
    }
    return count;
}

gboolean fetch_directory(gpointer data){
    char *buf = NULL;
    curl = curl_easy_init();
    const char* url = "http://spaceapi.net/directory.json";
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, allocate_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
      fputs("curl failed", stderr);
    }else{
        puts(buf);
    }
    curl_easy_cleanup(curl);
    free(buf);
    return FALSE;
}

int main(int argc, char **argv){
    gtk_init(&argc, &argv);
    init_icons();

    GtkStatusIcon *status_icon = gtk_status_icon_new_from_pixbuf(green_icon);

    g_idle_add(fetch_directory, NULL);
    g_timeout_add(500, do_something, status_icon);
    
    gtk_main();
}
