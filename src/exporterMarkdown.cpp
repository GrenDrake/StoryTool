
#include "exporter.h"

void MarkdownExporter::start(const Story &story) {
	this->story = &story;
	outfile << "# " << story.getMetadata("title") << "\n\n";
	outfile << "_" << story.getMetadata("author") << ", " << story.getMetadata("date") << "_\n\n";
}


void MarkdownExporter::doChapter(const std::string &name) {
	if (story->chapterCount() > 1) {
		outfile << "## Chapter " << (chapterNo++) << ": " << name << "\n\n";
	}
	firstScene = true;
}

void MarkdownExporter::doScene(const std::string &name) {
	if (!firstScene) {
		outfile << "&nbsp;\n\n";
	}
	firstScene = false;
}


void MarkdownExporter::doParagraph(const std::string &text) {
	std::string t = text;
	size_t pos = 0;
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


	t = doReplace(t, "\\*", "\\*");
	t = doReplace(t, "\\_", "\\_");
	t = doReplace(t, "<", "&lt;");
	t = doReplace(t, "<", "&gt;");
	t = doReplace(t, "\\[b\\](.*?)\\[\\/b\\]", "**$1**");
	t = doReplace(t, "\\[i\\](.*?)\\[\\/i\\]", "_$1_");

	// remove remaining control codes / comments
	while ( (pos = t.find_first_of('[')) != std::string::npos) {
		size_t end = t.find_first_of(']', pos);
		if (end == std::string::npos) {
			break;
		}
		t.erase(pos, end-pos + 1);
	}

	trim(t);
	if (!t.empty()) {
		if (inList) {
			outfile << "- " << t << "\n";
		} else {
			if (endOfList) {
				outfile << "\n";
			}
			outfile << t << "\n\n";
		}
	}
}


void MarkdownExporter::end() {
	outfile << "}";
	outfile.flush();
	if (outfile) {
		std::cout << "Markdown export saved to " << filename << ".\n\n";
	}
}
