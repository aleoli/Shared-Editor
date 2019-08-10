# Composition

<u>1 main window, 4 different main pages:</u>

**1)** login form
**2)** register form
**3)** document browser
**4)** text editor
**5)** (optional) some animation to show when loading

**reqs and checks:**

```
id: 8-20 char length alphanum
id text area should not be empty
id should exist
pwd: 8-20 char lenght alphanum
pwd should not be empty
pwd should correspond to id
```

------

# Main Windows

![UML Class Diagram](<http://www.plantuml.com/plantuml/png/ZPHDJnmz3CVl-oaYJi-3j4XSEG5TMY7nqi9Mg2VadPXPInbdb3WTjbM_UnE6e60XoXusCp__7R_4a-Ce42GDRj4vYD4CG7n7RFraVY-MvhvpFkAfy97P917FXMt9QXGmqYyDdU-lavJbdEzRsv_5SxHi8lximv235G-oH-RR1J6UMP82nacJ9nSLY17jsjzWJr4m50VOe2kMI3r_7qk4riMObmoA88tlfO03a5Eiy_n0OJZJ9frnA_wQmqq-Q2l3W0h55DwWqEEF_BkyNAsovHCuP0lXZcoFinS-Pdu3blpfIsVcylIvrAuh3YuWs5tj8zhQoRPX9rtd4ukfxvVUqWEXVMvVYXZEi-Ej1EA-4Eix1K1mJGDMrkuk-2YbO1-UzolpkCumfrAMbU_IW2mNiFD9Mi5j5xnpNmACsxlyDpxDmGnF0i9y4EDEbTmbKMI4AAZOnZkh40auwXHCZbZxCauhhuJpLwLmquLQvVqc-2bYc5iH4KAtFOMW9-Yj261jXbTwZAf1gUDJdpAFhvlMv7I5KqrhCUUZPhGLAoLzOEWjo3lpRVBS2Vqa8OpclsdhZILhfYs80IuFoFzdqNetObvMD217yXppo_N46ERacijjxt3Fx2-NPMrkeIFJUPQiYczgz_VgVV4XSJFD_n8V7XnalJxtralRNWMgneyubBMUflQf7tUHB6UfgZWW26hXd4Iz2OeCs6nmKKanr_sQNAli6DccmVq5>)

<details>
	<summary> PlantUML code </summary>
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

class Account 

class Modified {

- _userId: String
- _docId: String
- _dateTime: String
  }

class textEditor {

- _mainText: QTextDocumentLayout
- _mainTextScrollGraphWrapper: QScrollArea
- _copyBtn: button
- _cutBtn: button
- _pasteBtn: button
- _boldBtn: button
- _italicBtn: button
- _ulineBtn: button
- _newDocBtn: button
- _closeDocBtn: button
  }

class Document_browser{

- _searchBar: QLineEdit
- _newDocBtn: button
- _addDocName: QLineEdit
- _addDocBtn: button
- _scrollDocs: QScrollArea
  }

class NewDocument
{

- _name: QLineEdit
  }

class Document
{

- _id: QLineEdit
- _name: QLineEdit
- _dateYYYYMMDD: QCalendarWidget
  Activities (who did what and when)
  Type
  Dimensions
  Owner
  }

mainWindow "1" *-- "1" loginForm : contains
mainWindow "1" *-- "*" signupForm : contains
mainWindow "1" *-- "*" textEditor : contains
mainWindow "1" *-- "1..*" Document_browser : contains
Document_browser "1" *-- "*" Document : contains
Document_browser "1" *-- "1" NewDocument : contains
Account "1" --- "1" signupForm : creates
Account "1" --- "*" Modified : can be
Modified "*" --- "1" Document : can be
@enduml
</details>

P.S. button could be a different type but probably will be QPushButton

