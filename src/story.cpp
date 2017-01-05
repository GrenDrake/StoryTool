#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "story.h"

std::string &trim(std::string &s) {
	if (s.empty()) {
		return s;
	}

	size_t pos = s.size() - 1;
	while (pos > 0 && isspace(s[pos])) {
		--pos;
	}
	s.resize(pos + 1);

	pos = 0;
	while (pos < s.size() && isspace(s[pos])) {
		++pos;
	}
	s.erase(0,pos);

	return s;
}

std::string trim(const std::string &s) {
	std::string realStr = s;
	return trim(realStr);
}


int Paragraph::wordcount() const {
	int wc = 0;
	bool inCommand = false;
	bool inWord = true;
	for (char c : text) {
		if (inCommand) {
			if (c == ']') {
				inCommand = false;
			}
		} else if (c == '[') {
			inCommand = true;
		} else if (isspace(c)) {
			if (inWord) {
				inWord = false;
				++wc;
			}
		} else {
			if (isalnum(c)) {
				inWord = true;
			}
		}
	}
	if (inWord) {
		++wc;
	}
	return wc;
}

void Story::fromFile(const std::string &filename) {
	scene = nullptr;
	chapter = nullptr;
	fromFileHelper(filename);
}

void Story::fromFileHelper(const std::string &filename) {
	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Could not open \"" << filename << "\"\n";
		return;
	}

	if (infile.peek() == 0xEF) {
		infile.ignore();
		if (infile.get() != 0xBB || infile.get() != 0xBF) {
			std::cerr << "Story file " << filename << " has bad encoding format.\n";
			return;
		}
	}

	std::string inputline;
	int lineNo = 0;
	while (getline(infile, inputline)) {
		++lineNo;

		// skip empty lines and strip whitespace
		trim(inputline);
		if (inputline.empty()) {
			continue;
		}


		// process paragraph formats
		if (inputline[0] == '/') {
			size_t pos = inputline.find_first_of(' ');
			std::string value;
			if (pos != std::string::npos) {
				value = trim(inputline.substr(pos));
				inputline.resize(pos);
			}
            
            // handle include commands
            if (inputline == "/include") {
                std::cerr << "including \"" << value << "\" from \"" << filename << "\"\n";
                fromFileHelper(value);
                continue;
            }
			
			// handle metadata commands
			if (inputline == "/title" || inputline == "/author" || inputline == "/date") {
				setMetadata(inputline.substr(1), value);
				continue;
			}
			
			// process chapter boundaries
			if (inputline == "/chapter") {
				chapter = new Chapter(filename, lineNo, value);
				addChapter(chapter);
				scene = nullptr;
				continue;
			}

			// process scene boundaries
			if (inputline == "/scene" || inputline == "/todoscene") {
				if (chapter == nullptr) {
					chapter = new Chapter(filename, lineNo, "Anonymous Chapter");
					addChapter(chapter);
				}
				scene = new Scene(filename, lineNo, trim(value), (inputline == "/todoscene") );
				chapter->addScene(scene);
				continue;
			}
			
			// unknown paragraph command
			std::cerr << "Unknown paragraph command on line " << lineNo << " of file " << filename << ".\n";
			continue;
		}

		// process scene notes
		if (inputline[0] == '%') {
			// currently we do nothing we these
			continue;
		}

		// add paragraph text
		if (chapter == nullptr) {
			chapter = new Chapter(filename, lineNo, "Anonymous Chapter");
			addChapter(chapter);
		}
		if (scene == nullptr) {
			scene = new Scene(filename, lineNo, "Anonymous Scene");
			chapter->addScene(scene);
		}
		scene->addParagraph(new Paragraph(lineNo, inputline));
	}
	infile.close();
	finish();
}

void Story::displayInfo() const {
	// dispaly story information
	size_t width = getLongestName();
	const int wordCountWidth = 8;
	std::string title = getMetadata("title");
	std::string byline = getMetadata("author");
	std::string date = getMetadata("date");

	int roundedWords = wordcount();
	if ((roundedWords % 100) < 50) {
		roundedWords = roundedWords / 100 * 100;
	} else {
		roundedWords = (roundedWords / 100 + 1) * 100;
	}

	if (title.empty())	title = "Untitled Work";
	if (byline.empty())	byline = "Anonymous";
	std::cout << title << "\n" << byline;
	if (!date.empty()) {
		std::cout << ", " << date;
	}
	std::cout << "\n\n";

	for (const Chapter *c : chapters) {
		if (chapters.size() > 1) {
			std::cout << std::setw(width) << c->getName() << "  ";
			std::cout << std::setw(wordCountWidth) << c->wordcount() << " words";
			std::cout << ", line #" << c->getLine() << ", ";
            std::cout << c->getSourceFile() << "\n";
			for (size_t i = 0; i < width; ++i) {
				std::cout << "-";
			}
			std::cout << "\n";
		}

		c->eachScene([ width ](const Scene *s) {
			std::cout << std::setw(width) << s->getName() << "  ";
			if (s->getPredictive()) {
				std::cout << std::setw(wordCountWidth) << "-";
			} else {
				std::cout << std::setw(wordCountWidth) << s->wordcount();
			}
			std::cout << " words, line #" << s->getLine() << ", ";
            std::cout << s->getSourceFile() << "\n";
		});

		std::cout << "\n";
	}

	std::cout << "\n" << std::setw(width) << "Statistics" << "\n";
	for (size_t i = 0; i < width; ++i) {
		std::cout << "-";
	}
	std::cout << "\n";
	std::cout << std::setw(width) << "Average Chapter" << "  ";
	std::cout << std::setw(wordCountWidth) << averageChapter() << " words\n";
	std::cout << std::setw(width) << "Average Scene" << "  ";
	std::cout << std::setw(wordCountWidth) << averageScene() << " words\n\n";

	std::cout << std::setw(width) << "(Est.) Pages" << "  ";
	std::cout << std::setw(wordCountWidth) << estimatePageCount() << " pages\n";
	std::cout << std::setw(width) << "Total" << "  ";
	std::cout << std::setw(wordCountWidth) << wordcount() << " words\n";
	std::cout << std::setw(width) << "Rounded" << "  ";
	std::cout << std::setw(wordCountWidth) << roundedWords << " words\n\n";
}




void Scene::doExport(StoryExporter *exporter) const {
	exporter->doScene(getName());

	for (auto p = paragraphs.begin(); p != paragraphs.end(); ++p) {
		exporter->doParagraph((*p)->getText());
	}
}

void Chapter::doExport(StoryExporter *exporter) const {
	exporter->doChapter(getName());

	for (auto s = scenes.begin(); s != scenes.end(); ++s) {
		if (!(*s)->getPredictive()) {
			(*s)->doExport(exporter);
		}
	}
}

void Story::doExport(StoryExporter *exporter) const {
	exporter->start(*this);
	for (const Chapter *c : chapters) {
		c->doExport(exporter);
	}
	exporter->end();
}


void Chapter::averageSceneHelper(int &wc, int &sc) const {
	for (auto s = scenes.begin(); s != scenes.end(); ++s) {
		if (!(*s)->getPredictive()) {
			++sc;
			wc += (*s)->wordcount();
		}
	}
}
int Story::averageScene() const {
	int wc = 0, sc = 0;
	for (const Chapter *c : chapters) {
		c->averageSceneHelper(wc, sc);
	}
	return (wc / sc);
}

int Story::averageChapter() const {
	int wc = 0;
	for (const Chapter *c : chapters) {
		wc += c->wordcount();
	}
	return (wc / chapters.size());
}

