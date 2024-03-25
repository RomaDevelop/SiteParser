#include "html.h"

using namespace std;

#define QSn QString::number
#define qdbg qDebug()

void HTML::ParseTags()
{
	int size = html.size();
	bool quatsNow = false;
	int tegStart = -1;
	vector<Tag*> tagsOpeners;

	for(int i=0; i<size; i++)
	{
		if(!quatsNow && html[i] == '"') { quatsNow = true; continue; }
		if(quatsNow && html[i] == '"') { quatsNow = false; continue; }

		if(!quatsNow)
		{
			if(tegStart == -1 && html[i] == '<')
			{
				tegStart = i;
				continue;
			}

			if(tegStart != -1 && html[i] == '<')
				LogsSt::Error("HTML::ParseTegs: < nested in teg");

			if(tegStart != -1 && html[i] == '>')
			{
				tags.push_back(std::make_unique<Tag>(html, tegStart, i));
				if(tags.back()->type == Tag::opener || tags.back()->type == Tag::closer) tagsOpeners.push_back(tags.back().get());
				tegStart = -1;
			}
		}
	}

	for(uint i=0; i<tagsOpeners.size(); i++)
	{
		if(tagsOpeners[i]->type == Tag::opener)
		{
			for(uint j=i+1; j<tagsOpeners.size(); j++)
			{
				if(tagsOpeners[j]->type == Tag::opener)
					tagsOpeners[i]->nestedOpenersTags.push_back(tagsOpeners[j]);
				if(tagsOpeners[j]->type == Tag::closer && tagsOpeners[i]->name == tagsOpeners[j]->name)
				{
					tagsOpeners[i]->closerTag = tagsOpeners[j];
					tagsOpeners[j]->openerTag = tagsOpeners[i];
					break;
				}
			}

			if(!tagsOpeners[i]->closerTag)
				LogsSt::Error("HTML::ParseTags can't find closer for " + tagsOpeners[i]->ToStr());
		}
	}
}

QString HTML::TagsToStr()
{
	QString ret;
	for(auto &tag:tags)
	{
		ret += tag->ToStr() + "\n";
	}
	return ret;
}

Tag *HTML::FindTag(QString name, std::vector<Attribute> attributes)
{
	Tag *retTag = nullptr;
	bool findNotOne = false;
	for(auto &tag:tags)
	{
		if(tag->name == name)
		{

			bool check = true;
			for(uint i=0; i<attributes.size(); i++)
			{
				bool find = false;
				for(auto &attrib:tag->attributes)
				{
					auto attribValueOfTag = attrib.value;
					if(attrib.value.left(1) == '\"' && attrib.value.right(1) == '\"')
						attribValueOfTag = attrib.value.mid(1,attrib.value.length()-2);
					if(attrib.name == attributes[i].name
							&& attribValueOfTag == attributes[i].value)
						find = true;
				}
				if(!find) check = false;
			}
			if(check)
			{
				if(!retTag) retTag = tag.get();
				else findNotOne = true;
			}
		}
	}

	if(findNotOne) LogsSt::Warning("HTML::FindTag find tag find more than 1 tag");

	return retTag;
}

QString& HTML::RemoveJungAndAddSpaces(QString &text, bool removeJung, bool addSpaces)
{
	bool quatsNow = false;
	for(int i=text.size()-1; i>=0; i--)
	{
		if(!quatsNow && text[i] == '"') { quatsNow = true; }
		else if(quatsNow && text[i] == '"') { quatsNow = false; }

		if(!quatsNow)
		{
			if(removeJung && (text[i] == '\n' || text[i] == '\r' || text[i] == '\t'))
			{
				text.remove(i,1);
				continue;
			}

			if(removeJung && text[i] == ' ' && i+1 < text.size() && text[i+1] == ' ')
			{
				text.remove(i,1);
				continue;
			}

			if(addSpaces && text[i] == '=')
			{
				if(i+1 < text.size() && text[i+1] != ' ') text.insert(i+1,' ');
				if(i-1 >= 0 && text[i-1] != ' ') text.insert(i,' ');

				continue;
			}
		}
	}
	return text;
}

