#ifndef AUTO_TEST_H
#define AUTO_TEST_H

#include <QWidget>


#include "db/msg_structs.h"
#include "support/list.hpp"
#include "support/vector.hpp"


class QFileSystemModel;
class QOption;


namespace Ui {
class auto_test;
}

class auto_test : public QWidget
{
    Q_OBJECT
    
public:
    explicit auto_test(QWidget *parent = 0);
    ~auto_test();
    
private slots:
    void on_pb_start_clicked();
    void slot_option_clicked(QOption* option);

    void on_pushButton_clicked();

private:
    Ui::auto_test *ui;

    mtk::nullable<ae::msg::sub_status>      status;
    QFileSystemModel*                       file_system_model;



    mtk::list<ae::msg::sub_question>        list_questions;
    mtk::vector<ae::msg::sub_question>     vector_questions;

    void  start_game(void);
    bool  load_questions(void);
    void  ask_random_question(void);
    void  clear_ui_options(void);
    void  configure_ui_options(const  ae::msg::sub_question& question);
    void  update_counter(void);
    void  go_congratulations(void);


    unsigned  get_options_ui_count(void) const  {  return 10;  }
    QOption*  get_option_by_index(int index);

};

#endif // AUTO_TEST_H
