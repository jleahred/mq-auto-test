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
    mtk::list<std::string>  right_options;
    wrong_options.push_back("Valid1");
    wrong_options.push_back("Valid 1");

    mtk::list<ae::msg::sub_question>        list_questions;


    ae::msg::sub_question question1{"This is a test 1", right_options, wrong_options};
    ae::msg::sub_question question2{"This is a test 2", right_options, wrong_options};


    list_questions.push_back(question1);
    list_questions.push_back(question2);



    std::ofstream file;
    file.open ("../data/test.yaml", std::ios::out | std::ios::trunc);
    YAML::Emitter out;
    out  << list_questions;
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


void  load_from_file(const std::string& file_name, mtk::list<ae::msg::sub_question>&        list_questions)
{
    std::ifstream file(file_name.c_str());
    mtk::list<ae::msg::sub_question>  readed_list_questions;

    YAML::Parser parser(file);

    YAML::Node doc;
    parser.GetNextDocument(doc);

    doc >>  readed_list_questions;
    file.close();
    for(auto it=readed_list_questions.begin(); it!=readed_list_questions.end(); ++it)
        list_questions.push_back(*it);
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
    list_questions.clear();
    vector_questions.clear();


    QModelIndexList selections = ui->tv_exam->selectionModel()->selectedRows();

    for(int i=0; i<selections.count(); ++i)
    {
        QModelIndex  index = selections.at(i);
        if(file_system_model->isDir(index) == false)
            load_from_file(file_system_model->filePath(index).toUtf8().data(), list_questions);
    }
    if(list_questions.size() == 0)
    {
        ui->lbl_status->setText("empty questions");
        ui->stackedWidget->setCurrentIndex(0);
    }
    for(auto it=list_questions.begin(); it!=list_questions.end(); ++it)
        vector_questions.push_back(*it);

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
    ae::msg::sub_question  random_question =  vector_questions[mtk::rand()%vector_questions.size()];


    clear_ui_options();
    configure_ui_options(random_question);
    update_counter();
}

void  auto_test::update_counter(void)
{
    ui->lbl_counter->setText(MTK_SS(status.Get().pendings <<":" << status.Get().repetitions).c_str());
}


std::string   pick_random_right_answer(const mtk::list<std::string>& answer_list)
{
    mtk::vector<std::string> answer_vector;
    for(auto it=answer_list.begin(); it!=answer_list.end(); ++it)
        answer_vector.push_back(*it);

    return answer_vector[mtk::rand()%answer_vector.size()];
}

mtk::tuple<std::string, std::string>   pick_2_random_answers(const mtk::list<std::string>& answer_list)
{
    mtk::vector<std::string> answer_vector;
    for(auto it=answer_list.begin(); it!=answer_list.end(); ++it)
        answer_vector.push_back(*it);

    unsigned first  = mtk::rand()%answer_vector.size();
    unsigned second=0;
    if(answer_vector.size() != 1)
    {
        while(second == first)
            second = mtk::rand()%answer_vector.size();
    }
    return mtk::make_tuple (
                answer_vector[first],
                answer_vector[second]
                );
}

void  auto_test::configure_ui_options(const  ae::msg::sub_question&  question_info)
{
    mtk::vector<std::string>  all_options_vector;
    for(auto it=question_info.wrong_options.begin(); it!=question_info.wrong_options.end(); ++it)
        all_options_vector.push_back(*it);

    std::string  question = question_info.question;
    std::string  right_answer;
    if(question != ""  &&  question != "~")
    {
        right_answer = pick_random_right_answer(question_info.answers);
    }
    else
    {
        mtk::tuple<std::string, std::string>  _2answers = pick_2_random_answers(question_info.answers);
        question = _2answers._0;
        right_answer = _2answers._1;
    }

    ui->lbl_question->setText(QString::fromUtf8(question.c_str()));

    //  riffle answers
    for(unsigned i=0; i<all_options_vector.size()*10; i++)
    {
        int index1 = mtk::rand()%all_options_vector.size();
        int index2 = mtk::rand()%all_options_vector.size();

        std::swap(all_options_vector[index1], all_options_vector[index2]);
    }

    unsigned number_questions = (unsigned(status.Get().options_per_question)<(question_info.wrong_options.size()+1)
                                 ?
                                     unsigned(status.Get().options_per_question)
                                   : question_info.wrong_options.size()+1);
    unsigned pos_right_question = mtk::rand() % number_questions;
    all_options_vector.push_back(right_answer);

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
        option->setStyleSheet("background-color: rgb(255, 200, 200);");
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
