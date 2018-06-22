
#include "exporter.h"

void LatexExporter::start(const Story &story) {
    this->story = &story;
    if (story.chapterCount() > 1) {
        outfile << "\\documentclass[12pt,letter]{book}\n";
    } else {
        outfile << "\\documentclass[12pt,letter]{article}\n";
    }
    outfile << "\\linespread{1.5}\n";
    outfile << "\\raggedright\n";
    outfile << "\\newcounter{scenecounter}\n";
    outfile << "\\setlength{\\parskip}{0.5cm plus4mm minus3mm}\n";
    outfile << "\\title{" << story.getMetadata("title") << "}\n";
    outfile << "\\author{" << story.getMetadata("author") << "}\n";
    outfile << "\\date{" << story.getMetadata("date") << "}\n";
    outfile << "\\begin{document}\n";
    outfile << "\\maketitle\n\n";
}


void LatexExporter::doChapter(const std::string &name) {
    if (story->chapterCount() > 1) {
        outfile << "\\chapter{" << name << "}\n\n";
    }
    firstScene = true;
}

void LatexExporter::doScene(const std::string &name) {
    outfile << "\n\\begin{center}\n";
    outfile << "\\stepcounter{scenecounter}\n";
    outfile << "\\textbf{\\arabic{scenecounter}: " << name << "}\n";
    outfile << "\\end{center}\n\n";
}


void LatexExporter::doParagraph(const std::string &text) {
    std::string t = text;
    size_t pos = 0;
    bool startOfList = false;
    bool endOfList = false;

    trim(t);

    for (listLevel = 0; listLevel < t.size() &&
                        (t[listLevel] == '*' || t[listLevel] == '-');
                        ++listLevel);

    if (listLevel > 0) {
        if (!inList) {
            inList = true;
            startOfList = true;
        }
        t.erase(0, listLevel);
    } else if (inList) {
        // end list
        inList = false;
        endOfList = true;
    }


    t = doReplace(t, "%", "\\%");
    t = doReplace(t, "(\\s)'", "$1`");
    t = doReplace(t, "^'", "`");
    t = doReplace(t, "(\\s)\"", "$1``");
    t = doReplace(t, "^\"", "``");
    t = doReplace(t, "\"", "''");
    t = doReplace(t, "\\[b\\](.*?)\\[\\/b\\]", "\\textbf{$1}");
    t = doReplace(t, "\\[i\\](.*?)\\[\\/i\\]", "\\emph{$1}");

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
            // TODO nested lists
            if (startOfList) {
                outfile << "\\begin{itemize}\n";
            }
            outfile << "\\item " << t << "\n";
        } else {
            if (endOfList) {
                outfile << "\\end{itemize}\n\n";
            }
            outfile << t << "\n\n";
        }
    }
}


void LatexExporter::end() {
    if (inList) {
        outfile << "\\end{itemize}\n\n";
    }
    outfile << "\n\\end{document}\n";
    outfile.flush();
    if (outfile) {
        std::cout << "Latex export saved to " << filename << ".\n\n";
    }
}
