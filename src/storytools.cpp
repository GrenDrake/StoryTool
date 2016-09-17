#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <vector>

#include "story.h"
#include "exporter.h"

typedef struct EXPORTER {
	const char *code;
	unsigned int minArgs;
	unsigned int maxArgs;
	const char *desc;
	StoryExporter* (*exportFunc)(const std::vector<std::string>&);
} exporter_t;

StoryExporter* exportRTF(const std::vector<std::string> &args);
StoryExporter* exportHTML(const std::vector<std::string> &args);
StoryExporter* exportHTMLSeperate(const std::vector<std::string> &args);
StoryExporter* exportBare(const std::vector<std::string> &args);
StoryExporter* exportMarkdown(const std::vector<std::string> &args);

exporter_t exporters[] = {
	{ "bare", 1, 1, "Plain text with no formatting, headers, or metadata", exportBare },
	{ "html", 1, 2, "HTML page with optional template", exportHTML },
	{ "htmlsep", 1, 2, "Export as a seperate HTML page for each chapter with optional template", exportHTMLSeperate },
	{ "markdown", 1, 1, "Translate into markdown format", exportMarkdown },
	{ "rtf",  1, 1, "Export as RTF document", exportRTF },
	// TODO bbcode
	{ nullptr, 0, 0, nullptr, nullptr }
};

void showHelp() {
	std::cerr << "USAGE: storytools <filename> [-quiet] [<export_format> <export_file> [export options]]\n";
	std::cerr << "Supported export formats:\n";
	for (size_t i = 0; exporters[i].code != 0; ++i) {
		std::cerr << std::setw(12) << exporters[i].code << "   ";
		std::cerr << exporters[i].desc << "\n";
	}
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		showHelp();
		return 1;
	}

	bool showCount = true;
	std::locale loc("");
	std::cout.imbue(loc);

	std::string sourceFilename(argv[1]);
	Story story;
	story.fromFile(sourceFilename);

	size_t firstArg = 3;
	if (argc > 2) {
		if (strcmp(argv[2], "-quiet") == 0) {
			++firstArg;
			showCount = false;
		}
	}

	if (showCount) {
		story.displayInfo();
	}

	if (argc >= (int)firstArg) {
		StoryExporter *exporter = nullptr;
		std::string exporterName = argv[firstArg - 1];

		if (!exporterName.empty()) {
			std::vector<std::string> args;
			for (int i = firstArg; i < argc; ++i) {
				args.push_back(argv[i]);
			}

			if (args.empty()) {
				std::cerr << "Tried to export story to nowhere!\n\n";
				showHelp();
				return 1;
			}

			for (size_t i = 0; exporter == nullptr && exporters[i].code != 0; ++i) {
				if (exporterName == exporters[i].code) {
					if (args.size() < exporters[i].minArgs || args.size() > exporters[i].maxArgs) {
						std::cerr << exporters[i].code << " exporter expects ";
						std::cerr << exporters[i].minArgs << " to ";
						std::cerr << exporters[i].maxArgs << " arguments; found ";
						std::cerr << argc << "\n";
						return 1;
					}
					exporter = exporters[i].exportFunc(args);
				}
			}

			if (exporter == nullptr) {
				std::cerr << "Unrecognized exporter " << argv[2] << ".\n\n";
				showHelp();
				return 1;
			}

			if (!exporter->good()) {
				std::cerr << "Error writing to file " << argv[3] << ".\n";
				return 1;
			}
			story.doExport(exporter);
			delete exporter;
		}
	}

	return 0;
}

StoryExporter* exportRTF(const std::vector<std::string> &args) {
	return new RTFExporter(args[0]);
}
StoryExporter* exportHTML(const std::vector<std::string> &args) {
	if (args.size() > 1) {
		return new HTMLSingleExporter(args[0], args[1]);
	}
	return new HTMLSingleExporter(args[0], "");
}
StoryExporter* exportHTMLSeperate(const std::vector<std::string> &args) {
	if (args.size() > 1) {
		return new HTMLMultipleExporter(args[0], args[1]);
	}
	return new HTMLMultipleExporter(args[0], "");
}
StoryExporter* exportBare(const std::vector<std::string> &args) {
	return new BareExporter(args[0]);
}
StoryExporter* exportMarkdown(const std::vector<std::string> &args) {
	return new MarkdownExporter(args[0]);
}
