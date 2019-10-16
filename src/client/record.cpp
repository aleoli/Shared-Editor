#include "record.h"
#include "ui_record.h"
#include "account.h"
#include "string.h"

using namespace std;

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);
}

Record::~Record()
{
    delete ui;
}

/*
bool isGoodRecord(){
    return ( validNickname() && validPassword && etc );
}
*/
Account createAccount(string user, string password){
    //user = new Account();
    //return user;
}

void Record::on_pushButton_3_clicked()
{
    showLoginPage();
}
