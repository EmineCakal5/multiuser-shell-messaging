#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "include/model.h"
#include "include/controller.h"

GtkTextBuffer* text_buffer;
ShmBuf* shmp;

static void append_text(const char* text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(text_buffer, &end);
    gtk_text_buffer_insert(text_buffer, &end, text, -1);
    gtk_text_buffer_insert(text_buffer, &end, "\n", -1);
}

static gboolean update_messages(gpointer data) {
    static char last_msg[BUF_SIZE] = "";  // daha önce gösterilen mesaj

    char* new_msg = model_read_messages(shmp);
    if (new_msg && strlen(new_msg) > 0 && strcmp(new_msg, last_msg) != 0) {
        append_text("[Paylaşılan Mesaj]:");
        append_text(new_msg);
        strncpy(last_msg, new_msg, BUF_SIZE - 1);  // kopyala
        last_msg[BUF_SIZE - 1] = '\0';  // güvenlik
        free(new_msg);
    } else {
        free(new_msg);
    }
    return TRUE; // tekrar çağırmak için TRUE
}

static void on_send_clicked(GtkWidget* widget, gpointer entry_ptr) {
    const char* input = gtk_entry_get_text(GTK_ENTRY(entry_ptr));
    char* response = NULL;
    handle_input(input, shmp, &response);

    if (response) {
        append_text(response);
        free(response);
    } else {
        append_text("Komut işlenemedi veya boş.");
    }
    gtk_entry_set_text(GTK_ENTRY(entry_ptr), "");
}

static void generate_user_tag() {
    char tag[16];
    srand(getpid());
    snprintf(tag, sizeof(tag), "Shell%d", rand() % 1000);
    setenv("USER_TAG", tag, 1);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    generate_user_tag();
    shmp = buf_init();

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Furkan'ın Ballı Turtası Terminali");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget* text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 5);
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkWidget* entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

    GtkWidget* button = gtk_button_new_with_label("Gönder");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);

    g_signal_connect(button, "clicked", G_CALLBACK(on_send_clicked), entry);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    // Shared memory'den mesajları döngüyle kontrol et
    g_timeout_add(1000, update_messages, NULL);

    gtk_main();
    return 0;
}
