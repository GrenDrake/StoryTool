# StoryTool

A StoryTool file is a form of minimally marked up text. The markup is divided into two broad categories: paragraph commands and text commands. The typical StoryTool file extensions is ".txt".

StoryTool is released under the [MIT license](LICENSE).


## History

StoryTool was created to assist in posting stories to a variety of websites. Each wanted submissions to be in a different format (BB Code, HTML, and Markdown being popular) so rather than reformat stories by hand, I wrote a JavaScript tool to do the task for me. At this point, the tool supported little more that basic BB Code-like markup.

Initially, it was little more than a basic BB Code to HTML converter. I quickly found that I wanted to add different kinds of formatting and utilities. The first special format was scene headings; although the headings themselves don't show up in the output, the scene breaks are (typically as a series of asterisks: `*  *  *`). I wanted it to be nicely formatted in the various output formats.

After this came the word count feature. This was designed to avoid including the words in headings or formatting commands. Later additions to the format included lists and named, auto-numbered chapters. By this point, however, the original JavaScript version had become an unpleasant tangle of code, so I rewrote it using C++.

## Usage

```
USAGE: storytools <filename> [-quiet] [<export_format> <export_file> [export options]]
```

StoryTool runs on the command line, taking the input file as the first argument. The `-quiet` argument (if given) will suppress the output of word count information. This can optionally be followed by the name of an exporter and the name of the output file. Some exporters can take an additional argument as well.


## Word Count

Unless the `-quiet` argument has been passed, StoryTool will produce a word count summary after processing a file. This will break the input file's word count down by chapter and scene, as well as providing the average length of a scene or chapter. The total word count is also displayed, both exactly and rounded to the nearest hundred, along with an estimate of the number of pages based on [the numbers given by Raven Book Design](http://www.fionaraven.com/estimate-your-page-count/) for fiction.

## Document Export

StoryTool is capable of exporting documents into a variety of formats as listed in the table below. HTML formats take an extra, optional argument providing the name of a page template to use.

| **Argument Name** | **Format** | **Description** |
|-------------------|------------|-----------------|
| bare     | Bare text     | Plain text with no formatting, headers, or metadata. |
| html     | HTML          | HTML page with optional template. |
| htmlsep  | HTML          | Export as a seperate HTML page for each chapter with optional template. |
| markdown | Markdown      | Translate into markdown format. |
| latex    | LaTeX Source  | Translate into latex format. |
| rtf      | .RTF File     | Export as RTF document. |
| bbcode   | BB Code       | Export as BBCode. |


## Formatting Commands

A modern StoryTool document is formatted using two kinds of commands: paragraph and text. Paragraph commands are always the first thing on a line (not including leading whitespace) and they begin with a slash (/). Text commands occur inside of a paragraph and always have a scope limited to that paragraph.

Every newline is considered to start a paragraph and leading and trailing whitespace is always trimmed. Sequences of whitespace characters are reduced to a single space. Blank paragraphs are ignored.

Lines beginning with a percent sign (%) are considered comments and are not including in the output or in word counts.

There are no formatting commands required in a document; omitting them will result in StoryTool supplying default values.

### Paragraph Commands

A paragraph command consists of a forward slash immediately followed by a single word; any remaining text on the line is the parameter of the command.

The most important paragraph commands are `/scene` and `/chapter` which begin a new scene and chapter respectively. Starting a new chapter also begins a new scene. These take the name of the scene or chapter as a parameter. Chapter and scene names may be omitted in which case StoryTool will give them the name "Anonymous Chapter" and "Anonymous Scene".

There are three metadata commands: `/title`, `/author`, and `/date`. All three fields are completely free-form; no validation or processing is performed. If left out, they default to "Untitled Work" and "Anonymous" for the title and author while the date is simply omitted.

The `/include` command which will include the text from the file named in the parameter as though it had occurred at that point in the file. This command does not start a new scene or chapter, but will start a new paragraph. Be aware that include file paths are currently processed from the current working direction, not from the parent file's location.

### Text Commands

Bold and italic formatting is provided by surrounding the text with either the `\[b\]bold\[/b\]` or `\[i\]italic\[/i\]` tags. Tags must be closed in the same paragraph that they were opened in.

Bulleted lists are created by placing one or more hyphens (-) or asterisks (*) at the beginning of the line. The number of symbols indicates the depth of the list item. A sample list is included below.

```
- item one, first level
-- item one, second level
- item two, first level
```
