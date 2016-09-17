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
	int minArgs;
	int maxArgs;
	const char *desc;
	StoryExporter* (*exportFunc)(const std::vector<std::string>&);
} exporter_t;

StoryExporter* exportRTF(const std::vector<std::string> &args);
StoryExporter* exportHTML(const std::vector<std::string> &args);
StoryExporter* exportHTMLSeperate(const std::vector<std::string> &args);
StoryExporter* exportBare(const std::vector<std::string> &args);
StoryExporter* exportMarkdown(const std::vector<std::string> &args);

int main(int argc, char *argv[]) {
	exporter_t exporters[] = {
		{ "bare", 1, 1, "Plain text with no formatting, headers, or metadata", exportBare },
		{ "html", 1, 2, "HTML page with optional template", exportHTML },
		{ "htmlsep", 1, 2, "Export as a seperate HTML page for each chapter with optional template", exportHTMLSeperate },
		{ "markdown", 1, 1, "Translate into markdown format", exportMarkdown },
		{ "rtf",  1, 1, "Export as RTF document", exportRTF },
		// TODO bbcode
		{ nullptr, 0, 0, nullptr, nullptr }
	};

	if (argc == 1 || argc == 3) {
		std::cerr << "USAGE: storytools <filename> [<export_format> <export_file> [export options]]\n";
		std::cerr << "Supported export formats:\n";
		for (size_t i = 0; exporters[i].code != 0; ++i) {
			std::cerr << std::setw(12) << exporters[i].code << "   ";
			std::cerr << exporters[i].desc << "\n";
		}
		return 1;
	}

	std::locale loc("");
	std::cout.imbue(loc);

	std::string sourceFilename(argv[1]);
	Story story;
	story.fromFile(sourceFilename);
	story.displayInfo();

	if (argc > 2) {
		StoryExporter *exporter = nullptr;

		std::vector<std::string> args;
		for (size_t i = 3; i < argc; ++i) {
			args.push_back(argv[i]);
		}

		for (size_t i = 0; exporter == nullptr && exporters[i].code != 0; ++i) {
			if (strcmp(exporters[i].code, argv[2]) == 0) {
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
			std::cerr << "Unrecognized exporter " << argv[2] << ".\n";
			return 1;
		}

		if (!exporter->good()) {
			std::cerr << "Error writing to file " << argv[3] << ".\n";
			return 1;
		}
		story.doExport(exporter);
		delete exporter;
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
