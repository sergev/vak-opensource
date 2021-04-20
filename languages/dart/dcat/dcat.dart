import 'dart:convert';
import 'dart:io';

import 'package:args/args.dart';

const lineNumber = 'line-number';

ArgResults argResults;

void main(List<String> arguments) {
  exitCode = 0; // presume success
  final parser = ArgParser()
    ..addFlag(lineNumber, negatable: false, abbr: 'n');

  argResults = parser.parse(arguments);
  final paths = argResults.rest;

  dcat(paths, argResults[lineNumber] as bool);
}

Future dcat(List<String> paths, bool showLineNumbers) async {
  if (paths.isEmpty) {
    // No files provided as arguments. Read from stdin and print each line.
    await stdin.pipe(stdout);
  } else {
    for (var path in paths) {
      var lineNumber = 1;
      final lines = utf8.decoder
          .bind(File(path).openRead())
          .transform(const LineSplitter());
      try {
        await for (var line in lines) {
          if (showLineNumbers) {
            stdout.write('${lineNumber++} ');
          }
          stdout.writeln(line);
        }
      } catch (_) {
        await _handleError(path);
      }
    }
  }
}

Future _handleError(String path) async {
  if (await FileSystemEntity.isDirectory(path)) {
    stderr.writeln('error: $path is a directory');
  } else {
    exitCode = 2;
  }
}
