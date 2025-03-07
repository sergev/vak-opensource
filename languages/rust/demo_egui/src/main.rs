//
// Use egui for the main GUI (immediate-mode, modern aesthetics) and
// pair it with the rfd (Rust File Dialogs) crate for native file dialogs.
// For message boxes, integrate native_dialog.
//
use eframe::egui;
use rfd::FileDialog;
use native_dialog::{MessageDialog, MessageType};

struct MyApp {
    file_path: Option<String>,
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("egui with Native Dialogs (eframe 0.31.0)");

            if ui.button("Open File").clicked() {
                if let Some(path) = FileDialog::new()
                    .add_filter("Text", &["txt"])
                    .add_filter("All", &["*"])
                    .pick_file()
                {
                    self.file_path = Some(path.display().to_string());
                }
            }

            if ui.button("Show Message").clicked() {
                MessageDialog::new()
                    .set_title("Info")
                    .set_text("This is a native message box!")
                    .set_type(MessageType::Info)
                    .show_alert()
                    .unwrap();
            }

            if let Some(ref path) = self.file_path {
                ui.label(format!("Selected file: {}", path));
            } else {
                ui.label("No file selected yet.");
            }
        });
    }
}

fn main() {
    // Configure NativeOptions for eframe 0.31.0, including shader_version
    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([400.0, 300.0])  // Window size
            .with_resizable(false)            // Non-resizable window
            .with_decorations(true),          // Show title bar
        vsync: true,                          // Enable vsync
        multisampling: 4,                     // Basic anti-aliasing
        depth_buffer: 0,                      // No depth buffer (2D UI)
        stencil_buffer: 0,                    // No stencil buffer
        hardware_acceleration: eframe::HardwareAcceleration::Preferred, // Prefer GPU
        renderer: eframe::Renderer::Glow,     // Use OpenGL
        run_and_return: false,                // Exit when window closes
        event_loop_builder: None,             // No custom event loop
        window_builder: None,                 // No legacy window tweaks
        centered: true,                       // Center the window
        persist_window: true,                 // Save window state
        persistence_path: None,               // Default persistence path
        dithering: true,                      // Reduce banding in gradients
        shader_version: None,                 // Explicitly set shader_version
    };

    // Run the app
    eframe::run_native(
        "egui + Native Dialogs",              // App name
        native_options,                       // Updated options
        Box::new(|_cc| Ok(Box::new(MyApp { file_path: None }))), // App creator
    ).expect("Failed to launch application");
}
