#include <algorithm>

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/minifram.h>

#include "wex/codeedit.h"
#include "wex/utils.h"

enum { ID_FIND_TEXT = wxID_HIGHEST+1, ID_FIND_NEXT,
	ID_REPLACE_TEXT, ID_REPLACE_NEXT, ID_REPLACE_ALL };

class FRDialog : public wxFrame
{
	wxCodeEditCtrl *m_editor;
	wxTextCtrl *m_findText, *m_replaceText;
	wxCheckBox *m_matchCase, *m_wholeWord;
	wxStaticText *m_replaceLabel;
	wxButton *m_replaceNextButton, *m_replaceAllButton;

public:
	FRDialog( wxCodeEditCtrl *parent )
		: wxFrame( parent, wxID_ANY, "Find & Replace", wxDefaultPosition, wxDefaultSize,
			wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxCLOSE_BOX|wxCAPTION|wxSYSTEM_MENU )
	{
		SetBackgroundColour( *wxWHITE );
		m_editor = parent;

		m_findText = new wxTextCtrl( this, ID_FIND_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER  );
		m_replaceText = new wxTextCtrl( this, ID_REPLACE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER   );
		m_matchCase = new wxCheckBox( this, wxID_ANY, "Match case" );
		m_wholeWord = new wxCheckBox( this, wxID_ANY, "Whole word" );
		
		
		wxFlexGridSizer *find_sizer = new wxFlexGridSizer( 3, 0, 0 );
		find_sizer->AddGrowableCol(1);
		find_sizer->Add( new wxStaticText( this, wxID_ANY, "   Search for:"), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
		find_sizer->Add( m_findText, 0, wxALL|wxEXPAND, 3 );
		find_sizer->Add( new wxButton( this, ID_FIND_NEXT, "Find next" ), 0, wxALL|wxEXPAND, 3 );
				
		find_sizer->Add( m_replaceLabel = new wxStaticText( this, wxID_ANY, "   Replace with:"), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
		find_sizer->Add( m_replaceText, 0, wxALL|wxEXPAND, 3 );
		find_sizer->Add( m_replaceNextButton = new wxButton( this, ID_REPLACE_NEXT, "Replace next" ), 0, wxALL|wxEXPAND, 3 );
		
		find_sizer->Add( m_matchCase, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		find_sizer->Add( m_wholeWord, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		find_sizer->Add( m_replaceAllButton = new wxButton( this, ID_REPLACE_ALL, "Replace all" ), 0, wxALL|wxEXPAND, 3 );

		SetSizer( find_sizer );
		Fit();
	
	}

	void OnCharHook( wxKeyEvent &evt )
	{
		if ( evt.GetKeyCode() == WXK_ESCAPE )
			Hide();
		else
			evt.Skip();
	}

	void FindNext()
	{
		m_editor->FindNext( m_findText->GetValue(), -1,
			m_matchCase->GetValue(), m_wholeWord->GetValue() );
		m_editor->SetFocus();
	}

	void ReplaceNext()
	{
		m_editor->ReplaceNext( m_findText->GetValue(), m_replaceText->GetValue(), false,
			m_matchCase->GetValue(), m_wholeWord->GetValue() );	
		m_editor->SetFocus();	
	}

	void ReplaceAll()
	{
		m_editor->ReplaceAll( m_findText->GetValue(), m_replaceText->GetValue(),
			m_matchCase->GetValue(), m_wholeWord->GetValue() );	
	}

	void SelectAll()
	{
		m_findText->SetFocus();
		m_findText->SelectAll();		
	}

	void OnCommand( wxCommandEvent &evt )
	{
		switch( evt.GetId() )
		{
		case ID_FIND_TEXT: // handle enter press on text widget too
		case ID_FIND_NEXT:
			FindNext();
			break;
		
		case ID_REPLACE_TEXT: // handle enter press on text widget too
		case ID_REPLACE_NEXT:		
			ReplaceNext();
			break;
			
		case ID_REPLACE_ALL:
			ReplaceAll();
			break;
		}
	}

	void OnClose( wxCloseEvent &evt )
	{
		Hide();
	}

	DECLARE_EVENT_TABLE();

};

BEGIN_EVENT_TABLE( FRDialog, wxFrame )
	EVT_CLOSE( FRDialog::OnClose )
	EVT_CHAR_HOOK( FRDialog::OnCharHook )
	EVT_TEXT_ENTER( ID_FIND_TEXT, FRDialog::OnCommand )
	EVT_TEXT_ENTER( ID_REPLACE_TEXT, FRDialog::OnCommand )
	EVT_BUTTON( ID_FIND_NEXT, FRDialog::OnCommand )
	EVT_BUTTON( ID_REPLACE_NEXT, FRDialog::OnCommand )
	EVT_BUTTON( ID_REPLACE_ALL, FRDialog::OnCommand )
END_EVENT_TABLE()



static bool IsBrace( wxUniChar ch )
{
	return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
}

BEGIN_EVENT_TABLE (wxCodeEditCtrl, wxStyledTextCtrl)

    // stc
    EVT_STC_MARGINCLICK( wxID_ANY, wxCodeEditCtrl::OnMarginClick )
    EVT_STC_CHARADDED( wxID_ANY, wxCodeEditCtrl::OnCharAdded )
	EVT_STC_UPDATEUI( wxID_ANY, wxCodeEditCtrl::OnUpdateUI )
	
END_EVENT_TABLE()


#ifdef __WXMAC__
#define DEFAULT_FONT_SIZE 13
#define DEFAULT_FONT_FACE "Courier New"
#else
#ifdef __WXMSW___
	#define DEFAULT_FONT_SIZE 10
	#define DEFAULT_FONT_FACE "Consolas"
#else
	#define DEFAULT_FONT_SIZE 10
	#define DEFAULT_FONT_FACE "Courier New"
#endif
#endif

static char* CWordlist1 =
    "asm auto break case char const "
    "continue default delete do double else enum "
    "extern float for goto if inline int long "
    "mutable register "
    "return short signed sizeof static "
    "struct switch typedef "
    "union unsigned void volatile "
    "while";

static char* CppWordlist1 =
    "asm auto bool break case catch char class const const_cast "
    "continue default delete do double dynamic_cast else enum explicit "
    "export extern false float for friend goto if inline int long "
    "mutable namespace new operator private protected public register "
    "reinterpret_cast return short signed sizeof static static_cast "
    "struct switch template this throw true try typedef typeid "
    "typename union unsigned using virtual void volatile wchar_t "
    "while";

static char *TrnsysWordlist1 = 
	"version assign simulation tolerances limits unit type "
	"parameters inputs equations constants end width "
	"accelerate loop repeat dfq nocheck solver derivatives "
	"trace format nolist list map ";

static char *TrnsysWordlist2 =
	"abs acos and asin atan cos eql exp gt int or ln log lt max min mod "
	"not sin tan";

static char *TrnsysWordlist3 =
	"const time ";

static char *PythonWordlist1 = 
	"and assert break class continue def "
	"del elif else except exec finally "
	"for from global if import in "
	"is lambda not or pass print "
	"raise return try while yield";

static  char *LKWordlist1  =
	"if while for return exit break continue function const enum class else elseif define this typeof common true false null import ";


wxCodeEditCtrl::wxCodeEditCtrl( wxWindow *parent, int id, 
		const wxPoint &pos, const wxSize &size )
		: wxStyledTextCtrl( parent, id, pos, size, wxBORDER_NONE )
{
	m_frDialog = new FRDialog( this );
	m_lastFindPos = m_lastReplacePos = -1;

	m_callTipsEnabled = false;
	m_ctCaseSensitive = false;
	m_ctStart = '(';
	m_ctEnd = ')';
	
	SetScrollWidthTracking( true );
}

void wxCodeEditCtrl::SetLanguage( const wxString &fileName )
{
	SetLanguage( GetLanguage( fileName ) );
}

void wxCodeEditCtrl::SetLanguage( Language lang )
{
	// first, revert to standard style
	m_lang = NONE;
	
	SetStyleBits( 8 );
    SetLayoutCache( wxSTC_CACHE_PAGE );
	SetLexer( wxSTC_LEX_NULL );
		
    wxFont font ( DEFAULT_FONT_SIZE,
		wxFONTFAMILY_MODERN,
		wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL,
		false,
		DEFAULT_FONT_FACE );

	SetFont( font );
	StyleSetFont (wxSTC_STYLE_DEFAULT, font);
		
	wxFont fontslant( font );
	fontslant.SetStyle( wxFONTSTYLE_ITALIC );
		
	wxFont fontsmall( font );
	fontsmall.SetPointSize( fontsmall.GetPointSize() - 1 );
	
	SetViewEOL( false );
    SetIndentationGuides( false );
    SetEdgeMode( wxSTC_EDGE_NONE );
    SetViewWhiteSpace( wxSTC_WS_INVISIBLE );
    SetOvertype( false );
    SetReadOnly( false );
    SetWrapMode( wxSTC_WRAP_NONE );
	StyleSetForeground( wxSTC_STYLE_DEFAULT, *wxBLACK );
    StyleSetBackground( wxSTC_STYLE_DEFAULT, *wxWHITE );
    StyleSetForeground( wxSTC_STYLE_INDENTGUIDE, *wxLIGHT_GREY );
    SetFoldFlags(0);

    // set spaces and indentation
    SetTabWidth( 4 );
    SetUseTabs( true );
    SetTabIndents( true );
    SetBackSpaceUnIndents( true );
    SetIndent( 4 );
	SetEdgeColumn( 80 );
	SetEdgeColour( wxColour(255,187,187) );
	SetEdgeMode( wxSTC_EDGE_LINE );
    
    // set visibility
    SetVisiblePolicy (wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
    SetXCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
    SetYCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
	
	SetSelForeground( true, *wxWHITE );
	SetSelBackground( true, *wxBLACK );

    // markers
    MarkerDefine( wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_DOTDOTDOT, *wxBLACK, *wxBLACK);
    MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN, *wxBLACK, *wxBLACK);
    MarkerDefine( wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY,     *wxBLACK, *wxBLACK);
    MarkerDefine( wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_DOTDOTDOT, *wxBLACK, *wxWHITE);
    MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, *wxBLACK, *wxWHITE);
    MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY,     *wxBLACK, *wxBLACK);
    MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY,     *wxBLACK, *wxBLACK);
		
	CallTipUseStyle( 30 );
	wxFont fontnormal (*wxNORMAL_FONT) ;
	StyleSetFont( wxSTC_STYLE_CALLTIP, fontnormal );
	StyleSetForeground( wxSTC_STYLE_CALLTIP, *wxBLACK );
	StyleSetBackground( wxSTC_STYLE_CALLTIP, wxColour(247,240,210) );
		
	// set up line number margin
	SetMarginType( m_lineNumMarginId, wxSTC_MARGIN_NUMBER );
	StyleSetForeground( wxSTC_STYLE_LINENUMBER, wxColour(80,80,80) );
	StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour(230,230,230) );
    int lineNrMarginWidth = TextWidth (wxSTC_STYLE_LINENUMBER, _T("_99999"));
	SetMarginWidth( m_lineNumMarginId, lineNrMarginWidth );

