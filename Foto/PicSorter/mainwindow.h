#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>
#include <QSignalMapper>
#include <QUrl>
#include <QDebug>
#include <QObject>
#include <QList>

#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>

/*
    Aufnahmekriterium für ein weiteres, ähnliches File:
    1. ist ein Dateiname doppelt (bzw. in einem weiteren Derivat enthalten)?
        => dann sollte die JPG-Datei als Datums-Referenz dienen

    2. ist ein Dateiname doppelt und nur mit einer weiteren Endung wie NEF, RW2 oder ähnlichem
    vorhanden
        => dann sollte ebenfalls die JPG-Datei (Datei mit dem kürzeren Dateinamen) als Referenz für das Datum dienen
*/


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_pushButtonPreview_clicked();
    void on_pushButtonCreate_clicked();
    void on_pushButtonCopy_clicked();
    void on_pushButtonMove_clicked();

protected:
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);
   void dropEvent(QDropEvent *event);

private:
    Ui::MainWindow *ui;

    class FileEntry
    {
      public:
        QString     inputFile;
        QString     outputFile;
        QFileInfo   inputFileInfo;
        QString     mkdirString;
        bool      needsReferenceFile = false;
    };

    enum FileStates
    {
        success = 0
        , fileAlreadyExists
        , copyFailed
        , couldNotDeleteSourceFile
        , undefined
    };

    void initializeUrlList(QList<QUrl>& urlList);
    void updateTable(QList<QUrl>& urlList);
    void updateSourceFileList(QList<QUrl>& urlList);
    void createFileEntryListFromUrls(const QList<QUrl>& urlList);
    void createDirectories(const QList<FileEntry>& entryList);
    FileStates copyFilesToDestination(QList<FileEntry>& fileEntryList);
    FileStates moveFilesToDestination(QList<FileEntry>& fileEntryList);

    bool isReferenceFile(const QFileInfo fileInfo);
    void prepareCreateDirectories();
    QString  createMkDirString(const QFileInfo &fileinfo);

    enum columns
    {
        FILE=0,
        DATE,
        MISC
    };

    int m_tableLineNo = 0;

    QList<QUrl>      m_urlList;
    QList<FileEntry> m_fileEntryList;

};

#endif // MAINWINDOW_H
