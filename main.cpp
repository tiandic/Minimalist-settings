#include "RoundedWidget.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cstdio>
#include <filesystem>

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolButton>

namespace fs = std::filesystem;

bool get_config(const std::string line, std::string *title, std::string *detail, std::string *type)
{
    const std::regex line_reg("//\\$.*$");
    const std::regex title_reg("title:\\s*([^;]*)");
    const std::regex detail_reg("detail:\\s*([^;]*)");
    const std::regex type_reg("type:\\s*([^;]*)");
    std::smatch match;
    std::string con;

    if (!std::regex_search(line, match, line_reg))
        return false;
    con = match.str();
    std::regex_search(con, match, title_reg);
    *title = match[1];
    std::regex_search(con, match, detail_reg);
    *detail = match[1];
    std::regex_search(con, match, type_reg);
    *type = match[1];
    return true;
}

void trim(const std::string &str, std::string &out)
{
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
        start++;
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        end--;

    out = str.substr(start, end - start);
}

void getValue(const std::string line, std::string &value)
{
    bool found = false;
    for (size_t i = 0; i < line.size(); i++)
    {
        if (found)
            value += line[i];
        else
        {
            if (line[i] == ':')
                found = true;
        }
    }
    std::string tmp;
    trim(value, tmp);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    if (tmp.compare(0, 4, "true", 0, 4) == 0)
        value = "true";
    else if (tmp.compare(0, 4, "false", 0, 5) == 0)
        value = "false";
    else if (tmp[0] == '"')
    {
        value.clear();
        for (size_t i = 1; i < tmp.size(); i++)
        {
            if (tmp[i] != '"' || tmp[i - 1] == '\\')
                value += tmp[i];
            else
                break;
        }
    }
    else if (tmp[0] == '\'')
    {
        value.clear();
        for (size_t i = 1; i < tmp.size(); i++)
        {
            if (tmp[i] != '\'' || tmp[i - 1] == '\\')
                value += tmp[i];
            else
                break;
        }
    }
}

bool moveFileOverwrite(const fs::path &source, const fs::path &dest)
{
    try
    {
        if (!fs::exists(source))
        {
            std::cerr << "源文件不存在: " << source << std::endl;
            return false;
        }

        if (fs::exists(dest))
        {
            fs::remove(dest);
        }

        // 移动文件（相当于重命名）
        fs::rename(source, dest);
        return true;
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "文件移动失败: " << e.what() << std::endl;
        return false;
    }
}

void editLine(const std::string &fileName, size_t lineNum, const std::string &regStr, const std::string &newStr, bool ignore)
{
    std::regex r;
    if (ignore)
        r = std::regex(regStr, std::regex_constants::icase); // 不区分大小写
    else
        r = std::regex(regStr); // 区分大小写
    std::ifstream fin(fileName);
    if (!fin.is_open())
    {
        std::cerr << "Error: cannot open " << fileName << "\n";
        return;
    }

    std::ofstream fout(fileName + ".tmp");
    if (!fout.is_open())
    {
        std::cerr << "Error: cannot create temp file\n";
        return;
    }

    std::string line;
    size_t i = 0;
    while (getline(fin, line))
    {
        if (i == lineNum)
            fout << std::regex_replace(line, r, newStr, std::regex_constants::format_first_only) << '\n';
        else
            fout << line << '\n';
        i++;
    }

    fin.close();
    fout.close();

    // 覆盖原文件
    moveFileOverwrite(fileName + ".tmp", fileName);
}

std::string getNumLine(std::string fileName, size_t lineNum)
{
    std::string line;
    std::ifstream fin(fileName);
    size_t i = 0;
    if (!fin.is_open())
    {
        std::cerr << "Error: cannot open " << fileName << "\n";
        return "";
    }
    while (getline(fin, line))
    {
        if (i == lineNum)
        {
            fin.close();
            return line;
        }
        i++;
    }
    return "";
}

QLabel *createTitle(std::string title, std::string detail, RoundedWidget *roundedWidget)
{
    QLabel *label = new QLabel(QString::fromStdString(title), roundedWidget);
    QFont font = label->font();
    font.setPointSize(14);
    label->setFont(font);
    label->setToolTip(QString::fromStdString(detail)); // detail在鼠标悬停在title时显示
    return label;
}