	// breakpoint margin	
	MarkerDefine( m_markCircle, wxSTC_MARK_CIRCLE );
	MarkerDefine( m_markArrow, wxSTC_MARK_SHORTARROW );
	SetMarginType( m_breakpointMarginId, wxSTC_MARGIN_SYMBOL );
	SetMarginWidth( m_breakpointMarginId, 0 );
	SetMarginSensitive( m_breakpointMarginId, false );

	// then, apply language specific changes
	if ( lang == CPP || lang == C || lang == LK )
	{
		SetLexer( wxSTC_LEX_CPP );

		/*
		// folding
		SetMarginType( m_foldingMarginId, wxSTC_MARGIN_SYMBOL );
		SetMarginMask( m_foldingMarginId, wxSTC_MASK_FOLDERS );
		StyleSetBackground (m_foldingMarginId, wxColour (_T("WHITE")));
    
		SetMarginWidth (m_foldingMarginId, 16);
		SetMarginSensitive (m_foldingMarginId, true);
		SetProperty (_T("fold"), "1");
		SetProperty (_T("fold.comment"), "1");
		SetProperty (_T("fold.compact"), "1");
		SetProperty (_T("fold.preprocessor"), "1");
	
		SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
					  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
		*/
 
		StyleSetForeground(wxSTC_C_COMMENT, wxColour(0x00, 0xaf, 0x00));
		StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0x00, 0xaf, 0x00));
		StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0xaf, 0xaf, 0xaf));
	
		StyleSetFont( wxSTC_STYLE_DEFAULT, font );
		StyleSetFont( wxSTC_C_DEFAULT, font );
		StyleSetFont( wxSTC_C_COMMENT, fontslant );
		StyleSetFont( wxSTC_C_COMMENTLINE, fontslant );
		StyleSetFont( wxSTC_C_COMMENTDOC, fontslant );

		StyleSetForeground(wxSTC_C_WORD, wxColour("red"));
		StyleSetForeground(wxSTC_C_WORD2,  wxColour(0,128,192));
		
		StyleSetForeground(wxSTC_C_NUMBER,  wxColour(0x00, 0x7f, 0x7f));

		wxColour cLiteral( "maroon" );
		StyleSetForeground(wxSTC_C_STRING, cLiteral );
		StyleSetForeground(wxSTC_C_STRINGEOL, cLiteral );
		StyleSetForeground(wxSTC_C_VERBATIM, cLiteral );
		StyleSetForeground(wxSTC_C_STRINGRAW, cLiteral );
		StyleSetForeground(wxSTC_C_TRIPLEVERBATIM, cLiteral );
		StyleSetForeground(wxSTC_C_HASHQUOTEDSTRING, cLiteral );
		
		StyleSetForeground(wxSTC_C_CHARACTER,  wxColour(0x7f, 0x00, 0x7f));
		StyleSetForeground(wxSTC_C_UUID,  wxColour(0x00, 0x7f, 0x7f));
		StyleSetForeground(wxSTC_C_PREPROCESSOR,  wxColour(0x7f, 0x7f, 0x7f));
		StyleSetForeground(wxSTC_C_OPERATOR, wxColour("blue"));
		StyleSetForeground(wxSTC_C_IDENTIFIER, wxColour(0x00, 0x00, 0x00));

		StyleSetBackground(wxSTC_STYLE_BRACELIGHT, *wxLIGHT_GREY );
		StyleSetForeground(wxSTC_STYLE_BRACELIGHT, *wxWHITE );
		

		if ( lang == C ) SetKeyWords(wxSTC_C_DEFAULT, CWordlist1);	
		else if ( lang == LK )
		{
			SetKeyWords(wxSTC_C_DEFAULT, LKWordlist1 );
			SetMarginWidth(m_foldingMarginId, 0);
			SetProperty(wxT("fold"), "0");
		}
		else SetKeyWords(wxSTC_C_DEFAULT, CppWordlist1);

		m_lang = lang;
	}
	else if ( lang == VBA )
	{
		SetLexer( wxSTC_LEX_VB );
	
		StyleSetForeground(  wxSTC_B_DEFAULT, *wxBLACK );
		StyleSetForeground(  wxSTC_B_COMMENT, wxColour(0,190,0));
		StyleSetForeground(  wxSTC_B_NUMBER, *wxBLUE);
		StyleSetItalic( wxSTC_B_NUMBER, false);

		StyleSetForeground(  wxSTC_B_KEYWORD, *wxRED);
		StyleSetItalic( wxSTC_B_KEYWORD, false);
		StyleSetForeground(  wxSTC_B_STRING, wxColour("maroon"));
		StyleSetForeground(  wxSTC_B_PREPROCESSOR, *wxBLACK); 
		StyleSetForeground(  wxSTC_B_OPERATOR, *wxBLUE);
		StyleSetForeground(  wxSTC_B_IDENTIFIER, *wxBLACK);
		StyleSetForeground(  wxSTC_B_DATE, *wxBLACK);
		StyleSetForeground(  wxSTC_B_STRINGEOL, *wxBLACK);
		StyleSetForeground(  wxSTC_B_KEYWORD2, wxColour(0,128,192));
		StyleSetForeground(  wxSTC_B_KEYWORD3, *wxBLACK);
		StyleSetForeground(  wxSTC_B_KEYWORD4, *wxBLACK);
		StyleSetForeground(  wxSTC_B_CONSTANT, *wxBLACK);
		StyleSetForeground(  wxSTC_B_ASM, *wxBLACK);
		StyleSetForeground(  wxSTC_B_LABEL, *wxLIGHT_GREY);
		StyleSetForeground(  wxSTC_B_ERROR, *wxBLACK);
		StyleSetForeground(  wxSTC_B_HEXNUMBER, *wxBLACK);
		StyleSetForeground(  wxSTC_B_BINNUMBER, *wxBLACK);

	
		StyleSetBackground(wxSTC_STYLE_BRACELIGHT, *wxLIGHT_GREY );
		StyleSetForeground(wxSTC_STYLE_BRACELIGHT, *wxWHITE );

		SetMarginWidth(m_foldingMarginId,0);
		
		m_lang = VBA;
	}
	else if ( lang == HTML )
	{
		SetLexer(wxSTC_LEX_HTML);
		
		wxColour cPhpFore(0, 0, 0);
		wxColour cPhpBack(253, 255, 223);
		
		StyleSetFont(wxSTC_STYLE_DEFAULT, font);
		StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
		StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (_T("WHITE")));
		
		StyleSetFont(wxSTC_H_DEFAULT, font);
		StyleSetForeground( wxSTC_H_DEFAULT, wxColour("black"));
		StyleSetForeground( wxSTC_H_TAG, wxColour("blue"));
		StyleSetForeground( wxSTC_H_TAGUNKNOWN, wxColour("blue"));
	
		StyleSetForeground( wxSTC_H_COMMENT, wxColour(0, 128, 0));
		StyleSetFont( wxSTC_H_COMMENT, fontslant );

		StyleSetForeground( wxSTC_H_ATTRIBUTE, wxColour(0, 0, 150));
		StyleSetForeground( wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(0, 0, 150));
		StyleSetForeground( wxSTC_H_NUMBER, wxColour("black"));
		StyleSetForeground( wxSTC_H_DOUBLESTRING, wxColour(128, 0, 64));
		StyleSetForeground( wxSTC_H_SINGLESTRING, wxColour(128, 0, 64));
		StyleSetForeground( wxSTC_H_OTHER, wxColour("black"));
		StyleSetForeground( wxSTC_H_ENTITY, wxColour("black"));

		//StyleSetBackground( wxSTC_H_SCRIPT, cPhpBack );

		StyleSetForeground( wxSTC_HPHP_DEFAULT, cPhpFore);
		//StyleSetBackground( wxSTC_HPHP_DEFAULT, cPhpBack);
		StyleSetFont( wxSTC_HPHP_DEFAULT, fontsmall);

		StyleSetForeground( wxSTC_HPHP_HSTRING, wxColour(128, 0, 64));
		//StyleSetBackground( wxSTC_HPHP_HSTRING, cPhpBack);
		StyleSetFont( wxSTC_HPHP_HSTRING, fontsmall);

		StyleSetForeground( wxSTC_HPHP_SIMPLESTRING, wxColour(128, 0, 64));
		//StyleSetBackground( wxSTC_HPHP_SIMPLESTRING, cPhpBack);
		StyleSetFont( wxSTC_HPHP_SIMPLESTRING, fontsmall);

		StyleSetForeground( wxSTC_HPHP_WORD, wxColour("red"));
		//StyleSetBackground( wxSTC_HPHP_WORD, cPhpBack);
		StyleSetFont( wxSTC_HPHP_WORD, fontsmall);

		StyleSetForeground( wxSTC_HPHP_NUMBER, wxColour(0, 70, 80));
		//StyleSetBackground( wxSTC_HPHP_NUMBER, cPhpBack);
		StyleSetFont( wxSTC_HPHP_NUMBER, fontsmall);

		StyleSetForeground( wxSTC_HPHP_VARIABLE, wxColour(128,128,64));
		//StyleSetBackground( wxSTC_HPHP_VARIABLE, cPhpBack);
		StyleSetFont( wxSTC_HPHP_VARIABLE, fontsmall);

		StyleSetForeground( wxSTC_HPHP_COMMENT, wxColour(0, 128, 0));
		//StyleSetBackground( wxSTC_HPHP_COMMENT, cPhpBack);
		StyleSetFont( wxSTC_HPHP_COMMENT, fontsmall);

		StyleSetForeground( wxSTC_HPHP_COMMENTLINE, wxColour(0, 128, 0));
		//StyleSetBackground( wxSTC_HPHP_COMMENTLINE, cPhpBack);
		StyleSetFont( wxSTC_HPHP_COMMENTLINE, fontsmall);

		StyleSetForeground( wxSTC_HPHP_HSTRING_VARIABLE, wxColour(128,128,64));
		//StyleSetBackground( wxSTC_HPHP_HSTRING_VARIABLE, cPhpBack);
		StyleSetFont( wxSTC_HPHP_HSTRING_VARIABLE, fontsmall);

		StyleSetForeground( wxSTC_HPHP_OPERATOR, wxColour(cPhpFore));
		//StyleSetBackground( wxSTC_HPHP_OPERATOR, cPhpBack);
		StyleSetFont( wxSTC_HPHP_OPERATOR, fontsmall);

		StyleSetForeground( wxSTC_HPHP_COMPLEX_VARIABLE, wxColour(0, 50, 50));
		//StyleSetBackground( wxSTC_HPHP_COMPLEX_VARIABLE, cPhpBack);
		StyleSetFont( wxSTC_HPHP_COMPLEX_VARIABLE, fontsmall);
		
		m_lang = HTML;
	}
	else if ( lang == TEXT )
	{
		SetLexer( wxSTC_LEX_NULL );
		m_lang = TEXT;
	}
	else if ( lang == TRNSYS )
	{
		SetLexer( wxSTC_LEX_SPICE );
		
		StyleSetForeground(  wxSTC_SPICE_DEFAULT, *wxBLACK );
		StyleSetForeground(  wxSTC_SPICE_COMMENTLINE, wxColour(0,190,0));
		StyleSetForeground(  wxSTC_SPICE_NUMBER, *wxBLACK);
		StyleSetItalic( wxSTC_SPICE_NUMBER, false);

		StyleSetForeground(  wxSTC_SPICE_KEYWORD, *wxRED);
		StyleSetForeground(  wxSTC_SPICE_KEYWORD2, wxColour(0,128,192));
		StyleSetForeground(  wxSTC_SPICE_KEYWORD3, wxColour("maroon"));
		StyleSetItalic( wxSTC_SPICE_KEYWORD, false);
		StyleSetItalic( wxSTC_SPICE_KEYWORD2, false);
		StyleSetForeground(  wxSTC_SPICE_DELIMITER, *wxBLUE);
		StyleSetForeground(  wxSTC_SPICE_VALUE, *wxBLACK); 
		StyleSetForeground(  wxSTC_SPICE_IDENTIFIER, *wxBLACK);
			
		SetKeyWords(0, TrnsysWordlist1);
		SetKeyWords(1, TrnsysWordlist2);
		SetKeyWords(2, TrnsysWordlist3);
		SetMarginWidth(m_foldingMarginId,0);
			
		StyleSetBackground(wxSTC_STYLE_BRACELIGHT, *wxLIGHT_GREY );
		StyleSetForeground(wxSTC_STYLE_BRACELIGHT, *wxWHITE );
		
		m_lang = TRNSYS;
	}
	else if ( lang == PYTHON )
	{	
		SetLexer (wxSTC_LEX_PYTHON);
			
		// folding
		/*
		SetMarginType (m_foldingMarginId, wxSTC_MARGIN_SYMBOL);
		SetMarginMask (m_foldingMarginId, wxSTC_MASK_FOLDERS);
		StyleSetBackground (m_foldingMarginId, wxColour (_T("WHITE")));
    
		SetMarginWidth (m_foldingMarginId, foldingMargin);
		SetMarginSensitive (m_foldingMarginId, true);
		SetProperty (_T("fold"), "1");
		SetProperty (_T("fold.comment"), "1");
		SetProperty (_T("fold.compact"), "1");
		SetProperty (_T("fold.preprocessor"), "1");
	
		SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
					  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
					  */
 
		StyleSetForeground(wxSTC_P_COMMENTLINE, wxColour(0x00, 0xaf, 0x00));
		StyleSetForeground(wxSTC_P_COMMENTBLOCK, wxColour(0xaf, 0xaf, 0xaf));
	
		StyleSetFont(wxSTC_P_COMMENTLINE, fontslant);
		StyleSetFont(wxSTC_P_COMMENTBLOCK, fontslant);

		StyleSetForeground(wxSTC_P_WORD, wxColour("red"));
		SetKeyWords(wxSTC_P_DEFAULT, PythonWordlist1);
	
		StyleSetForeground(wxSTC_P_NUMBER,  wxColour(0x00, 0x7f, 0x7f));
		StyleSetForeground(wxSTC_P_STRING,  wxColour("maroon"));
		StyleSetForeground(wxSTC_P_CHARACTER,  wxColour(0x7f, 0x00, 0x7f));
		StyleSetForeground(wxSTC_P_TRIPLE,  wxColour(0x00, 0x7f, 0x7f));
		StyleSetForeground(wxSTC_P_TRIPLEDOUBLE,  wxColour(0x7f, 0x7f, 0x7f));
		StyleSetForeground(wxSTC_P_OPERATOR, wxColour("blue"));
		StyleSetForeground(wxSTC_P_IDENTIFIER, wxColour(0x00, 0x00, 0x00));

		StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColour("light grey"));
		StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColour("white"));
		
		m_lang = PYTHON;
	}
	
	Colourise(0, GetLength());
	Refresh();
}

