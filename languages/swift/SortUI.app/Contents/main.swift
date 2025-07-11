import SwiftUI
import Foundation

@main
struct HelloApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

struct ContentView: View {
    @State private var inputText: String = "banana\napple\ncherry\ndate\n"
    @State private var outputText: String = ""

    var body: some View {
        VStack(spacing: 20) {
            VStack(alignment: .leading) {
                Text("Input (Editable):")
                    .font(.headline)

                TextEditor(text: $inputText)
                    .font(.system(size: 14, design: .monospaced))
                    .border(Color.gray, width: 1)
                    .padding(.horizontal)
                    .frame(maxHeight: .infinity)
            }
            .frame(minHeight: 150, maxHeight: .infinity)

            Button("Run") {
                runSort()
            }
            .padding()
            .background(Color.blue)
            .foregroundColor(.white)
            .cornerRadius(8)

            VStack(alignment: .leading) {
                Text("Output (Read-only):")
                    .font(.headline)

                ScrollView {
                    Text(outputText)
                        .font(.system(size: 14, design: .monospaced))
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(5)
                }
                .border(Color.gray, width: 1)
                .padding(.horizontal, 15)
                .frame(maxHeight: .infinity)
            }
            .frame(minHeight: 150, maxHeight: .infinity)
        }
        .padding(.bottom, 20)
        .frame(minWidth: 400, maxWidth: .infinity, minHeight: 600, maxHeight: .infinity)
    }

    func runSort() {
        let process = Process()
        process.executableURL = URL(fileURLWithPath: "/usr/bin/sort")

        let inputPipe = Pipe()
        let outputPipe = Pipe()

        process.standardInput = inputPipe
        process.standardOutput = outputPipe

        do {
            try process.run()

            if let inputData = inputText.data(using: .utf8) {
                try inputPipe.fileHandleForWriting.write(inputData)
                try inputPipe.fileHandleForWriting.close()
            }

            process.waitUntilExit()

            if let outputData = try outputPipe.fileHandleForReading.readToEnd() {
                outputText = String(data: outputData, encoding: .utf8) ?? "Error reading output"
            } else {
                outputText = "No output"
            }
        } catch {
            outputText = "Error: \(error.localizedDescription)"
        }
    }
}
