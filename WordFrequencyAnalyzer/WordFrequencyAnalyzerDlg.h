#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

class CWordFrequencyAnalyzerDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CWordFrequencyAnalyzerDlg)

public:
    CWordFrequencyAnalyzerDlg(CWnd* pParent = nullptr);
    virtual ~CWordFrequencyAnalyzerDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WORDFREQUENCYANALYZER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    HICON m_hIcon;

    DECLARE_MESSAGE_MAP()

private:
    std::string CStringToStdString(const CString& cstr);
    CString StdStringToCString(const std::string& str);

    void Log(const std::string& message, int level = 0);
    std::ofstream logFile;

    std::set<std::string> LoadKeywords(const std::string& filename);
    std::vector<std::vector<std::string>> LoadTextFile(const std::string& filename);
    std::map<std::string, std::vector<std::pair<int, int>>> AnalyzeText(
        const std::vector<std::vector<std::string>>& textLines,
        const std::set<std::string>& keywords);
    void SaveResults(const std::string& filename,
        const std::map<std::string, std::vector<std::pair<int, int>>>& results);

    CEdit m_editTextFile;
    CEdit m_editKeywordsFile;
    CEdit m_editOutputFile;
    CListCtrl m_listResults;

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonAnalyze();
    afx_msg void OnBnClickedButtonBrowseText();
    afx_msg void OnBnClickedButtonBrowseKeywords();
    afx_msg void OnBnClickedButtonBrowseOutput();
};