wxCodeEditCtrl::Language wxCodeEditCtrl::GetLanguage()
{
	return m_lang;
}

wxCodeEditCtrl::Language wxCodeEditCtrl::GetLanguage( const wxString &fileName )
{
	wxString ext = fileName.Lower();
	
	wxFileName fn( fileName );
	if (fn.HasExt())
		ext = fn.GetExt().Lower();

	if (ext == "cpp") return CPP;
	if (ext == "c") return C;
	if (ext == "h") return CPP;
	if (ext == "lk") return LK;
	if (ext == "sul" || ext == "samul" || ext == "vb" || ext == "vba") return VBA;
	if (ext == "html" || ext == "htm" || ext == "php") return HTML;
	if (ext == "txt") return TEXT;
	if (ext == "trd" || ext == "dck") return TRNSYS;
	if (ext == "py") return PYTHON;
	
	return NONE;
}

void wxCodeEditCtrl::SetKnownIdentifiers( const wxString &text )
{
	SetKeyWords( 1, text );
}

void wxCodeEditCtrl::EnableCallTips( bool en )
{
	m_callTipsEnabled = en;
}

void wxCodeEditCtrl::ClearCallTips()
{
	m_callTips.clear();
}

void wxCodeEditCtrl::ConfigureCallTips( wxUniChar start, wxUniChar end, bool case_sensitive )
{
	m_ctStart = start;
	m_ctEnd = end;
	m_ctCaseSensitive = case_sensitive;
}

