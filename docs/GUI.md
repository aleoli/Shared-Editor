# Composition

<u>1 main window, 4 different main pages:</u>

**1)** login form
**2)** register form
**3)** document browser
**4)** text editor
**5)** (optional) some animation to show when loading

**reqs and checks:**

```
id: 8-20 char lenght alphanum
id text area should not be empty
id should exist
pwd: 8-20 char lenght alphanum
pwd should not be empty
pwd should correspond to id
```
------------------------------------------------

# Main Windows
![UML Class Diagram](http://www.plantuml.com/plantuml/png/ZPD1Qnin48Nl-ok4Jst0WLnzIbEdK4YQDhY4deoycgu7jAD56bLnG_zx9UmLOZ19V97vtfjzipFQwy3QSnnjrriTWXerqXEISKczT4krxQqBSCEqKhl8xAYmFHg9F0JyAw5rmqFaSdobRQlzwqvfMKBwulnu34EpKZ_kaE3M81Sm9KbA1OaS0Z0RzmW31WPV2lGER5423lHpAW4Y_fHVz3YzqW2C6grWlQFVwCTRASb-M_Ftd9QSbohf4GGAqP-XUe1V-NT_lrvdvREsG4Rx9pG37AlWEVD7RT3TrBaStquCguvSS6qzQ7EOfmYcph5kY-6PIqFE7wTLxieci-minzhrSGJYEtrmyVJoqq74z945cNJE4spdh1688EM01jRRAN1jT-vdk_Ck1V0p_mPfbhgNCriGJwWsVCVsZEPN5lMfP_o3Z13KXxHtog1HQQzPQIf_W3vscyC4tHfpQa17eNj81Bvqr7oOYwk5kbWkozbyGIlLE-Bi2cUz5ulsjhzjRfRzXldgyZBxvRJRAgcrGRNmNGNvhFiJDTT09exsFm00)

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

class textEditor {
- _mainText: QTextDocumentLayout
- _copyBtn: button
- _cutBtn: button
- _pasteBtn: button
- _boldBtn: button
- _newDocBtn: button
- _closeDocBtn: button
}

class Document_browser

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
@enduml
