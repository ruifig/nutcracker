#include "NutcrackerPCH.h"
#include "FileEditorStyles.h"

namespace cz
{
namespace view
{

#define COLOUR_MULTIPLY(cl, mult) wxColour(int(cl.Red()*mult), int(cl.Green()*mult), int(cl.Blue()*mult))
const char* gWhiteSpaceColour = "#CCCCCC";

wxColour clDefaultBkg("#EEEEEE");
wxColour clCurrentLineBkg(COLOUR_MULTIPLY(clDefaultBkg, 0.95f));
wxColour clDarkerBkg(COLOUR_MULTIPLY(clDefaultBkg, 0.8f));
wxColour clInactiveCodeBkg(COLOUR_MULTIPLY(clDefaultBkg, 1.0f));
wxColour clStringBkg(COLOUR_MULTIPLY(clDefaultBkg, 1.0f));

wxColour clDefault("#000000");
wxColour clComment("#008000");
wxColour clString("#B8860B");
wxColour clPreprocessor("#9F1E1E");
wxChar* fntDefault = L"Courier New";
wxColour clKeyword("#4E2BFF");
wxColour clType("#1E839F");
wxColour clFunction("#880000");
wxColour clIdentifier("#000080");
wxColour clMacro("#7000B2");
wxColour clNumber(clMacro);
wxColour clInactiveCode("#808080");
wxColour clOperator("#0000AA");
wxColour clFlowControlKeyword("#0000FF");
wxColour clBraceHighlight("#0000AA");
wxColour clBraceBad("#FF0000");
int fntSizeDefault = 10;

namespace Styles
{
	StyleInfo LineNumber{ wxColour(75, 75, 75), wxColour(220, 220, 220), fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Default{ clDefault, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Comment{ clComment, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Keyword{ clKeyword, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Type{ clType, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Function{ clFunction, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Identifier{ clIdentifier, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Macro{ clMacro, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Number{ clNumber, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo InactiveCode{ clInactiveCode, clInactiveCodeBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo FlowControlKeywords{ clFlowControlKeyword, clDefaultBkg, fntDefault, fntSizeDefault, true, false, false };
	StyleInfo String{ clString, clStringBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo Preprocessor{ clPreprocessor, clDefaultBkg, fntDefault, fntSizeDefault, true, false, false };
	StyleInfo Operator{ clOperator, clDefaultBkg, fntDefault, fntSizeDefault, false, false, false };
	StyleInfo BraceHighlight{ clBraceHighlight, clDarkerBkg, fntDefault, fntSizeDefault, true, false, false };
	StyleInfo BraceBad{ clBraceBad, clDarkerBkg, fntDefault, fntSizeDefault, true, false, false };
}

const char* nutKeywords =
	"base break case catch class clone "\
	"continue const default delete else enum "\
	"extends for foreach function if in "\
	"local null resume return switch this "\
	"throw try typeof while yield constructor "\
	"instanceof true false static";

std::vector<LanguageInfo> gLanguages =
{
	{ "Squirrel", { "nut" }, wxSTC_LEX_CPP,
	{
		{wxSTC_C_DEFAULT				, &Styles::Default		, nullptr },
		{wxSTC_C_COMMENT				, &Styles::Comment		, nullptr },
		{wxSTC_C_COMMENTLINE			, &Styles::Comment		, nullptr },
		{wxSTC_C_COMMENTDOC				, &Styles::Comment		, nullptr },
		{wxSTC_C_NUMBER					, &Styles::Number		, nullptr },
		{wxSTC_C_WORD					, &Styles::Keyword		, nutKeywords },
		{wxSTC_C_STRING					, &Styles::String		, nullptr },
		{wxSTC_C_CHARACTER				, &Styles::Default		, nullptr },
		{wxSTC_C_UUID					, &Styles::Default		, nullptr },
		{wxSTC_C_PREPROCESSOR			, &Styles::Preprocessor	, nullptr },
		{wxSTC_C_OPERATOR				, &Styles::Operator		, nullptr },
		{wxSTC_C_IDENTIFIER				, &Styles::Default		, nullptr },
		{wxSTC_C_STRINGEOL				, &Styles::Default		, nullptr },
		{wxSTC_C_VERBATIM				, &Styles::Default		, nullptr },
		{wxSTC_C_REGEX					, &Styles::Default		, nullptr },
		{wxSTC_C_COMMENTLINEDOC			, &Styles::Comment		, nullptr },
		{wxSTC_C_WORD2					, &Styles::FlowControlKeywords , nullptr },
		{wxSTC_C_COMMENTDOCKEYWORD		, &Styles::Comment		, nullptr },
		{wxSTC_C_COMMENTDOCKEYWORDERROR	, &Styles::Comment		, nullptr },
		{wxSTC_C_GLOBALCLASS			, &Styles::Default		, nullptr },
		{wxSTC_C_STRINGRAW				, &Styles::Default		, nullptr },
		{wxSTC_C_TRIPLEVERBATIM			, &Styles::Default		, nullptr },
		{wxSTC_C_HASHQUOTEDSTRING		, &Styles::String		, nullptr },
		{wxSTC_C_PREPROCESSORCOMMENT	, &Styles::Comment		, nullptr }
		}
	}
};


/*
From scintilla documentation: http://www.scintilla.org/ScintillaDoc.html#StyleDefinition

STYLE_DEFAULT	32	This style defines the attributes that all styles receive when the SCI_STYLECLEARALL message is used.
STYLE_LINENUMBER	33	This style sets the attributes of the text used to display line numbers in a line number margin. The background colour set for this style also sets the background colour for all margins that do not have any folding mask bits set. That is, any margin for which mask & SC_MASK_FOLDERS is 0. See SCI_SETMARGINMASKN for more about masks.
STYLE_BRACELIGHT	34	This style sets the attributes used when highlighting braces with the SCI_BRACEHIGHLIGHT message and when highlighting the corresponding indentation with SCI_SETHIGHLIGHTGUIDE.
STYLE_BRACEBAD	35	This style sets the display attributes used when marking an unmatched brace with the SCI_BRACEBADLIGHT message.
STYLE_CONTROLCHAR	36	This style sets the font used when drawing control characters. Only the font, size, bold, italics, and character set attributes are used and not the colour attributes. See also: SCI_SETCONTROLCHARSYMBOL.
STYLE_INDENTGUIDE	37	This style sets the foreground and background colours used when drawing the indentation guides.
STYLE_CALLTIP	38	 Call tips normally use the font attributes defined by STYLE_DEFAULT. Use of SCI_CALLTIPUSESTYLE causes call tips to use this style instead. Only the font face name, font size, foreground and background colours and character set attributes are used.
STYLE_LASTPREDEFINED	39	To make it easier for client code to discover the range of styles that are predefined, this is set to the style number of the last predefined style. This is currently set to 39 and the last style with an identifier is 38, which reserves space for one future predefined style.
STYLE_MAX	255	This is not a style but is the number of the maximum style that can be set. Styles between STYLE_LASTPREDEFINED and STYLE_MAX would be appropriate if you used SCI_SETSTYLEBITS to set more than 5 style bits.
*/

std::vector<LanguageInfo::Style> gSharedStyles =
{
	{ wxSTC_STYLE_DEFAULT		,	&Styles::Default		, nullptr },
	{ wxSTC_STYLE_LINENUMBER	,	&Styles::LineNumber		, nullptr },
	{ wxSTC_STYLE_BRACELIGHT	,	&Styles::BraceHighlight	, nullptr },
	{ wxSTC_STYLE_BRACEBAD		,	&Styles::BraceBad		, nullptr },
	{ wxSTC_STYLE_CONTROLCHAR	,	&Styles::Default		, nullptr },
	{ wxSTC_STYLE_INDENTGUIDE	,	&Styles::Default		, nullptr },
	{ wxSTC_STYLE_CALLTIP		,	&Styles::Default		, nullptr }
};

std::vector<IndicatorStyle> gIndicatorStyles =
{
	// kINDIC_Warning
	{ wxSTC_INDIC_SQUIGGLE, "#BBBB00",255},
	// kINDIC_Error
	{ wxSTC_INDIC_SQUIGGLE, "#FF0000",255},
	// kINDIC_WordMatch
	{ wxSTC_INDIC_ROUNDBOX, "#000000",32}
};

void setStyle(wxStyledTextCtrl* ctrl, const LanguageInfo::Style& style, int* keywordsNum)
{
	wxFont font(style.info->fontsize, wxMODERN, wxNORMAL, wxNORMAL, false, style.info->fontname);

	ctrl->StyleSetFont(style.num, font);
	ctrl->StyleSetBold(style.num, style.info->bold);
	ctrl->StyleSetItalic(style.num, style.info->italic);
	ctrl->StyleSetUnderline(style.num, style.info->underline);
	ctrl->StyleSetForeground(style.num, style.info->foreground);
	ctrl->StyleSetBackground(style.num, style.info->background);
	if (style.words && keywordsNum)
		ctrl->SetKeyWords((*keywordsNum)++, style.words);
}

void setStyles(wxStyledTextCtrl* ctrl, const std::vector<LanguageInfo::Style>& styles)
{
	int keywordsNum = 0;
	for(auto &s : styles)
		setStyle(ctrl, s, &keywordsNum);
}

} // namespace view
} // namespace cz