void wxCodeEditCtrl::AddCallTip( const wxString &key, const wxString &value )
{
	wxString lckey = key;
	if ( !m_ctCaseSensitive )
		lckey.MakeLower();

	m_callTips[lckey] = value;
}

void wxCodeEditCtrl::ShowLineArrow( int line )
{
	MarkerDeleteAll( m_markArrow );
	MarkerAdd( line, m_markArrow );
}

void wxCodeEditCtrl::HideLineArrow()
{
	MarkerDeleteAll( m_markArrow );
}

void wxCodeEditCtrl::ShowBreakpoints( bool show )
{
	SetMarginWidth( m_breakpointMarginId, show ? 12 : 0 );
	SetMarginSensitive( m_breakpointMarginId, show );
}

void wxCodeEditCtrl::AddBreakpoint( int line )
{
	std::vector<int>::iterator it = std::find( m_breakPoints.begin(), m_breakPoints.end(), line );
	if ( it == m_breakPoints.end() )
	{
		MarkerAdd( line, m_markCircle );
		m_breakPoints.push_back( line );
	}
}

void wxCodeEditCtrl::RemoveBreakpoint( int line )
{
	std::vector<int>::iterator it = std::find( m_breakPoints.begin(), m_breakPoints.end(), line );
	if ( it != m_breakPoints.end() )
	{
		MarkerDelete( line, m_markCircle );
		m_breakPoints.erase( it );
	}
}

