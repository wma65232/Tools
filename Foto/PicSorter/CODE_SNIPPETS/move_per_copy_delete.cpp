    int fileCount = 0;
    int fileCountMax = fileEntryList.size();
    FileStates operationFinishedWith = FileStates::undefined;
    for(QList<FileEntry>::iterator it3 = fileEntryList.begin(); it3 != fileEntryList.end(); ++it3)
    {
        bool success = false;
        ui->progressBarFileOpreation->setValue((fileCount++) * 100 / fileCountMax);
        //std::this_thread::sleep_for(std::chrono::milliseconds(250)); // http://www.codebind.com/cpp-tutorial/qt-tutorials-for-beginners-qmessagebox/
        if(!QFile::exists(it3->outputFile))
        {
            success = QFile::copy(it3->inputFile, it3->outputFile);
            if(!success)
            {
                operationFinishedWith = FileStates::copyFailed;
                break;
            }

            success = QFile::remove(it3->inputFile);
            if(!success)
            {
                operationFinishedWith = FileStates::couldNotDeleteSourceFile;
                break;
            }
        }
        else
        {
            operationFinishedWith = FileStates::fileAlreadyExists;
        }
    }
    ui->progressBarFileOpreation->setValue(0);
    //return operationFinishedWith;
