#pragma once

namespace nutcracker
{

enum IndicatorNumber
{
	kINDIC_Warning=0,
	kINDIC_Error=1,
	kINDIC_WordMatch
};

struct IndicatorStyle
{
	int style;
	const char* colour;
	int alpha;
};

struct StyleInfo
{
	wxColour foreground;
	wxColour background;
	const wxChar *fontname;
	int fontsize;
	bool bold;
	bool italic;
	bool underline;
};

struct LanguageInfo
{
	std::string name;
	std::vector<const char*> fileextensions;
	int lexer;
	
	//
	// Each entry in this vector should match a lexer state according to Scintilla documentation
	//
	struct Style
	{
		int num;
		StyleInfo* info;
		const char* words;
	};
	std::vector<Style> styles;
};


extern const char* gWhiteSpaceColour;
extern std::vector<LanguageInfo> gLanguages;
extern std::vector<IndicatorStyle> gIndicatorStyles;
extern std::vector<LanguageInfo::Style> gSharedStyles;
extern wxColour clDefaultBkg;
extern wxColour clCurrentLineBkg;
void setStyle(wxStyledTextCtrl* ctrl, const LanguageInfo::Style& style, int* keywordsNum=nullptr);
void setStyles(wxStyledTextCtrl* ctrl, const std::vector<LanguageInfo::Style>& styles);

} // namespace nutcracker



