#include <gtk/gtk.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctime>

using namespace cv;

// Function to apply random distortions
void distort_image(const char *input_path, const char *output_path) {
    Mat img = imread(input_path);
    if (img.empty()) return;

    srand(time(0)); // Seed for randomness

    // Apply random transformations
    if (rand() % 2) { // Random rotation
        int angle = rand() % 360;
        Point2f center(img.cols / 2.0, img.rows / 2.0);
        Mat rot_mat = getRotationMatrix2D(center, angle, 1.0);
        warpAffine(img, img, rot_mat, img.size());
    }
    
    if (rand() % 2) { // Random blur
        int blur_size = 1 + 2 * (rand() % 3); // Always odd
        GaussianBlur(img, img, Size(blur_size, blur_size), 0);
    }

    if (rand() % 2) { // Random noise
        Mat noise = Mat::zeros(img.size(), img.type());
        randn(noise, Scalar::all(0), Scalar::all(30));
        img += noise;
    }

    if (rand() % 2) { // Random brightness shift
        int brightness = (rand() % 101) - 50; // Range: -50 to +50
        img += Scalar(brightness, brightness, brightness);
    }

    if (rand() % 2) { // Random contrast adjustment
        double alpha = 0.5 + static_cast<double>(rand()) / RAND_MAX; // 0.5 to 1.5
        img.convertTo(img, -1, alpha, 0);
    }

    // Save the distorted image
    imwrite(output_path, img);
}

// Process all images in a folder
void process_images(const char *input_folder, const char *output_folder) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(input_folder);
    if (!dir) return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            char input_path[1024], output_path[1024];
            snprintf(input_path, sizeof(input_path), "%s/%s", input_folder, entry->d_name);
            snprintf(output_path, sizeof(output_path), "%s/%s", output_folder, entry->d_name);
            distort_image(input_path, output_path);
        }
    }
    closedir(dir);
}

// GTK File Chooser for selecting folders
char *select_folder(GtkWidget *parent) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Folder",
                                                    GTK_WINDOW(parent),
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    char *folder_path = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        folder_path = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
    }
    gtk_widget_destroy(dialog);
    return folder_path;
}

// GTK Button Click Callback
static void on_process_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    char *input_folder = select_folder(window);
    if (!input_folder) return;

    char *output_folder = select_folder(window);
    if (!output_folder) {
        g_free(input_folder);
        return;
    }

    process_images(input_folder, output_folder);

    GtkWidget *message = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                "Image processing complete!");
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);

    g_free(input_folder);
    g_free(output_folder);
}

// Main GUI
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Distortion Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *button = gtk_button_new_with_label("Select Folders & Process");
    g_signal_connect(button, "clicked", G_CALLBACK(on_process_clicked), window);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 10);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
