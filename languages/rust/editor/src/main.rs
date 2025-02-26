use crossterm::{
    event::{self, Event, KeyCode, KeyEventKind},
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};
use ratatui::{
    backend::CrosstermBackend,
    layout::{Constraint, Layout},
    style::{Color, Style},
    text::{Line, Span},
    widgets::{Block, Borders, Paragraph},
    Terminal,
};
use std::io;

fn main() -> Result<(), io::Error> {
    // Set up terminal
    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen)?;
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    // Run the editor
    let app = App::new();
    let res = run_app(&mut terminal, app);

    // Clean up terminal
    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    terminal.show_cursor()?;

    if let Err(err) = res {
        println!("{:?}", err);
    }

    Ok(())
}

struct App {
    text: Vec<String>, // Lines of text
    cursor_x: usize,   // Cursor position in current line
    cursor_y: usize,   // Current line number
}

impl App {
    fn new() -> App {
        App {
            text: vec![String::new()], // Start with one empty line
            cursor_x: 0,
            cursor_y: 0,
        }
    }

    fn move_cursor_left(&mut self) {
        if self.cursor_x > 0 {
            self.cursor_x -= 1;
        }
    }

    fn move_cursor_right(&mut self) {
        let line_len = self.text[self.cursor_y].len();
        if self.cursor_x < line_len {
            self.cursor_x += 1;
        }
    }

    fn move_cursor_up(&mut self) {
        if self.cursor_y > 0 {
            self.cursor_y -= 1;
            self.cursor_x = self.cursor_x.min(self.text[self.cursor_y].len());
        }
    }

    fn move_cursor_down(&mut self) {
        if self.cursor_y < self.text.len() - 1 {
            self.cursor_y += 1;
            self.cursor_x = self.cursor_x.min(self.text[self.cursor_y].len());
        }
    }

    fn insert_char(&mut self, c: char) {
        let line = &mut self.text[self.cursor_y];
        line.insert(self.cursor_x, c);
        self.cursor_x += 1;
    }

    fn insert_newline(&mut self) {
        let current_line = &mut self.text[self.cursor_y];
        let remainder = current_line.split_off(self.cursor_x);
        self.text.insert(self.cursor_y + 1, remainder);
        self.cursor_y += 1;
        self.cursor_x = 0;
    }

    fn delete_char(&mut self) {
        if self.cursor_x > 0 {
            let line = &mut self.text[self.cursor_y];
            line.remove(self.cursor_x - 1);
            self.cursor_x -= 1;
        } else if self.cursor_y > 0 {
            let current_line = self.text.remove(self.cursor_y);
            self.cursor_y -= 1;
            self.cursor_x = self.text[self.cursor_y].len();
            self.text[self.cursor_y].push_str(&current_line);
        }
    }
}

fn run_app<B: ratatui::backend::Backend>(terminal: &mut Terminal<B>, mut app: App) -> io::Result<()> {
    loop {
        terminal.draw(|f| {
            let chunks = Layout::vertical([Constraint::Min(0)]).split(f.size());
            let text: Vec<Line> = app
                .text
                .iter()
                .map(|line| Line::from(Span::raw(line)))
                .collect();

            let paragraph = Paragraph::new(text)
                .block(Block::default().title("Simple Text Editor").borders(Borders::ALL))
                .style(Style::default().fg(Color::White));
            f.render_widget(paragraph, chunks[0]);

            // Set cursor position
            f.set_cursor(
                chunks[0].x + 1 + app.cursor_x as u16,
                chunks[0].y + 1 + app.cursor_y as u16,
            );
        })?;

        if let Event::Key(key) = event::read()? {
            if key.kind == KeyEventKind::Press {
                match key.code {
                    KeyCode::Char('c') if key.modifiers.contains(event::KeyModifiers::CONTROL) => {
                        return Ok(());
                    }
                    KeyCode::Char(c) => app.insert_char(c),
                    KeyCode::Enter => app.insert_newline(),
                    KeyCode::Backspace => app.delete_char(),
                    KeyCode::Left => app.move_cursor_left(),
                    KeyCode::Right => app.move_cursor_right(),
                    KeyCode::Up => app.move_cursor_up(),
                    KeyCode::Down => app.move_cursor_down(),
                    _ => {}
                }
            }
        }
    }
}