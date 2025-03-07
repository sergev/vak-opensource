#!/usr/bin/env python3
import dearpygui.dearpygui as dpg
import os

# Callback for button
def button_callback():
    dpg.set_value("status_text", "Button Clicked!")

# Initialize Dear PyGui
dpg.create_context()

# Set up the viewport
dpg.create_viewport(title="Natural Font GUI", width=400, height=300, resizable=False)

# Load and bind a font
with dpg.font_registry():
    # Option 1: Use San Francisco (macOS system font)
    # Path may vary; adjust if needed (requires sudo access or copying the font)
    font_path = "/System/Library/Fonts/SFNS.ttf"  # San Francisco font (may need adjustment)
    font_path = "/System/Library/Fonts/Geneva.ttf"
    font_path = "/System/Library/Fonts/HelveticaNeue.ttc"

    # Option 2: Use a downloaded font (e.g., Roboto) for portability
    # Download from Google Fonts and place in your project directory
    # font_path = "Roboto-Regular.ttf"

    if os.path.exists(font_path):
        custom_font = dpg.add_font(font_path, 18)  # 16 is the font size in pixels
        dpg.bind_font(custom_font)
    else:
        print(f"Font not found at {font_path}. Using default font.")

# Set up a simple dark theme
with dpg.theme() as global_theme:
    with dpg.theme_component(dpg.mvAll):
        dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (20, 20, 20, 255))  # Dark background
        dpg.add_theme_color(dpg.mvThemeCol_Text, (220, 220, 220, 255))   # Light text
        dpg.add_theme_color(dpg.mvThemeCol_Button, (50, 100, 150, 255))  # Blue button
        dpg.add_theme_style(dpg.mvStyleVar_FrameRounding, 5)             # Rounded corners

        # Light theme for a macOS-like feel
        #dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (240, 240, 240, 255))  # Light gray background
        #dpg.add_theme_color(dpg.mvThemeCol_Text, (0, 0, 0, 255))           # Black text

        # Add padding to mimic macOS’s airy layouts.
        dpg.add_theme_style(dpg.mvStyleVar_ItemSpacing, 8, 8)

dpg.bind_theme(global_theme)

# Create the window and widgets
with dpg.window(label="Main Window", width=400, height=300, no_resize=True, no_move=True):
    dpg.add_text("Welcome to Dear PyGui!", pos=(120, 50))
    dpg.add_button(label="Click Me", callback=button_callback, pos=(150, 100), width=100, height=30)
    dpg.add_text("Status:", tag="status_text", pos=(120, 150))

# Run the GUI
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()
