1 main window, 4 different main appearance

1) login form
2) register form
3) document browser
4) text editor
-5) (optional) some animation to show when loading

reqs and checks:

    id: 8-20 char lenght alphanum
    id text area should not be empty
    id should exist
    pwd: 8-20 char lenght alphanum
    pwd should not be empty
    pwd should correspond to id
    
------------------------------------------------
3) DOCUMENT BROWSER
------------------------------------------------
------------------------------------------------
![UML Class Diagram](http://www.plantuml.com/plantuml/png/VP2_IiSm4CVtUueZqw_53gvE5RkurCLTpkIm1-sb95U3YEzkYgs8Y5EEVFBza-kpKR9jcS7Fb3CEqMyBgpqzfrWo9pZkHoudWdSOQM7uEDaFS3zyiO6CyNANc4oYNc5zPyX1x1ZZoYhwqi2jDtaL4yvuALF4807BH8Qa-y3QmEFRMWsb9bIh30z5EUtXlwkYktROTXswrdrtmXlqKOr4yt-AUjOV_YNgMKDTpoS0)

<details>
<summary> Codice PlantUML </summary>
@startuml
class Document_browser

class NewDocument
{
Name
}

class Document
{
ID
Name
Date (Creation, last edit, last opening)
Activities (who did what and when)
Type
Dimenions
Owner
}

Document_browser "1" *-- "*" Document : contains

Document_browser "1" *-- "1" NewDocument : contains
@enduml
</details>
    
    
## finestre principali

<details>
<summary> Codice PlantUML </summary>
@startuml
class mainWindow {
- _closeBtn: button
- _hideBtn: button
- _resizeBtn: button
- _logOut_logIn_Btn: button
}

class loginForm {
- _id: QLineEdit
- _pwd: QLineEdit
- _loginBtn: button
- _needToRegister: QLabel
- _signUpFormBtn: button
}


class signupForm {
- _id: QLineEdit
- _email: QLineEdit
- _confirmEmail: QLineEdit
- _pwd: QLineEdit
- _confirmPassword: QLineEdit
- _name: QLineEdit
- _surname: QLineEdit
- _ageYYYYMMDD: QCalendarWidget
- _sex: QRadioButton
- _signupBtn: button
- _loginFormBtn: button
- _alreadyRegistered: QLabel
}

class textEditor {
- _mainText: QTextDocumentLayout
- _copyBtn: button
- _cutBtn: button
- _pasteBtn: button
- _boldBtn: button
- _newDocBtn: button
- _closeDocBtn: button
}


