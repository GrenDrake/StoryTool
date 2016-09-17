
#include "exporter.h"

void RTFExporter::start(const Story &story) {
	this->story = &story;
	outfile << "{\\rtf1\\ansi\\deff0 {\\fonttbl {\\f0 Times New Roman;}}\n";
	outfile << "\\deflang2057\\plain\\fs24\\widowctrl\\hyphauto\n";
	outfile << "{\\header \\pard\\qr\\plain\\f0\\fs16 \\chpgn \\par}\n";
	outfile << "{\\info\n";
	outfile << "{\\title " << story.getMetadata("title") << "}\n";
	outfile << "{\\author " << story.getMetadata("author") << "}\n";
	outfile << "{\\company (none)}}\n";
	outfile << "{\\pard\\sa45\\fs48\\b " << story.getMetadata("title") << "\\par}\n";
	outfile << "{\\pard\\sa270\\fs32\\i " << story.getMetadata("author");
	outfile << ", " << story.getMetadata("date") << "\\par}\n";
}


void RTFExporter::doChapter(const std::string &name) {
	if (story->chapterCount() > 1) {
		outfile << "{\\pard";
		if (chapterNo != 1) {
			outfile << "\\sb90";
		}
		outfile << "\\sa90\\fs36 Chapter " << (chapterNo++) << ": " << name << "\\par}\n";
	}
	firstScene = true;
}

void RTFExporter::doScene(const std::string &name) {
	if (!firstScene) {
		outfile << "{\\pard\\sa180\\fi720\\qc";
		outfile << " *  *  *\\par}\n";
	}
	firstScene = false;
}


void RTFExporter::doParagraph(const std::string &text) {
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


	t = doReplace(t, "\\[b\\](.*?)\\[\\/b\\]", "{\\\\b $1}");
	t = doReplace(t, "\\[i\\](.*?)\\[\\/i\\]", "{\\\\i $1}");

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
			outfile << "{\\pard";
			outfile << "\\li";
			outfile << 720 * (listLevel - 1);
			outfile << "\\bullet  " << t << "\\par}\n";
		} else {
			outfile << "{\\pard\\sa180" << (endOfList ? "\\sb180" : "") << "\\fi720";
			outfile << " " << t << "\\par}\n";
		}
	}
}


void RTFExporter::end() {
	outfile << "}";
	outfile.flush();
	if (outfile) {
		std::cout << "RTF export saved to " << filename << ".\n\n";
	}
}
