#include "pch.h"
#include "framework.h"
#include "WordFrequencyAnalyzer.h"
#include "WordFrequencyAnalyzerDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

IMPLEMENT_DYNAMIC(CWordFrequencyAnalyzerDlg, CDialogEx)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string CWordFrequencyAnalyzerDlg::CStringToStdString(const CString& cstr)
{
    CT2A ct2a(cstr);
    return std::string(ct2a);
}

CString CWordFrequencyAnalyzerDlg::StdStringToCString(const std::string& str)
{
    return CString(str.c_str());
}

CWordFrequencyAnalyzerDlg::CWordFrequencyAnalyzerDlg(CWnd* pParent)
    : CDialogEx(IDD_WORDFREQUENCYANALYZER_DIALOG, pParent)
    , m_hIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
{
    logFile.open("WordFrequencyAnalyzer.log", std::ios::out | std::ios::app);
    if (!logFile.is_open())
    {
        AfxMessageBox(_T("Failed to open log file!"), MB_ICONERROR);
    }
    Log("Application started");
}

CWordFrequencyAnalyzerDlg::~CWordFrequencyAnalyzerDlg()
{
    Log("Application closed");
    if (logFile.is_open())
    {
        logFile.close();
    }
}

void CWordFrequencyAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_TEXT_FILE, m_editTextFile);
    DDX_Control(pDX, IDC_EDIT_KEYWORDS_FILE, m_editKeywordsFile);
    DDX_Control(pDX, IDC_EDIT_OUTPUT_FILE, m_editOutputFile);
}

BEGIN_MESSAGE_MAP(CWordFrequencyAnalyzerDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_ANALYZE, &CWordFrequencyAnalyzerDlg::OnBnClickedButtonAnalyze)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_TEXT, &CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseText)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_KEYWORDS, &CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseKeywords)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUTPUT, &CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseOutput)
END_MESSAGE_MAP()

void CWordFrequencyAnalyzerDlg::Log(const std::string& message, int level)
{
    if (!logFile.is_open()) return;
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

    std::string levelStr;
    switch (level)
    {
    case 0: levelStr = "INFO"; break;
    case 1: levelStr = "WARNING"; break;
    case 2: levelStr = "ERROR"; break;
    default: levelStr = "DEBUG"; break;
    }
    logFile << "[" << timeStr << "] [" << levelStr << "] " << message << std::endl;
    logFile.flush();
}

std::set<std::string> CWordFrequencyAnalyzerDlg::LoadKeywords(const std::string& filename)
{
    Log("Loading keywords from: " + filename);
    std::set<std::string> keywords;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        Log("Failed to open keywords file: " + filename, 2);
        throw std::runtime_error("Failed to open keywords file: " + filename);
    }

    std::string word;
    while (file >> word)
    {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        keywords.insert(word);
    }

    Log("Loaded " + std::to_string(keywords.size()) + " keywords");
    return keywords;
}

std::vector<std::vector<std::string>> CWordFrequencyAnalyzerDlg::LoadTextFile(const std::string& filename)
{
    Log("Loading text file: " + filename);
    std::vector<std::vector<std::string>> lines;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        Log("Failed to open text file: " + filename, 2);
        throw std::runtime_error("Failed to open text file: " + filename);
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        lineNumber++;
        std::vector<std::string> words;
        std::istringstream iss(line);
        std::string word;

        int wordNumber = 0;
        while (iss >> word)
        {
            wordNumber++;
            word.erase(std::remove_if(word.begin(), word.end(),
                [](char c) { return !isalpha(c); }), word.end());

            if (!word.empty())
            {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                words.push_back(word);
            }
        }

        lines.push_back(words);
    }

    Log("Loaded " + std::to_string(lineNumber) + " lines from text file");
    return lines;
}

std::map<std::string, std::vector<std::pair<int, int>>> CWordFrequencyAnalyzerDlg::AnalyzeText(
    const std::vector<std::vector<std::string>>& textLines,
    const std::set<std::string>& keywords)
{
    Log("Analyzing text...");
    std::map<std::string, std::vector<std::pair<int, int>>> results;

    for (size_t i = 0; i < textLines.size(); ++i)
    {
        for (size_t j = 0; j < textLines[i].size(); ++j)
        {
            const std::string& word = textLines[i][j];
            if (keywords.find(word) == keywords.end())
            {
                results[word].emplace_back(i + 1, j + 1);
            }
        }
    }

    Log("Found " + std::to_string(results.size()) + " unique non-keywords");
    return results;
}

