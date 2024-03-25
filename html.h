#ifndef HTML_H
#define HTML_H

#include <memory>

#include <QString>
#include <QDebug>

#include "tagnames.h"

class LogsSt
{
public:
	static void Log(const QString &text) { qDebug() << text; }
	static void Warning(const QString &text) { qDebug() << text; }
	static void Error(const QString &text) { qDebug() << text; }
};

struct Attribute
{
	QString name;
	QString value;

	Attribute(QString name_, QString value_): name{name_}, value{value_} {}

	QString ToStr() { return "["+name+"="+value+"]"; }
};

struct Tag
{
	const QString &html;
	int startIndex;
	int endIndex;
	Tag *closerTag = nullptr;
	Tag *openerTag = nullptr;
	std::vector<Tag*> nestedOpenersTags;

	int type = undefined;
	QString name;
	std::vector<Attribute> attributes;

	Tag(const QString &html_, int startIndex_, int endIndex_): html{html_}, startIndex{startIndex_}, endIndex {endIndex_} { DecodeTag(); }

	void DecodeTag();

	QString GetDefinitionText() const { return html.mid(startIndex+1,endIndex-startIndex-1);}
	QString GetNestedText();
	QString ToStr();

	enum type { undefined, opener, closer, single };
	QString TypeToStr();
};

struct HTML
{
	QString html;
	std::vector<std::unique_ptr<Tag>> tags;

	void ParseTags();
	Tag* FindTag(QString name, std::vector<Attribute> attributes);

	QString TagsToStr();

	static QString& RemoveJungAndAddSpaces(QString &text, bool removeJung, bool addSpaces);
};



#endif // HTML_H