void wxCodeEditCtrl::ToggleBreakpoint( int line )
{
	if ( HasBreakpoint( line ) )
		RemoveBreakpoint( line );
	else
		AddBreakpoint( line );
}

bool wxCodeEditCtrl::HasBreakpoint( int line )
{
	return ( std::find( m_breakPoints.begin(), m_breakPoints.end(), line ) != m_breakPoints.end() );
}

void wxCodeEditCtrl::ClearBreakpoints()
{
	MarkerDeleteAll( m_markCircle );
	m_breakPoints.clear();
}

std::vector<int> wxCodeEditCtrl::GetBreakpoints()
{
	return m_breakPoints;
}

void wxCodeEditCtrl::ShowFindReplaceDialog()
{
	m_frDialog->Show();
	m_frDialog->SetFocus();
	m_frDialog->SelectAll();
}

void wxCodeEditCtrl::FindNext()
{
	m_frDialog->FindNext();
}

void wxCodeEditCtrl::ReplaceNext()
{
	m_frDialog->ReplaceNext();
}

void wxCodeEditCtrl::ReplaceAll()
{
	m_frDialog->ReplaceAll();
}

int	wxCodeEditCtrl::FindNext( const wxString &text, int frtxt_len, bool match_case, bool whole_word )
{
	if (frtxt_len < 0) frtxt_len = text.Len();

	int start = m_lastFindPos >= 0 ? m_lastFindPos+frtxt_len : 0;
	if (start > GetLength())
		start = 0;

	int flags = 0;
	
	if ( whole_word )
		flags |= wxSTC_FIND_WHOLEWORD;
	
	if ( match_case)
		flags |= wxSTC_FIND_MATCHCASE;

	m_lastFindPos = FindText(start, GetLength(), text, flags);
	if (m_lastFindPos >= 0)
	{
		SetSelection(m_lastFindPos, m_lastFindPos+text.Len());
		EnsureCaretVisible();
	}
	else
	{
		m_lastFindPos = FindText(0, GetLength(), text, flags);
		if (m_lastFindPos >= 0)
		{
			SetSelection(m_lastFindPos, m_lastFindPos+text.Len());
			EnsureCaretVisible();
		}
	}

	return m_lastFindPos;
}