void CWordFrequencyAnalyzerDlg::SaveResults(const std::string& filename,
    const std::map<std::string, std::vector<std::pair<int, int>>>& results)
{
    Log("Saving results to: " + filename);
    std::ofstream file(filename);

    if (!file.is_open())
    {
        Log("Failed to open output file: " + filename, 2);
        throw std::runtime_error("Failed to open output file: " + filename);
    }

    for (const auto& entry : results)
    {
        file << entry.first << "\t";
        for (size_t i = 0; i < entry.second.size(); ++i)
        {
            if (i != 0) file << ", ";
            file << "(" << entry.second[i].first << ", " << entry.second[i].second << ")";
        }
        file << std::endl;
    }

    Log("Results saved successfully");
}

BOOL CWordFrequencyAnalyzerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_editTextFile.SetWindowTextW(_T("input.txt"));
    m_editKeywordsFile.SetWindowTextW(_T("keywords.txt"));
    m_editOutputFile.SetWindowTextW(_T("output.txt"));

    return TRUE;
}

void CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseText()
{
    CFileDialog dlg(TRUE, _T("txt"), _T("input.txt"), OFN_FILEMUSTEXIST, _T("Text Files (*.txt)|*.txt||"));
    if (dlg.DoModal() == IDOK)
    {
        m_editTextFile.SetWindowTextW(dlg.GetPathName());
    }
}

void CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseKeywords()
{
    CFileDialog dlg(TRUE, _T("txt"), _T("keywords.txt"), OFN_FILEMUSTEXIST, _T("Text Files (*.txt)|*.txt||"));
    if (dlg.DoModal() == IDOK)
    {
        m_editKeywordsFile.SetWindowTextW(dlg.GetPathName());
    }
}

void CWordFrequencyAnalyzerDlg::OnBnClickedButtonBrowseOutput()
{
    CFileDialog dlg(FALSE, _T("txt"), _T("output.txt"), OFN_OVERWRITEPROMPT, _T("Text Files (*.txt)|*.txt||"));
    if (dlg.DoModal() == IDOK)
    {
        m_editOutputFile.SetWindowTextW(dlg.GetPathName());
    }
}

void CWordFrequencyAnalyzerDlg::OnBnClickedButtonAnalyze()
{
    try
    {
        Log("Starting analysis...");

        CString strTextFile, strKeywordsFile, strOutputFile;
        m_editTextFile.GetWindowText(strTextFile);
        m_editKeywordsFile.GetWindowText(strKeywordsFile);
        m_editOutputFile.GetWindowText(strOutputFile);

        if (strTextFile.IsEmpty() || strKeywordsFile.IsEmpty() || strOutputFile.IsEmpty())
        {
            AfxMessageBox(_T("Please specify all file paths!"), MB_ICONWARNING);
            Log("Not all file paths specified", 1);
            return;
        }

        auto keywords = LoadKeywords(CStringToStdString(strKeywordsFile));
        auto textLines = LoadTextFile(CStringToStdString(strTextFile));

        auto results = AnalyzeText(textLines, keywords);

        SaveResults(CStringToStdString(strOutputFile), results);

        int index = 0;
        for (const auto& entry : results)
        {
            CString positions;
            for (const auto& pos : entry.second)
            {
                if (!positions.IsEmpty())
                    positions += _T(", ");
                positions.AppendFormat(_T("(%d,%d)"), pos.first, pos.second);
            }


            index++;
        }

        AfxMessageBox(_T("Analysis completed successfully!"), MB_ICONINFORMATION);
        Log("Analysis completed successfully");
    }
    catch (const std::exception& e)
    {
        AfxMessageBox(StdStringToCString(e.what()), MB_ICONERROR);
        Log(std::string("Error during analysis: ") + e.what(), 2);
    }
}