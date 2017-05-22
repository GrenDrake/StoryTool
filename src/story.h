#ifndef STORY_H
#define STORY_H

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <string>

class Story;

class StoryExporter {
public:
	virtual ~StoryExporter() {}

	bool virtual good() const = 0;
	void virtual start(const Story &story) = 0;
	void virtual doChapter(const std::string &name) = 0;
	void virtual doScene(const std::string &name) = 0;
	void virtual doParagraph(const std::string &text) = 0;
	void virtual end() = 0;
};



class Paragraph {
public:
	Paragraph(int lineNo, std::string text = "")
		: line(lineNo), text(text)
	{ }
	void setText(const std::string &newText) {
		text = newText;
	}
	const std::string &getText() const {
		return text;
	}
	int getLine() const {
		return line;
	}
	int wordcount() const;

private:
	int line;
	std::string text;
};

class Scene {
public:
	Scene(std::string sourceFile, int lineNo, const std::string &name, bool predict = false)
		: sourceFile(sourceFile), predictive(predict), line(lineNo), name(name)
	{ }
	void addParagraph(const Paragraph *newPara) {
		if (!predictive) {
			paragraphs.push_back(newPara);
		}
	}
	size_t paragraphCount() const {
		return paragraphs.size();
	}

	const std::string &getName() const {
		return name;
	}
	const std::string& getSourceFile() const {
		return sourceFile;
	}
	int getLine() const {
		return line;
	}
	bool getPredictive() const {
		return predictive;
	}

	int wordcount() const {
		int wc = 0;
		for (const Paragraph *p : paragraphs) {
			wc += p->wordcount();
		}
		return wc;
	}

	void doExport(StoryExporter *exporter) const;
private:
	std::string sourceFile;
	bool predictive;
	int line;
	std::string name;
	std::vector<const Paragraph*> paragraphs;
};

class Chapter {
public:
	Chapter(std::string sourceFile, int lineNo, const std::string &name)
		: sourceFile(sourceFile), longestSceneName(0), line(lineNo), name(name)
	{ }

	void addScene(const Scene *newScene) {
		if (newScene->getName().size() > longestSceneName) {
			longestSceneName = newScene->getName().size();
		}
		scenes.push_back(newScene);
	}
	size_t sceneCount() const {
		return scenes.size();
	}

	template<typename Functor>
	void eachScene(Functor f) const {
		std::for_each(scenes.begin(), scenes.end(), f);
	}

	const std::string &getName() const {
		return name;
	}
	const std::string& getSourceFile() const {
		return sourceFile;
	}
	int getLine() const {
		return line;
	}
	size_t getLongestSceneName() const {
		return longestSceneName;
	}

	int wordcount() const {
		int wc = 0;
		for (const Scene *s : scenes) {
			wc += s->wordcount();
		}
		return wc;
	}

	void averageSceneHelper(int &wc, int &sc) const;

	void doExport(StoryExporter *exporter) const;
private:
	std::string sourceFile;
	size_t longestSceneName;
	int line;
	std::string name;
	std::vector<const Scene*> scenes;
};

class Story {
public:
	Story()
		// wordsPerPage - 390 for 5.5"x8.5", 475 for 6"x9"
		// http://www.fionaraven.com/estimate-your-page-count/
		: wordsPerPage(390), longestChapterName(0), longestSceneName(0)
	{ }
	bool fromFile(const std::string &filename);
	void displayInfo() const;
	void doExport(StoryExporter *exporter) const;

	void addChapter(const Chapter *newChapter) {
		if (newChapter->getName().size() > longestChapterName) {
			longestChapterName = newChapter->getName().size();
		}
		chapters.push_back(newChapter);
	}
	size_t chapterCount() const {
		return chapters.size();
	}

	template<typename Functor>
	void eachChapter(Functor f) const {
		std::for_each(chapters.begin(), chapters.end(), f);
	}

	size_t getLongestChapterName() const {
		return longestChapterName;
	}
	size_t getLongestSceneName() const {
		return longestSceneName;
	}
	size_t getLongestName() const {
		if (longestSceneName > longestChapterName) {
			return longestSceneName;
		}
		return longestChapterName;
	}

	int averageScene() const;
	int averageChapter() const;

	int wordcount() const {
		int wc = 0;
		for (const Chapter *c : chapters) {
			wc += c->wordcount();
		}
		return wc;
	}
	int estimatePageCount() const {
		int pc = 0;
		for (const Chapter *c : chapters) {
			pc += c->wordcount() / wordsPerPage;
			if (c->wordcount() % wordsPerPage) {
				++pc;
			}
		}

		return pc;
	}

	void setMetadata(const std::string &key, const std::string &value) {
		metadata[key] = value;
	}
	bool hasMetadata(const std::string &key) const {
		return (metadata.count(key) > 0);
	}
	const std::string &getMetadata(const std::string &key) const {
		static std::string emptyString;
		try {
			return metadata.at(key);
		} catch (std::out_of_range &e) {
			return emptyString;
		}
	}

	void finish() {
		for (const Chapter *c : chapters) {
			if (c->getLongestSceneName() > longestSceneName) {
				longestSceneName = c->getLongestSceneName();
			}
		}
	}
private:
	bool fromFileHelper(const std::string &filename);

	int wordsPerPage;

	size_t longestChapterName;
	size_t longestSceneName;
	std::vector<const Chapter*> chapters;
	std::map<std::string,std::string> metadata;

	// these are used for the story loading process
	Scene *scene;
	Chapter *chapter;
};

std::string &trim(std::string &s);
std::string trim(const std::string &s);
std::string doReplace(const std::string &source, const std::string &pattern,
					  const std::string &result);


#endif // STORY_H
