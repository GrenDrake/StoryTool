/title  StoryTool - The text formatting tool
/author Gren Drake
/date   2016

/chapter History

A StoryTool file is a form of minimally marked up text. The markup is divided into two broad catagories: paragraph commands and text commands.

Originally StoryTool was designed as a tool to assist me in posting stories to a variety of websites. Each website wanted input to be in its own format (BB Code, HTML, and Markdown being popular) so rather than reformat stories by hand, I write a simple JavaScript tool to do the task for me.

Although the original was little more than a basic BB Code to HTML interpeter, I quickly found that I wanted to add different kinds of formatting. Perhaps the original special format was actually the scene headings; although the headings themselves don't show up in the output, the way the scene breaks were displayed did; at the time I used a series of asterisks (* * *) and wanted it to be nicely formatted in the HTML output even if BB Code didn't really support it.

After this came the word count feature which was designed to give the word count without taking into account the words in scene headings or formatting commands. Later additions included lists and named autonumbered chapters. By this point, however, the original JavaScript version had become quite a mess resulting in this much cleaner C++ rewrite.

/chapter Formatting Commands

/scene intro to formatting

A modern StoryTool document is formatted using two kinds of commands: paragraph commands and text commands. Paragraph commands can be identified because they are the first thing on the line and begin with a slash (/). Text commands occur inside of a paragraph and always have a scope limited to that paragraph. In addition there are a few "general" formatting items described immediately below.

Unlike many text formatting systems, every newline is considered to start a paragraph. In addition, whitespace is always trimmed from the beginning and end of paragraphs and sequences of multiple whitespace characters inside of a document are reduced to a single space. Blank paragraphs are ignored, however, and not included in the output so it's still safe to add blank lines for readablity.



/scene paragraph commands

A paragraph command consists of a forward slash immediately followed by a single word; any remaining text on the line is the paramater of the command.

The most important paragraph commands are [b]/scene[/b] and [b]/chapter[/b] which begin a new scene and chapter respectively; starting a new chapter also begins a new scene. These take the name of the scene or chapter as a paramater; while this is optional in both cases, it is recommended to include chapter names as the chapters will otherwise be named "Anonymous Chapter".

It is possible to include text without explicitly creating a scene or chapter; this may be useful for short documents. When this occurs, StoryTool will automatically create an annonymous chapter and scene.

There are also the metadata commands [b]/title[/b], [b]/author[/b], and [b]/date[/b] which set the metadata fields much as one might expect. Although I typically use a bare year for the date this is not required; the date is treated as a text string and can take any format one desires, though the writer should be aware that it will be placed after the author's name in the byline as can be seen at the top of this document.

There is also the [b]/include[/b] command which will include the text from the file named in the paramater as though it had occured at that point in the file. This command does not start a new scene or chapter, but will start a new paragraph.

Although not a paragraph command as such, if a percent sign (%) is the first character on the line, the line will be considered a comment and be included in neither the word count nor exported output.


/scene text commands

The most basic formatting commands are for [b]bold[/b] and [i]italic[/i]. These are specified by putting a [b]b[/b] or [b]i[/i] in square brackets before the text to be formatted and the same with a slash (/) after the open bracket after the text. Examples can be seen by viewing the source of this document (readme.st). Note that underlined text is not supported at this time.

Bulleted lists are also considered to be text formatting commands; these are created by placing one or more hyphons (-) or asterisks (*) at the beginning of the line. The number of symbols indicates the depth of the list item. A sample list is included below.

- item one, first level
- item two, first level
-- item one, second level
- item three, first level
