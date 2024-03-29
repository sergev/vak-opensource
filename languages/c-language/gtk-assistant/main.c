/* Assistant
 *
 * Demonstrates a sample multi-step assistant. Assistants are used to divide
 * an operation into several simpler sequential steps, and to guide the user
 * through these steps.
 */

#include <gtk/gtk.h>

GtkApplication *application;
static GtkWidget *progress_bar = NULL;

static gboolean apply_changes_gradually(gpointer data)
{
    gdouble fraction;

    /* Work, work, work... */
    fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progress_bar));
    fraction += 0.05;

    if (fraction < 1.0) {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), fraction);
        return G_SOURCE_CONTINUE;
    } else {
        /* Close automatically once changes are fully applied. */
        g_application_quit(G_APPLICATION(application));
        return G_SOURCE_REMOVE;
    }
}

static void on_assistant_apply(GtkWidget *widget, gpointer data)
{
    /* Start a timer to simulate changes taking a few seconds to apply. */
    g_timeout_add(100, apply_changes_gradually, NULL);
}

static void on_assistant_close_cancel(GtkWidget *widget, gpointer data)
{
    GtkWidget **assistant = (GtkWidget **)data;

    gtk_widget_destroy(*assistant);
    *assistant = NULL;
}

static void on_assistant_prepare(GtkWidget *widget, GtkWidget *page, gpointer data)
{
    gint current_page, n_pages;
    gchar *title;

    current_page = gtk_assistant_get_current_page(GTK_ASSISTANT(widget));
    n_pages = gtk_assistant_get_n_pages(GTK_ASSISTANT(widget));

    title = g_strdup_printf("Sample assistant (%d of %d)", current_page + 1, n_pages);
    gtk_window_set_title(GTK_WINDOW(widget), title);
    g_free(title);

    /* The fourth page (counting from zero) is the progress page.  The
     * user clicked Apply to get here so we tell the assistant to commit,
     * which means the changes up to this point are permanent and cannot
     * be cancelled or revisited. */
    if (current_page == 3)
        gtk_assistant_commit(GTK_ASSISTANT(widget));
}

static void on_entry_changed(GtkWidget *widget, gpointer data)
{
    GtkAssistant *assistant = GTK_ASSISTANT(data);
    GtkWidget *current_page;
    gint page_number;
    const gchar *text;

    page_number = gtk_assistant_get_current_page(assistant);
    current_page = gtk_assistant_get_nth_page(assistant, page_number);
    text = gtk_entry_get_text(GTK_ENTRY(widget));

    if (text && *text)
        gtk_assistant_set_page_complete(assistant, current_page, TRUE);
    else
        gtk_assistant_set_page_complete(assistant, current_page, FALSE);
}

static void create_page1(GtkWidget *assistant)
{
    GtkWidget *box, *label, *entry;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(box), 12);

    label = gtk_label_new("You must fill out this entry to continue:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
    gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_entry_changed), assistant);

    gtk_widget_show_all(box);
    gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Page 1");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), box, GTK_ASSISTANT_PAGE_INTRO);
}

static void create_page2(GtkWidget *assistant)
{
    GtkWidget *box, *checkbutton;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(box), 12);

    checkbutton = gtk_check_button_new_with_label(
        "This is optional data, you may continue "
        "even if you do not check this");
    gtk_box_pack_start(GTK_BOX(box), checkbutton, FALSE, FALSE, 0);

    gtk_widget_show_all(box);
    gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
    gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), box, TRUE);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Page 2");
}

static void create_page3(GtkWidget *assistant)
{
    GtkWidget *label;

    label = gtk_label_new("This is a confirmation page, press 'Apply' to apply changes");

    gtk_widget_show(label);
    gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONFIRM);
    gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), label, TRUE);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Confirmation");
}

static void create_page4(GtkWidget *assistant)
{
    progress_bar = gtk_progress_bar_new();
    gtk_widget_set_halign(progress_bar, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(progress_bar, GTK_ALIGN_CENTER);

    gtk_widget_show(progress_bar);
    gtk_assistant_append_page(GTK_ASSISTANT(assistant), progress_bar);
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), progress_bar,
                                GTK_ASSISTANT_PAGE_PROGRESS);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), progress_bar, "Applying changes");

    /* This prevents the assistant window from being
     * closed while we're "busy" applying changes.
     */
    gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), progress_bar, FALSE);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new(app);

    GtkWidget *assistant = gtk_assistant_new();
    gtk_window_set_default_size(GTK_WINDOW(assistant), 600, 300);

    create_page1(assistant);
    create_page2(assistant);
    create_page3(assistant);
    create_page4(assistant);

    g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(on_assistant_close_cancel),
                     &assistant);
    g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(on_assistant_close_cancel),
                     &assistant);
    g_signal_connect(G_OBJECT(assistant), "apply", G_CALLBACK(on_assistant_apply), NULL);
    g_signal_connect(G_OBJECT(assistant), "prepare", G_CALLBACK(on_assistant_prepare), NULL);

    gtk_widget_show(assistant);

    gtk_window_set_transient_for(GTK_WINDOW(assistant), GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    application = app;
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