int wxCodeEditCtrl::ReplaceNext( const wxString &text, const wxString &replace, bool stop_at_find, 
			bool match_case, bool whole_word )
{
	bool cur_selected = false;
	if ( match_case )
		cur_selected = (GetSelectedText() == text );
	else
		cur_selected = (GetSelectedText().Lower() == text.Lower());

	if (!cur_selected)
	{
		cur_selected = (FindNext( text, -1, match_case, whole_word ) >= 0);
		if ( stop_at_find )
			return cur_selected ? m_lastFindPos : -1;
	}

	if (!cur_selected)
	{
		return -1;
	}

	ReplaceSelection( replace );
	return FindNext( text, replace.Len(), match_case, whole_word );
}

int wxCodeEditCtrl::ReplaceAll( const wxString &text, const wxString &replace, 
			bool match_case, bool whole_word, bool show_message )
{
	int count = 0;
	while ( ReplaceNext( text, replace, false, match_case, whole_word ) >= 0 )
		count++;

	if (show_message) wxMessageBox( wxString::Format( "%d instances replaced.", count ) );
	return count;
}

void wxCodeEditCtrl::SelectLine( int line )
{
	line--;
	if (line < 0) line = 0;
	GotoLine( line );
	SetSelection(PositionFromLine(line), GetLineEndPosition(line)+1);
	EnsureCaretVisible();
}

