#ifndef EXPORTER_H
#define EXPORTER_H

#include <sstream>
#include <string>

#include "story.h"

class BareExporter : public StoryExporter {
public:
	BareExporter(const std::string &filename)
		: filename(filename), outfile(filename)
	{ }

	bool virtual good() const {
		return outfile.good();
	}

	void virtual start(const Story &story);
	void virtual doChapter(const std::string &name);
	void virtual doScene(const std::string &name);
	void virtual doParagraph(const std::string &text);
	void virtual end();
private:
	std::string filename;
	std::ofstream outfile;
};

class MarkdownExporter : public StoryExporter {
public:
	MarkdownExporter(const std::string &filename)
		: chapterNo(1), story(nullptr), filename(filename), outfile(filename),
		firstScene(true), inList(false), listLevel(0)
	{ }

	bool virtual good() const {
		return outfile.good();
	}

	void virtual start(const Story &story);
	void virtual doChapter(const std::string &name);
	void virtual doScene(const std::string &name);
	void virtual doParagraph(const std::string &text);
	void virtual end();
private:
	int chapterNo;
	const Story *story;
	std::string filename;
	std::ofstream outfile;
	bool firstScene;
	bool inList;
	size_t listLevel;
};

class RTFExporter : public StoryExporter {
public:
	RTFExporter(const std::string &filename)
		: chapterNo(1), story(nullptr), filename(filename), outfile(filename),
		firstScene(true), inList(false), listLevel(0)
	{ }

	bool virtual good() const {
		return outfile.good();
	}

	void virtual start(const Story &story);
	void virtual doChapter(const std::string &name);
	void virtual doScene(const std::string &name);
	void virtual doParagraph(const std::string &text);
	void virtual end();
private:
	int chapterNo;
	const Story *story;
	std::string filename;
	std::ofstream outfile;
	bool firstScene;
	bool inList;
	size_t listLevel;
};

class BaseHTMLExporter : public StoryExporter {
public:
	BaseHTMLExporter()
		: story(nullptr), inList(false), lastListLevel(0)
	{ }

	bool virtual good() const {
		return true;
	}

	void virtual start(const Story &story);
	void virtual doScene(const std::string &name);
	void virtual doParagraph(const std::string &text);
protected:
	std::stringstream htmlout;
	bool firstScene;
	const Story *story;
	bool inList;
	size_t lastListLevel;
};

class HTMLSingleExporter : public BaseHTMLExporter {
public:
	HTMLSingleExporter(const std::string &filename, const std::string &templateFilename)
		: chapterNo(1), filename(filename), templateFilename(templateFilename)
	{ }

	void virtual doChapter(const std::string &name);
	void virtual end();
private:
	int chapterNo;
	std::string filename;
	std::string templateFilename;
};

class HTMLMultipleExporter : public BaseHTMLExporter {
public:
	HTMLMultipleExporter(const std::string &filename, const std::string &templateFilename)
		: chapterNo(1), filename(filename), templateFilename(templateFilename)
	{ }

	void virtual doChapter(const std::string &name);
	void virtual end();
	void writeContent();
private:
	int chapterNo;
	std::string filename;
	std::string templateFilename;
	std::string templateText;
	std::string chapterName;
};

class BBCodeExporter : public StoryExporter {
public:
	BBCodeExporter(const std::string &filename)
		: chapterNo(1), story(nullptr), filename(filename), outfile(filename),
		firstScene(true), inList(false), listLevel(0)
	{ }

	bool virtual good() const {
		return outfile.good();
	}

	void virtual start(const Story &story);
	void virtual doChapter(const std::string &name);
	void virtual doScene(const std::string &name);
	void virtual doParagraph(const std::string &text);
	void virtual end();
private:
	int chapterNo;
	const Story *story;
	std::string filename;
	std::ofstream outfile;
	bool firstScene;
	bool inList;
	size_t listLevel;
};


#endif // EXPORTER_H
