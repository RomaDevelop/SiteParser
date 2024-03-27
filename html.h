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
	Tag *nextTag = nullptr;
	Tag *prevTag = nullptr;
	Tag *nextOpener = nullptr;
	Tag *prevOpener = nullptr;

	int type = undefined;
	QString name;
	std::vector<Attribute> attributes;

	Tag(const QString &html_, int startIndex_, int endIndex_): html{html_}, startIndex{startIndex_}, endIndex {endIndex_} { ParseTagDefinition(); }
	void ParseTagDefinition();
//	Tag* FindCloser()
//	{
//		Tag* retPtr = nullptr;
//		if(type != opener)
//		{
//			if(nestedOpenersTags.back() && nestedOpenersTags.back()->closerTag && nestedOpenersTags.back()->closerTag->nextTag)
//			{
//				Tag* tmpTagPt = nestedOpenersTags.back()->closerTag->nextTag;
//				while(tmpTagPt->type == single)
//				{
//					if(tmpTagPt->nextTag)
//						tmpTagPt = tmpTagPt->nextTag;
//					else
//					{
//						LogsSt::Error("Tag::FindCloser error structrure 1");
//						break;
//					}
//				}
//				if(tmpTagPt->type == closer && tmpTagPt->name == name)
//				{
//					retPtr = tmpTagPt;
//				}
//				else LogsSt::Error("Tag::FindCloser error structrure 2");
//			}
//			else LogsSt::Error("Tag::FindCloser error structrure 3");
//		}
//		else LogsSt::Error("Tag::FindCloser tag is not opener " + GetTagInfo());
//		return retPtr;
//	}

	QString GetDefinitionText() const { return html.mid(startIndex+1,endIndex-startIndex-1);}
	QString GetNestedText();
	QString GetTagInfo();

	enum type { undefined, opener, closer, single };
	QString TypeToStr();
};

struct HTML
{
	QString html;
	std::vector<std::unique_ptr<Tag>> tags;

	void ParseTags();
	std::vector<Tag*> FindTags(QString name, std::vector<Attribute> attributes);

	QString TagsInfo();

	static QString& RemoveJungAndAddSpaces(QString &text, bool removeJung, bool addSpaces);
};



#endif // HTML_H