void wxCodeEditCtrl::YankLine()
{
	int line = GetCurrentLine();
	wxString text = GetLine(line);
	SetTargetStart( PositionFromLine(line) );
	SetTargetEnd( PositionFromLine(line+1) );
	ReplaceTarget( wxEmptyString );
	m_yankText = text;
}

void wxCodeEditCtrl::PutLine()
{
	if ( !m_yankText.IsEmpty() )
		InsertText( PositionFromLine( GetCurrentLine() ), m_yankText );
}
	
void wxCodeEditCtrl::OnMarginClick( wxStyledTextEvent &evt )
{
	if (evt.GetMargin() == m_breakpointMarginId )
	{
		int line = LineFromPosition(evt.GetPosition());
		ToggleBreakpoint( line );
	}
	else if (evt.GetMargin() == m_foldingMarginId)
	{
        int lineClick = LineFromPosition (evt.GetPosition());
        int levelClick = GetFoldLevel (lineClick);
        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
		{
            ToggleFold (lineClick);
        }
    }
	
	evt.Skip(); // pass on these events to descendant classes
}

void wxCodeEditCtrl::OnCharAdded( wxStyledTextEvent &evt )
{
	wxUniChar ch = evt.GetKey();

	if (ch == '\n')
	{
		int curline = GetCurrentLine();
		
		if (curline - 1 >= 0 && curline -1 <= GetLineCount())
		{
			wxString prevline = GetLine(curline - 1);
		
			static char buf[32];
			size_t prevlinelen = prevline.Length();
			size_t i;
			for ( i=0;i<prevlinelen && i < 31 && (prevline[i] == '\t' || prevline[i] == ' '); i++)
				buf[i] = prevline[i];

			buf[i] = '\0';
			
			ReplaceSelection(buf);
		}
	}
	else if (ch == m_ctStart && m_callTipsEnabled )
	{
		wxString buf = GetTextRange( PositionFromLine( GetCurrentLine() ), GetCurrentPos() );
		
		int i = (int)buf.Len()-2;
		while (i >= 0 && (buf[i] == '\t' || buf[i] == ' '))
			i--;

		int len = 0;
		while (i >= 0 && (isalpha(buf[i]) || isdigit(buf[i]) || buf[i] == '_'))
		{
			i--;
			len++;
		}

		wxString func = buf.Mid(i+1, len);
		if ( !m_ctCaseSensitive )
			func.MakeLower();

		if ( m_callTips.find(func) != m_callTips.end() )		
		{
			if (CallTipActive())			
				Cancel();
			else
				m_ctStack.Clear();

			wxString tip = wxLimitTextColumns( m_callTips[func], 80 );
			m_ctStack.Add( tip );
			CallTipShow( GetCurrentPos(), tip );
		}
	}
	else if (ch == m_ctEnd && m_callTipsEnabled)
	{
		Cancel();
		if ( m_ctStack.Count() > 0 )
			m_ctStack.RemoveAt( m_ctStack.Count() - 1 );

		if ( m_ctStack.Count() > 0 )
			CallTipShow( GetCurrentPos(), m_ctStack.Last() );
	}
	
	evt.Skip(); // pass on events to descendent classes
}

