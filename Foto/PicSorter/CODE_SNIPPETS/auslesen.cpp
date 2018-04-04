//    QString fileString(m_urlList.at(0).toLocalFile());

//    QFile oldFile(fileString);
//    fi.setFile(oldFile);
//    QDateTime created = fi.created();
//    qDebug() << "Time : " << created.time().toString();
//    qDebug() << "Date : " << created.date().toString();

//    bool stop = true;

//    for(auto it = m_urlList.begin(); it != m_urlList.end(); ++it)
//    {

//        fe.inputFile = it->toLocalFile();
//        fe.outputFile = it->toLocalFile();
//        m_test.append(fe);
//    }

//    foreach (auto oObject, m_test)
//    {
//        qDebug() << oObject.inputFile;
//    }