RoundedWidget *createCheckBox(std::string title, std::string detail, bool CheckStatus, std::string fileName, size_t lineNum)
{
    RoundedWidget *roundedWidget = new RoundedWidget;
    QHBoxLayout *layout = roundedWidget->layoutContainer();
    roundedWidget->setRadius(30);
    // Title部分
    QLabel *label = createTitle(title, detail, roundedWidget);
    layout->addWidget(label);
    // 开关
    QCheckBox *checkBox = new QCheckBox(roundedWidget);
    checkBox->setChecked(CheckStatus);
    QObject::connect(checkBox, &QCheckBox::toggled, checkBox, [fileName, lineNum](bool checked)
                     {
                        if (checked)
                            editLine(fileName, lineNum, "false", "true",true);
                        else
                            editLine(fileName, lineNum, "true", "false",true); });
    checkBox->setText("");
    checkBox->setFixedSize(30, 30); // 设置为正方形
    checkBox->setStyleSheet(R"(
    QCheckBox {
        background-color: transparent;  /* 设置背景透明 */
        border: 3px solid #808080;
        border-radius: 5px;
        padding: 0;
    }
    QCheckBox::indicator {
        width: 20px;
        height: 20px;
        border-radius: 5px;
        background-color: transparent;  /* 设置指示器背景透明 */
        margin: 2px;
    }
    QCheckBox::indicator:checked {
        border-radius: 5px;
        image: url(:/images/images/checked_icon2.png);
        background-position: center;
        background-repeat: no-repeat;
    }
)");

    layout->addWidget(checkBox);
    return roundedWidget;
}

RoundedWidget *createLine(std::string title, std::string detail, std::string line, std::string fileName, size_t lineNum)
{
    RoundedWidget *roundedWidget = new RoundedWidget;
    QHBoxLayout *layout = roundedWidget->layoutContainer();
    roundedWidget->setRadius(30);
    // ttile部分
    QLabel *label = createTitle(title, detail, roundedWidget);
    layout->addWidget(label);
    // 编辑框部分
    QLineEdit *lineEdit = new QLineEdit(roundedWidget);
    lineEdit->setStyleSheet(R"(
    QLineEdit {
        background-color: #f5f5f5;
        border: 2px solid #8f8f8f;
        border-radius: 10px;
        padding: 1px;
        font-size: 12px;
        min-height: 20px;  /* 最小高度 */
        max-height: 20px;  /* 最大高度 */
        font-family: Arial, sans-serif;
    }
    QLineEdit:focus {
        border-color: #3b8dbd;
    }
)");
    lineEdit->setText(QString::fromStdString(line));
    QObject::connect(lineEdit, &QLineEdit::returnPressed, [=]()
                     {
        std::string lastLine = getNumLine(fileName,lineNum);
        std::string lastValue;
        getValue(lastLine,lastValue);
        editLine(fileName,lineNum,"[\"']"+lastValue+"[\"']","\""+lineEdit->text().toStdString()+"\"",false); });
    layout->addWidget(lineEdit);
    return roundedWidget;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::string fileName = argv[1];
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file '" << fileName << "'" << std::endl;
        return -1;
    }
    std::string line;
    // 创建主窗口部件和布局
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 创建一个QScrollArea来容纳内容
    QScrollArea *scrollArea = new QScrollArea;
    QWidget *contentWidget = new QWidget; // 内容部件
    scrollArea->setStyleSheet(R"(

    QScrollArea {
        background-color: transparent;
        border: 2px solid #cccccc;
        border-radius: 12px;
        padding: 10px;
    }


    QScrollBar:vertical {
        border: none;
        background: transparent;
        width: 12px;
        border-radius: 6px;
        margin: 0px 0px 0px 0px;
    }


    QScrollBar::handle:vertical {
        background: #87cefa;
        border-radius: 6px;
        min-height: 50px;
    }


    QScrollBar::handle:vertical:hover {
        background: #4682B4;
    }


    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {
        border: none;
        background: none;
    }


    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        border: none;
        background: none;
    }

)");
    contentWidget->setStyleSheet("background-color:transparent;"); // 确保内容部件也透明
    // 在 contentWidget 中设置纵向布局
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    size_t i = 0;
    while (std::getline(file, line))
    {
        std::string title;
        std::string detail;
        std::string type;
        i++;
        if (!get_config(line, &title, &detail, &type))
            continue;
        RoundedWidget *b;
        std::string lineValue;
        getValue(line, lineValue);
        if (type == "bool")
            b = createCheckBox(title, detail, (lineValue == "true"), fileName, i - 1);
        else if (type == "string")
            b = createLine(title, detail, lineValue, fileName, i - 1);
        contentLayout->addWidget(b);
    }

    // 设置主窗口
    QMainWindow mainWin;
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    mainWin.setCentralWidget(centralWidget);
    mainWin.setStyleSheet(R"(
        QMainWindow {
            background-image: url(:/images/images/background.png);
            background-repeat: no-repeat;
            background-position: center;
            background-size: cover;
        }
    )");
    mainWin.resize(600, 300);
    mainWin.show();
    file.close();
    return app.exec();
}
