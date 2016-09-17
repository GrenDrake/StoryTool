#include "exporter.h"


void BareExporter::start(const Story &story) {
}

void BareExporter::doChapter(const std::string &name) {
}

void BareExporter::doScene(const std::string &name) {
}

void BareExporter::doParagraph(const std::string &text) {
	std::string t = text;
	size_t pos = 0;
	while ( (pos = t.find_first_of('[')) != std::string::npos) {
		size_t end = t.find_first_of(']', pos);
		if (end == std::string::npos) {
			break;
		}
		t.erase(pos, end-pos + 1);
	}

	trim(t);
	if (!t.empty()) {
		outfile << t << "\n\n";
	}
}

void BareExporter::end() {
	if (outfile) {
		std::cout << "Bare export saved to " << filename << ".\n";
	}
}
