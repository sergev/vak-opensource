use iced::Application;

//
// Run the Application.
// Call method run() which will take control of the current thread and will NOT return.
// It should be the last thing in the main function.
//
pub fn main() -> iced::Result {
    // Use logger for debug messages.
    simple_logger::init_with_level(log::Level::Warn).unwrap();

    MyApp::run(iced::Settings::default())
}

//
// The state of our application.
//
#[derive(Default)]
struct MyApp {
    button1_state: iced::button::State,
    button2_state: iced::button::State,
    button3_state: iced::button::State,
    phrase_selected: String,
}

//
// The type of messages our application will produce.
//
#[derive(Debug, Clone)]
pub enum Message {
    Phrase(String),
}

//
// Implement Application trait: the main entrypoint of Iced.
// On native platforms, it will run in its own window.
// An Application can execute asynchronous actions by returning
// a Command in some of its methods.
//
impl iced::Application for MyApp {

    type Message = Message;
    type Executor = iced::executor::Default;
    type Flags = ();

    //
    // Initialize the Application.
    // Return the initial state of our app.
    // Also return a Command to perform some async action in the background on startup.
    //
    fn new(_flags: ()) -> (Self, iced::Command<Message>) {
        (
            Self {
                phrase_selected: "Hello, World!".to_string(),
                ..Self::default()
            },
            iced::Command::none(),
        )
    }

    //
    // Return the current title of the Application.
    // This title can be dynamic! The runtime will automatically
    // update the title of the application when necessary.
    //
    fn title(&self) -> String {
        String::from("MyApp - Iced")
    }

    //
    // Process messages and update the state of the Application.
    // All the messages, produced by either user interactions or commands,
    // are be handled by this method.
    // Any Command returned will be executed immediately in the background.
    //
    fn update(&mut self, message: Message, _cb: &mut iced::Clipboard) -> iced::Command<Message> {
        match message {
            Message::Phrase(phrase) => {
                log::warn!("phrase = {}", phrase);
                self.phrase_selected = phrase;
            }
        }

        iced::Command::none()
    }

    //
    // Return the widgets to display in the Application.
    // These widgets can produce messages based on user interaction.
    //
    fn view(&mut self) -> iced::Element<Message> {
        let message = iced::Text::new(&self.phrase_selected)
            .size(96);

        let canvas = iced::Container::new(message)
            .width(iced::Length::Fill)
            .height(iced::Length::Fill)
            .padding(20)
            .center_x()
            .center_y();

        let controls = iced::Column::new()
            .padding(10)
            .spacing(20)
            .push(iced::Button::new(&mut self.button1_state, iced::Text::new("Hi there!"))
                    .on_press(Message::Phrase("Hi there!".to_string())))
            .push(iced::Button::new(&mut self.button2_state, iced::Text::new("Hola!"))
                    .on_press(Message::Phrase("Hola!".to_string())))
            .push(iced::Button::new(&mut self.button3_state, iced::Text::new("Приветик!"))
                    .on_press(Message::Phrase("Приветик!".to_string())));

        let content = iced::Row::new()
            .spacing(20)
            .push(canvas)
            .push(controls);

        content.into()
    }
}
