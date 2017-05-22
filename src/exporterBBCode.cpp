
#include "exporter.h"

void BBCodeExporter::start(const Story &story) {
	this->story = &story;
	outfile << "[b]" << story.getMetadata("title") << "[/b]\n";
	outfile << "[i]" << story.getMetadata("author") << ", " << story.getMetadata("date") << "[/i]\n\n";
}


void BBCodeExporter::doChapter(const std::string &name) {
	if (story->chapterCount() > 1) {
		outfile << "\n\n[b]Chapter " << (chapterNo++) << ": " << name << "[/b]\n\n";
	}
	firstScene = true;
}

void BBCodeExporter::doScene(const std::string &name) {
	if (!firstScene) {
		outfile << "\n";
	}
	firstScene = false;
}


void BBCodeExporter::doParagraph(const std::string &text) {
	std::string t = text;
	bool endOfList = false;

	trim(t);

	for (listLevel = 0; listLevel < t.size() &&
						(t[listLevel] == '*' || t[listLevel] == '-');
						++listLevel);

	if (listLevel > 0) {
		if (!inList) {
			inList = true;
		}
		t.erase(0, listLevel);
	} else if (inList) {
		// end list
		inList = false;
		endOfList = true;
	}


// 	We don't actually need to do these since they're the same in our input
//      and output
// 	t = doReplace(t, "\\[b\\](.*?)\\[\\/b\\]", "**$1**");
// 	t = doReplace(t, "\\[i\\](.*?)\\[\\/i\\]", "_$1_");

	// remove remaining control codes / comments
// 	while ( (pos = t.find_first_of('['), pos+1) != std::string::npos) {
// 		size_t end = t.find_first_of(']', pos);
// 		if (end == std::string::npos) {
// 			break;
// 		}
// 		t.erase(pos, end-pos + 1);
// 	}

	trim(t);
	if (!t.empty()) {
		if (inList) {
			for (size_t i = 1; i < listLevel; ++i) {
				outfile << "    ";
			}
			outfile << "- " << t << "\n";
		} else {
			if (endOfList) {
				outfile << "\n";
			}
			outfile << t << "\n\n";
		}
	}
}


void BBCodeExporter::end() {
	outfile.flush();
	if (outfile) {
		std::cout << "BBCode export saved to " << filename << ".\n\n";
	}
}
