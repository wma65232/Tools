#include <QDragEnterEvent>  //akne inversa, Hautärztin Aschaffenbucg
#include <QMimeData>
#include <QDebug>
#include <QList>
#include <QString>

#include <QMessageBox>

#include <chrono>
#include <thread>


#include "mainwindow.h"
#include "ui_mainwindow.h"

// Quelle : https://www.youtube.com/watch?v=XintqJlSY1U

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList titel;
    setAcceptDrops(true);
    setWindowTitle("Picture Sorter");

    ui->tableWidget->setColumnCount(3);
    titel << "File" << "Date" << "Misc";
    ui->tableWidget->setHorizontalHeaderLabels(titel);
    ui->progressBarFileOpreation->setValue(0);
    //ui->progressBarFileOpreation->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    initializeUrlList(urls);
}

void MainWindow::initializeUrlList(QList<QUrl>& urlList)
{
    //QString currentFileName;
    m_urlList.clear();
    m_urlList = urlList;
    //updateTable(urlList);
    ui->listWidgetSource->clear();
    ui->listWidgetTarget->clear();
    updateSourceFileList(urlList);
}


void MainWindow::updateSourceFileList(QList<QUrl>& urlList)
{
    QString currentFileName;
    for(QList<QUrl>::iterator it = urlList.begin(); it != urlList.end(); ++it)
    {
        currentFileName = it->path();
        ui->listWidgetSource->addItem(currentFileName);
    }
}

void MainWindow::updateTable(QList<QUrl>& urlList)
{
    QString currentFileName;

    ui->listWidgetTarget->clear();
    ui->listWidgetSource->clear();

    for(QList<QUrl>::iterator it = urlList.begin(); it != urlList.end(); ++it)
    {
        currentFileName = it->path();
        ui->listWidgetSource->addItem(currentFileName);

        m_tableLineNo = ui->tableWidget->rowCount()-1;
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(m_tableLineNo, FILE, new QTableWidgetItem(currentFileName));
        ui->tableWidget->setItem(m_tableLineNo, DATE, new QTableWidgetItem("22.1.2017"));
        m_tableLineNo++;
    }

}

bool MainWindow::isReferenceFile(const QFileInfo fileInfo)
{
    bool testResult = false;
    QString suffix = fileInfo.suffix();
    QString name   = fileInfo.fileName();

    QRegExp rx("jp*", Qt::CaseInsensitive); // all jpeg's
    rx.setPatternSyntax(QRegExp::Wildcard);
    testResult = rx.exactMatch(suffix);

    QRegExp rx2("m*", Qt::CaseInsensitive); // all movies mp4, mts, mov
    rx2.setPatternSyntax(QRegExp::Wildcard);
    testResult |= rx2.exactMatch(suffix);

    QRegExp rx3("avi", Qt::CaseInsensitive); // all movies avi
    rx3.setPatternSyntax(QRegExp::Wildcard);
    testResult |= rx3.exactMatch(suffix);

    QRegExp rx4("wma", Qt::CaseInsensitive); // all movies wma
    rx4.setPatternSyntax(QRegExp::Wildcard);
    testResult |= rx4.exactMatch(suffix);

    QRegExp rx5("tif*", Qt::CaseInsensitive); // all tif files
    rx5.setPatternSyntax(QRegExp::Wildcard);
    testResult |= rx5.exactMatch(suffix);

    testResult &= !name.contains("-");  // all pano files need special treatment

    return testResult;
}

QString  MainWindow::createMkDirString(const QFileInfo &fileinfo)
{
    int year  = (fileinfo.lastModified().date().year());
    int month = (fileinfo.lastModified().date().month());
    int day   = (fileinfo.lastModified().date().day());

    QString mkdirString = QString("%1/%2%3%4_").arg(
                QString(fileinfo.absolutePath()),
                QString::number(year),
                QString::number(month).rightJustified(2, '0'),
                QString::number(day).rightJustified(2, '0')
                );
    //qDebug() << mkdirString;
    return  mkdirString;
}

void MainWindow::createFileEntryListFromUrls(const QList<QUrl>& urlList)
{
    if(urlList.empty())
        return;

    for(auto it = urlList.begin(); it != urlList.end(); ++it)
    {
        QFileInfo fileinfo(it->toLocalFile());
        FileEntry fe;
        fe.inputFileInfo = fileinfo;
        fe.inputFile = fileinfo.absoluteFilePath();

        QString mkdirString = createMkDirString(fileinfo);

        fe.mkdirString = mkdirString;

        QString outputFullFileName = QString("%1/%2").arg(mkdirString, fileinfo.fileName());
        ui->listWidgetTarget->addItem(outputFullFileName);
        fe.outputFile = outputFullFileName;

        if(!isReferenceFile(fileinfo))
            fe.needsReferenceFile = true; // mark this file to complete its date information in a 2nd pass

        m_fileEntryList.append(fe);
    }// for..




    // 2nd pass...
    for(QList<FileEntry>::iterator needRefIt = m_fileEntryList.begin(); needRefIt != m_fileEntryList.end(); ++needRefIt)
    {
        QString fileNameWithoutExtension = needRefIt->inputFileInfo.fileName().left(needRefIt->inputFileInfo.fileName().lastIndexOf('.'));

        if (fileNameWithoutExtension.contains('-'))
            fileNameWithoutExtension = fileNameWithoutExtension.left(fileNameWithoutExtension.indexOf('-'));

        if(needRefIt->needsReferenceFile)
        {
            //qDebug() << "needs a reference : " << needRefIt->inputFileInfo.fileName();
            for(int ix = 0; ix < m_fileEntryList.size(); ++ix)
            {
                QString fileThatNeedsReference = needRefIt->inputFileInfo.fileName();
                QString fileToBeLookedUpInList = m_fileEntryList.at(ix).inputFileInfo.fileName();
                if(m_fileEntryList.at(ix).inputFileInfo.fileName().startsWith(fileNameWithoutExtension) && !m_fileEntryList.at(ix).needsReferenceFile)
                {
                    qDebug() << "searching for ref of : " << fileThatNeedsReference << "  ...found this: " << fileToBeLookedUpInList << " ix= " << ix;
                    QFileInfo fi = m_fileEntryList.at(ix).inputFileInfo;

                    QString mkdirString = createMkDirString(fi);

                    //QString outputFullFileName = QString("%1/%2").arg(mkdirString, fi.fileName());//fileThatNeedsReference
                    //QString outputFullFileName = QString("%1/%2").arg(mkdirString, fileThatNeedsReference);
                    //qDebug() << mkdirString;

                    QString outputFullFileName = QString("%1/%2").arg(mkdirString, needRefIt->inputFileInfo.fileName());

                    m_fileEntryList[ix].mkdirString = mkdirString;
                    m_fileEntryList[ix].needsReferenceFile = false;
                    m_fileEntryList[ix].outputFile = outputFullFileName;

                }// m_fileEntryList.at(ix)...
            }// for(int ix...
        }// if(needRefIt...
    }// for(QList...
}

