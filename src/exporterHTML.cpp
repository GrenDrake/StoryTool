#include "exporter.h"



void BaseHTMLExporter::start(const Story &story) {
	this->story = &story;
}

void BaseHTMLExporter::doScene(const std::string &name) {
	if (!firstScene) {
		htmlout << "\n<p>&nbsp;\n\n";
	}
	firstScene = false;
}


void BaseHTMLExporter::doParagraph(const std::string &text) {
	std::string t = text;
	size_t pos = 0;
	bool endOfList = false;
	int listLevel;

	trim(t);

	for (listLevel = 0; listLevel < t.size() &&
						(t[listLevel] == '*' || t[listLevel] == '-');
						++listLevel)
	{ }

	while (listLevel > lastListLevel) {
		htmlout << "<ul>\n";
		++lastListLevel;
	}
	while (listLevel < lastListLevel) {
		htmlout << "</ul>\n";
		--lastListLevel;
	}

	lastListLevel = listLevel;

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


	t = doReplace(t, "<", "&lt;");
	t = doReplace(t, "<", "&gt;");
	t = doReplace(t, "---", "&mdash;");
	t = doReplace(t, "--", "&ndash;");
	t = doReplace(t, "\\[b\\](.*?)\\[\\/b\\]", "<b>$1</b>");
	t = doReplace(t, "\\[i\\](.*?)\\[\\/i\\]", "<i>$1</i>");

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
			htmlout << "<li>" + t + "\n";
		} else {
			htmlout << "<p>" + t + "\n";
		}
	}
}


void HTMLSingleExporter::doChapter(const std::string &name) {
	if (story->chapterCount() > 1) {
		htmlout << "\n<h2 id='chapter" << chapterNo << "'>Chapter ";
		htmlout << chapterNo << ": " << name + "</h2>\n\n";
//				out = "<div class='chapter'>";
		++chapterNo;
	}
	firstScene = true;
}

void HTMLSingleExporter::end() {
	std::string result;

	if (!templateFilename.empty()) {
		std::cout << "Using HTML page template: " << templateFilename << "\n";
		std::ifstream templateFile(templateFilename);
		if (!templateFile) {
			std::cerr << "Could not open HTML template file " << templateFilename << "\n";
			return;
		}
		// load template
		std::stringstream ss;
		ss << templateFile.rdbuf();
		result = ss.str();
	}

	std::stringstream tocText;
	if (story->chapterCount() > 1) {
		int cNum = 1;
		tocText << "<ul>\n";
		story->eachChapter([ &cNum, &tocText ](const Chapter *c) {
			tocText << "<li><a href='#chapter" << cNum << "'>Chapter " << cNum << ": " << c->getName() << "</a>\n";
			++cNum;
		});
		tocText << "</ul>";
	}

	if (!result.empty()) {
		result = doReplace(result, "\\{title\\}", story->getMetadata("title"));
		result = doReplace(result, "\\{author\\}", story->getMetadata("author"));
		result = doReplace(result, "\\{date\\}", story->getMetadata("date"));
		result = doReplace(result, "\\{toc\\}", tocText.str());
		result = doReplace(result, "\\{body\\}", htmlout.str());
	} else {
		result = htmlout.str();
	}

	std::ofstream outfile(filename);
	if (!outfile) {
		std::cerr << "Could not open output file for writing.\n";
		return;
	}
	outfile << result;
	outfile.flush();
	std::cout << "HTML export saved to " << filename << ".\n";
}



void HTMLMultipleExporter::doChapter(const std::string &name) {
	if (!htmlout.str().empty()) {
		writeContent();
		++chapterNo;
	}
	firstScene = true;
	chapterName = name;
}

void HTMLMultipleExporter::end() {
	writeContent();
}

void HTMLMultipleExporter::writeContent() {
	std::string result;

	if (!templateFilename.empty() && templateText.empty()) {
		std::cout << "Using HTML page template: " << templateFilename << "\n";
		std::ifstream templateFile(templateFilename);
		if (!templateFile) {
			std::cerr << "Could not open HTML template file " << templateFilename << "\n";
			return;
		}
		// load template
		std::stringstream ss;
		ss << templateFile.rdbuf();
		templateText = ss.str();
	}
	result = templateText;

	if (!result.empty()) {
		result = doReplace(result, "\\{title\\}", chapterName);
		result = doReplace(result, "\\{author\\}", story->getMetadata("author"));
		result = doReplace(result, "\\{date\\}", story->getMetadata("date"));
		result = doReplace(result, "\\{toc\\}", "");
		result = doReplace(result, "\\{body\\}", htmlout.str());
	} else {
		result = htmlout.str();
	}
	htmlout.clear();
	htmlout.str("");

	std::string thisFile = doReplace(filename, "\\{\\}", std::to_string(chapterNo));

	std::ofstream outfile(thisFile);
	if (!outfile) {
		std::cerr << "Could not open output file for writing.\n";
		return;
	}
	outfile << result;
	outfile.flush();
	std::cout << "HTML export (chapter " << chapterNo << ") saved to " << thisFile << ".\n";
}