void Tag::DecodeTag()
{
	QString text = GetDefinitionText();

	HTML::RemoveJungAndAddSpaces(text, true, true);

	QStringList words;
	QString word;
	bool quatsNow = false;
	for(int i=0; i<text.size(); i++)
	{
		if(!quatsNow && text[i] == '"') { quatsNow = true; }
		else if(quatsNow && text[i] == '"') { quatsNow = false; }

		if(!quatsNow)
		{
			if(text[i] == ' ')
			{
				words += word;
				word = "";
				continue;
			}
		}

		word += text[i];
	}
	words += word;

	for(int i=0; i<words.size(); i++)
		if(words[i].size() == 0) LogsSt::Error("Teg::ParseTeg: empty word["+QSn(i)+"] in Teg " + ToStr());

	if(words.size() % 3 == 1)
	{
		if(words[0][0] == '/')
		{
			words[0].remove(0,1);
			type = closer;
		}
		else type = opener;

		name = words[0];
		if(TagNames::all.contains(name))
		{
			if(TagNames::needClose.contains(name)) { /* nothing to do */ }
			else if(TagNames::notNeedClose.contains(name))
			{
				type = single;
			}
			else LogsSt::Error("Teg::ParseTeg: name " + name + " not contains neither in TagNames::needClose:["
							   + TagNames::needClose.join(';') + "] nor in TagNames::notNeedClose:[" + TagNames::notNeedClose.join(';') + "]");
		}
		else LogsSt::Error("Teg::ParseTeg: name " + name + " not contains in TagNames::all:[" + TagNames::all.join(';') + "]");

		for(int i=1; i<words.size(); i+=3)
		{
			attributes.push_back(Attribute(words[i],words[i+2]));
		}
	}
	else
		LogsSt::Error("Teg::ParseTeg: wrong words.size() = " + QSn(words.size()) + " in Teg " + ToStr());
}

QString Tag::GetNestedText()
{
	if(closerTag)
	{
		return html.mid(endIndex+1,closerTag->startIndex-endIndex-1);
	}
	else LogsSt::Error("Tag::GetNestedText Tag has no closer tag " + ToStr());
	return "";
}

QString Tag::ToStr()
{
	QString ret;
	ret = "["+TypeToStr()+"]["+name+"] (["+QSn(startIndex)+"-"+QSn(endIndex)+"])";

	if(type == opener && closerTag) ret += " (closer:["+QSn(closerTag->startIndex)+"-"+QSn(closerTag->endIndex)+"])";
	if(type == closer && openerTag) ret += " (opener:["+QSn(openerTag->startIndex)+"-"+QSn(openerTag->endIndex)+"])";

	QString textDefinition = GetDefinitionText();
	HTML::RemoveJungAndAddSpaces(textDefinition,true,false);
	if(textDefinition.length()>63)
	textDefinition = textDefinition.left(30) + "..." + textDefinition.right(30);
	ret += "\n    Definition text:["+textDefinition+"]";

	for(uint i=0; i<attributes.size(); i++)
	{
		QString attributtext = attributes[i].ToStr();
		HTML::RemoveJungAndAddSpaces(attributtext,true,false);
		if(attributtext.length()>63)
		attributtext = attributtext.left(30) + "..." + attributtext.right(30);
		ret += "\n    Attribut:" + attributtext;
	}

	if(nestedOpenersTags.size()) ret += "\n    Nested openers:";
	for(auto nestedOpener:nestedOpenersTags) ret += "["+nestedOpener->name+"]["+QSn(nestedOpener->startIndex)+"-"+QSn(nestedOpener->endIndex)+"]";

	if(type == opener)
	{
		QString nestedText = GetNestedText();
		HTML::RemoveJungAndAddSpaces(nestedText,true,false);
		if(nestedText.length()>63)
		nestedText = nestedText.left(30) + "..." + nestedText.right(30);
		ret += "\n    NestedText:[" + nestedText + "]";
	}
	return ret;
}

QString Tag::TypeToStr()
{
	QString ret = "error";
	switch (type) {
	case undefined: ret = "undefined"; break;
	case opener: ret = "opener"; break;
	case closer: ret = "closer"; break;
	case single: ret = "single"; break;
	default: ret = "unknown";
	}
	return ret;
}
