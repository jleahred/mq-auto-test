#include <QtGui/QApplication>
#include <QIcon>

#include "auto_test.h"


#include "support/alarm.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setWindowIcon(QIcon(":/icon/green_question.svg"));

    auto_test w;
    w.show();
    
    return a.exec();
}


namespace mtk
{

void  AlarmMsg(const mtk::Alarm& alarm)
{
    std::cout << alarm << std::endl;
}

}