void MainWindow::createDirectories(const QList<FileEntry>& fileEntryList)
{
    if(fileEntryList.empty())
        return;

    for(auto it = fileEntryList.begin(); it != fileEntryList.end(); ++it)
    {
            QDir dir(it->mkdirString);
            if (!dir.exists()){
              dir.mkdir(".");
            }
        }
    }

//Ui::MainWindow::FileStates MainWindow::copyFilesToDestination(QList<FileEntry>& fileEntryList)

MainWindow::FileStates MainWindow::copyFilesToDestination(QList<FileEntry>& fileEntryList)
{
    //FileStates state = FileStates::undefined;
    int fileCount = 0;
    int fileCountMax = fileEntryList.size();
    for(QList<FileEntry>::iterator it3 = fileEntryList.begin(); it3 != fileEntryList.end(); ++it3)
    {
        ui->progressBarFileOpreation->setValue((fileCount++) * 100 / fileCountMax);

        if(!QFile::exists(it3->outputFile)) {
            QFile::copy(it3->inputFile, it3->outputFile);
    }
        else {
            //state = FileStates::fileAlreadyExists;
        }
    }
    ui->progressBarFileOpreation->setValue(0);


    return MainWindow::success; // TODO : only for debug!
    //return state;
}

MainWindow::FileStates MainWindow::moveFilesToDestination(QList<FileEntry>& fileEntryList)
{
    int fileCount = 0;
    int fileCountMax = fileEntryList.size();
    FileStates operationFinishedWith = FileStates::undefined;
    for(QList<FileEntry>::iterator it3 = fileEntryList.begin(); it3 != fileEntryList.end(); ++it3)
    {
        bool success = true;
        //std::this_thread::sleep_for(std::chrono::milliseconds(250)); // http://www.codebind.com/cpp-tutorial/qt-tutorials-for-beginners-qmessagebox/
        if(!QFile::exists(it3->outputFile))
        {
            success &= QFile::rename(it3->inputFile, it3->outputFile);
            ui->progressBarFileOpreation->setValue((fileCount++) * 100 / fileCountMax);
        }

    }
    ui->progressBarFileOpreation->setValue(0);

    return MainWindow::success; // TODO : only for debug!
}

void MainWindow::on_pushButtonPreview_clicked()
{
    ui->listWidgetTarget->clear();
    m_fileEntryList.clear();
    createFileEntryListFromUrls(m_urlList);
    //createDirectories(m_fileEntryList, false);
}

void MainWindow::on_pushButtonCreate_clicked()
{
    prepareCreateDirectories();
}

void MainWindow::prepareCreateDirectories()
{
    ui->listWidgetTarget->clear();
    m_fileEntryList.clear();
    createFileEntryListFromUrls(m_urlList);
    createDirectories(m_fileEntryList);

}

void MainWindow::on_pushButtonCopy_clicked()
{
    prepareCreateDirectories();
    ui->progressBarFileOpreation->setVisible(true);
    copyFilesToDestination(m_fileEntryList);
    //ui->progressBarFileOpreation->setVisible(false);
}

void MainWindow::on_pushButtonMove_clicked()
{
    prepareCreateDirectories();
    //FileStates fileState = moveFilesToDestination(m_fileEntryList);
    FileStates fileState = FileStates::couldNotDeleteSourceFile;
    moveFilesToDestination(m_fileEntryList);

    ui->progressBarFileOpreation->setVisible(true);
//    QString displayMessage;
//    switch(fileState)
//    {
//        case FileStates::success:
//            displayMessage = QString("Operation Successful");
//            break;
//        case FileStates::fileAlreadyExists:
//            displayMessage = QString("File already exists!");
//            break;
//        case FileStates::couldNotDeleteSourceFile:
//            displayMessage = QString("Could not delete source file!");
//            break;
//        case FileStates::copyFailed:
//            displayMessage = QString("Could not copy source file!");
//            break;
//        case FileStates::undefined:
//        default:
//            displayMessage = QString("undefined error state!");
//            break;
//    }

//    QMessageBox::StandardButton reply = QMessageBox::question(this,
//                                       "Move Filesto destination", displayMessage,
//                              QMessageBox::Yes | QMessageBox::No);
    //    if(reply == QMessageBox::Yes) {
    //        QApplication::quit();
    //    } else {
    //        qDebug() << "No is clicked";
    //    }

    //ui->progressBarFileOpreation->setVisible(false);
}