void wxCodeEditCtrl::OnUpdateUI( wxStyledTextEvent &evt )
{
	DoBraceMatch();
	evt.Skip(); // pass on events to descendent classes
}

void wxCodeEditCtrl::DoBraceMatch()
{
	int braceAtCaret = -1;
	int braceOpposite = -1;

	FindMatchingBracePosition(braceAtCaret, braceOpposite, false);

	if ((braceAtCaret != -1) && (braceOpposite == -1))
	{
		BraceBadLight(braceAtCaret);
		SetHighlightGuide(0);
	}
	else
	{
		BraceHighlight(braceAtCaret, braceOpposite);

		int columnAtCaret = GetColumn(braceAtCaret);
		int columnOpposite = GetColumn(braceOpposite);

		SetHighlightGuide(columnAtCaret < columnOpposite ? columnAtCaret : columnOpposite);
	}
}

bool wxCodeEditCtrl::FindMatchingBracePosition( int &braceAtCaret, int &braceOpposite, bool sloppy )
{
	bool isInside = false;
	
	int caretPos = GetCurrentPos();
	braceAtCaret = -1;
	braceOpposite = -1;
	wxUniChar charBefore = 0;
	wxUniChar styleBefore = 0;
	int lengthDoc = GetLength();

	if ((lengthDoc > 0) && (caretPos > 0))
	{
		// Check to ensure not matching brace that is part of a multibyte character
		if (PositionBefore(caretPos) == (caretPos - 1))
		{
			charBefore = GetCharAt(caretPos - 1);
			styleBefore = (char)(GetStyleAt(caretPos - 1) & 31);
		}
	}
	// Priority goes to character before caret
	if (charBefore && IsBrace(charBefore) )
	{
		braceAtCaret = caretPos - 1;
	}


	bool isAfter = true;
	if (lengthDoc > 0 && sloppy && (braceAtCaret < 0) && (caretPos < lengthDoc))
	{
		// No brace found so check other side
		// Check to ensure not matching brace that is part of a multibyte character
		if (PositionAfter(caretPos) == (caretPos + 1))
		{
			char charAfter = GetCharAt(caretPos);

			if (charAfter && IsBrace(charAfter))
			{
				braceAtCaret = caretPos;
				isAfter = false;
			}
		}
	}

	if (braceAtCaret >= 0)
	{
		braceOpposite = BraceMatch(braceAtCaret);

		if (braceOpposite > braceAtCaret) {
			isInside = isAfter;
		} else {
			isInside = !isAfter;
		}
	}

	return isInside;
}
