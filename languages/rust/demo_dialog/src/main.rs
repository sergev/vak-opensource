use rfd::FileDialog;
use native_dialog::{MessageDialog, MessageType};
use std::path::PathBuf;

fn main() {
    // Use rfd to pick a file
    println!("Please select a file...");
    let file_path: Option<PathBuf> = FileDialog::new()
        .add_filter("Text Files", &["txt"])
        .add_filter("All Files", &["*"])
        .set_title("Choose a File")
        .pick_file();

    // Process the result and show a native message dialog
    match file_path {
        Some(path) => {
            let path_str = path.display().to_string();
            println!("Selected file: {}", &path_str);

            // Use native_dialog to show a confirmation message
            let text = format!("You chose: {}\nDo you want to proceed?", path_str);
            let dialog = MessageDialog::new()
                .set_title("File Selected")
                .set_text(&text)
                .set_type(MessageType::Info);

            let result = dialog.show_confirm().unwrap_or(false);
            if result {
                println!("User confirmed: Proceeding with {}", path_str);
            } else {
                println!("User canceled.");
            }
        }
        None => {
            // Show an error message if no file was selected
            MessageDialog::new()
                .set_title("No Selection")
                .set_text("You didn’t select a file.")
                .set_type(MessageType::Warning)
                .show_alert()
                .unwrap();

            println!("No file selected.");
        }
    }
}
