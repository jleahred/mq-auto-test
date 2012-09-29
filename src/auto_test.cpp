#include "auto_test.h"
#include "ui_auto_test.h"
#include <QFileSystemModel>
#include <QDir>
#include <fstream>




auto_test::auto_test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::auto_test),
    file_system_model(new QFileSystemModel(this))
{
    ui->setupUi(this);

    QDir  dir("../data");
    file_system_model->setRootPath(dir.path());
    file_system_model->setNameFilters(QStringList() << "*.yaml");
    file_system_model->setNameFilterDisables(false);
    ui->tv_exam->setModel(file_system_model);
    ui->tv_exam->setRootIndex(file_system_model->index(dir.path()));
    ui->tv_exam->setColumnWidth(0, 500);
    ui->tv_exam->setColumnHidden(1, true);
    ui->tv_exam->setColumnHidden(2, true);
    ui->tv_exam->setHeaderHidden(true);

    ui->tv_exam->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->lbl_status->setText("");

    for(unsigned i=0; i<get_options_ui_count(); ++i)
    {
        QOption*  option = get_option_by_index(i);
        connect(option, SIGNAL(signal_clicked(QOption*)), this, SLOT(slot_option_clicked(QOption*)));
    }

    ui->stackedWidget->setCurrentIndex(0);
}

auto_test::~auto_test()
{
    delete ui;
}


void  save_test(void)
{
    mtk::list<std::string>  wrong_options;
    wrong_options.push_back("error1");
    wrong_options.push_back("error2");
    /*
    ae::msg::sub_question question("This is a test", "valid question", wrong_options);
    */

    mtk::map<ae::msg::sub_question::key_type, ae::msg::sub_question>        map_questions;


    ae::msg::sub_question question1{"This is a test 1", "valid question 1", wrong_options};
    ae::msg::sub_question question2{"This is a test 2", "valid question 2", wrong_options};


    map_questions.insert(std::make_pair(question1.get_key(), question1));
    map_questions.insert(std::make_pair(question2.get_key(), question2));



    std::ofstream file;
    file.open ("../data/test.yaml", std::ios::out | std::ios::trunc);
    YAML::Emitter out;
    out  << map_questions;
    file << out.c_str();
    file.close();
}


void auto_test::on_pb_start_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    start_game();

    //save_test();
}


void  auto_test::start_game(void)
{
    ui->lbl_status->setText("");
    status = ae::msg::sub_status(mtk::dtNowLocal(), ui->sb_repetitions->value(), ui->sb_penalizations->value(), ui->sb_options->value(), 0, ui->sb_repetitions->value());
    if(load_questions())
        ask_random_question();
}


void  load_from_file(const std::string& file_name, mtk::map<ae::msg::sub_question::key_type, ae::msg::sub_question>&        map_questions)
{
    std::ifstream file(file_name.c_str());

    YAML::Parser parser(file);

    YAML::Node doc;
    parser.GetNextDocument(doc);

    doc >>  map_questions;
    file.close();
}


QOption*  auto_test::get_option_by_index(int index)
{
#define __GET_OPTION_BY_INDEX__(__INDEX__)  \
    if(index==__INDEX__)        return  ui->option##__INDEX__;

    __GET_OPTION_BY_INDEX__(0);
    __GET_OPTION_BY_INDEX__(1);
    __GET_OPTION_BY_INDEX__(2);
    __GET_OPTION_BY_INDEX__(3);
    __GET_OPTION_BY_INDEX__(4);
    __GET_OPTION_BY_INDEX__(5);
    __GET_OPTION_BY_INDEX__(6);
    __GET_OPTION_BY_INDEX__(7);
    __GET_OPTION_BY_INDEX__(8);
    __GET_OPTION_BY_INDEX__(9);

    return 0;
}

bool  auto_test::load_questions(void)
{
    map_questions.clear();
    vector_questions.clear();


    QModelIndexList selections = ui->tv_exam->selectionModel()->selectedRows();

    for(int i=0; i<selections.count(); ++i)
    {
        QModelIndex  index = selections.at(i);
        if(file_system_model->isDir(index) == false)
            load_from_file(file_system_model->filePath(index).toStdString(), map_questions);
    }
    if(map_questions.size() == 0)
    {
        ui->lbl_status->setText("empty questions");
        ui->stackedWidget->setCurrentIndex(0);
    }
    for(auto it=map_questions.begin(); it!=map_questions.end(); ++it)
        vector_questions.push_back(it->first);

    return  (vector_questions.size() >0 ? true : false);
}


void  auto_test::clear_ui_options(void)
{
    for(unsigned i=0; i<get_options_ui_count(); ++i)
    {
        QOption*  option = get_option_by_index(i);
        option->setText("");
        option->setVisible(false);
        option->setEnabled(false);
        option->setStyleSheet("");
    }
}

void auto_test::ask_random_question(void)
{
    ae::msg::sub_question::key_type  random_index =  vector_questions[mtk::rand()%vector_questions.size()];

    ae::msg::sub_question question = map_questions.find(random_index)->second;

    clear_ui_options();
    configure_ui_options(question);
    update_counter();
}

void  auto_test::update_counter(void)
{
    ui->lbl_counter->setText(MTK_SS(status.Get().pendings <<":" << status.Get().repetitions).c_str());
}


void  auto_test::configure_ui_options(const  ae::msg::sub_question&  question)
{
    mtk::vector<std::string>  all_options_vector;
    for(auto it=question.wrong_options.begin(); it!=question.wrong_options.end(); ++it)
        all_options_vector.push_back(*it);

    ui->lbl_question->setText(QString::fromUtf8(question.question.c_str()));

    //  riffle answers
    for(unsigned i=0; i<all_options_vector.size()*10; i++)
    {
        int index1 = mtk::rand()%all_options_vector.size();
        int index2 = mtk::rand()%all_options_vector.size();

        std::swap(all_options_vector[index1], all_options_vector[index2]);
    }

    unsigned number_questions = (unsigned(status.Get().options_per_question)<question.wrong_options.size()+1
                                 ?
                                     unsigned(status.Get().options_per_question+1)
                                   : question.wrong_options.size()+1);
    unsigned pos_right_question = mtk::rand() % number_questions;
    all_options_vector.push_back(question.answer);
    std::swap(all_options_vector[pos_right_question], all_options_vector[all_options_vector.size()-1]);

    for(unsigned i=0; i<number_questions; ++i)
    {
        QOption* option = get_option_by_index(i);
        option->setText(QString::fromUtf8(all_options_vector[i].c_str()));
        option->setEnabled(true);
        option->setVisible(true);
        if(i==pos_right_question)
            option->correct_option = true;
        else
            option->correct_option = false;
    }
}


void auto_test::slot_option_clicked(QOption* option)
{
    if(option->correct_option == false)
    {
        option->setStyleSheet("color: red;");
        status.Get().pendings += status.Get().penalizations;
        status.Get().failed += 1;
        update_counter();
    }
    else
    {
        status.Get().pendings -= 1;
        update_counter();
        if(status.Get().pendings == 0)
            go_congratulations();
        else
            ask_random_question();
    }
}

void  auto_test::go_congratulations(void)
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->pte_report->setPlainText(QString::fromUtf8(MTK_SS(
                                            "Time:  " << mtk::dtNowLocal() - status.Get().started << std::endl  << std::endl
                                     << YAML::string_from_yaml(status)
                                                       ).c_str()));
}

void auto_test::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
